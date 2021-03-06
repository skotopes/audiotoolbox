#include "avfile.h"
#include "avobject.h"
#include "avexception.h"
#include "memring.h"

extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}


static volatile bool ffmpeginit = false;

AVFile::AVFile(int sample_rate, int channels) :
    formatCtx(0), codecCtx(0), swrCtx(0), audioStream(-1),
    _io_object(0), _io_sample_rate(sample_rate), _io_channels(channels),
    do_shutdown(false), eof(false), position(0), seek_to(-1)
{
    if (!ffmpeginit) {
        av_register_all();
        avcodec_register_all();
        ffmpeginit = true;
    }
}

AVFile::~AVFile()
{
    close();
}

void AVFile::open(const char *url)
{
    if (formatCtx)
        throw AVException("Programming error: i already did it");

    if (avformat_open_input(&formatCtx, url, 0, 0) < 0)
        throw AVException("Unable to open media");

    if (avformat_find_stream_info(formatCtx, 0) < 0)
        throw AVException("Unable to find streams in media");

    AVCodec *codec;
    audioStream = av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    if (audioStream < 0)
        throw AVException("No audio stream found");    

    codecCtx = formatCtx->streams[audioStream]->codec;
    if (avcodec_open2(codecCtx, codec, 0) < 0)
        throw AVException("Could not open codec");

    if (!codecCtx->channel_layout)
        codecCtx->channel_layout = av_get_default_channel_layout(codecCtx->channels);

    // TODO: stream params can be changed on the fly, add moar checks
    if (codecCtx->channel_layout != av_get_default_channel_layout(_io_channels) ||
            codecCtx->sample_fmt != AV_SAMPLE_FMT_FLT ||
            codecCtx->sample_rate != _io_sample_rate) {
        allocSWR();
    }
}

void AVFile::close()
{
    if (codecCtx) {
        avcodec_close(codecCtx);
        codecCtx = 0;
    }

    if (formatCtx) {
        avformat_close_input(&formatCtx);
    }

    if (swrCtx) {
        swr_free(&swrCtx);
    }

    audioStream = -1;
}

float AVFile::getDurationTime() {
    return (float) formatCtx->duration / AV_TIME_BASE;
}

size_t AVFile::getDurationSamples() {
    return formatCtx->duration * _io_sample_rate / AV_TIME_BASE;
}

float AVFile::getPosition() {
    AVStream * s = formatCtx->streams[audioStream];
    return (float) position * s->time_base.num / s->time_base.den;
}

size_t AVFile::getBitrate() {
    return formatCtx->bit_rate;
}

size_t AVFile::getCodecBitrate() {
    return codecCtx->bit_rate;
}

int AVFile::getCodecSamplerate() {
    return codecCtx->sample_rate;
}

int AVFile::getCodecChannels() {
    return codecCtx->channels;
}

int AVFile::getIOSamplerate() {
    return _io_sample_rate;
}

int AVFile::getIOChannels() {
    return _io_channels;
}

AVFile::Progress AVFile::getProgress() {
    AVFile::Progress p;
    p.duration = getDurationTime();
    p.position = getPosition();

    return p;
}

void AVFile::seekToPositionPercent(float p)
{
    if (0. < p && p < 1.) {
        AVStream * s = formatCtx->streams[audioStream];
        seek_to = av_rescale(p * formatCtx->duration, s->time_base.den, AV_TIME_BASE * s->time_base.num);
    }
}

// Protected
void AVFile::decoderLoop(AVObject *io_object)
{
    _io_object = io_object;
    eof = false;

    AVFrame frame;
    int got_frame;

    AVPacket packet;
    int packet_size;
    uint8_t *packet_data;
    av_init_packet(&packet);

    uint8_t * shadow = reinterpret_cast<uint8_t*>(av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE * 4));

    while (av_read_frame(formatCtx, &packet) == 0) {
        if (packet.stream_index == audioStream) {
            // make shure that we will be able to free it later
            packet_size = packet.size;
            packet_data = packet.data;

            // decode frames till packet contains data
            while (packet.size > 0) {
                avcodec_get_frame_defaults(&frame);
                int len = avcodec_decode_audio4(codecCtx, &frame, &got_frame, &packet);
                if (len < 0) {
                    break; // probably corrupted packet
                }

                packet.data += len;
                packet.size -= len;

                if (got_frame) {
                    got_frame = 0;

                    if (swrCtx) {
                        uint8_t *shadow_array[] = { shadow };
                        const uint8_t **input_array = (const uint8_t **)frame.extended_data;
                        // todo: check original code^ some nasty shit inside
                        int ret = swr_convert(swrCtx, shadow_array, AVCODEC_MAX_AUDIO_FRAME_SIZE, input_array, frame.nb_samples * _io_channels);
                        if (ret > 0) {
                            fillRing(reinterpret_cast<float *>(shadow), ret);
                        }
                    } else {
                        fillRing(reinterpret_cast<float *>(frame.data[0]), frame.nb_samples * _io_channels);
                    }

                    // update position
                    if (frame.pts != AV_NOPTS_VALUE) {
                        position = frame.pts;
                    } else if (packet.pts != AV_NOPTS_VALUE) {
                        position = packet.pts;
                    } else {
                        position = 0;
                    }
                }
                // hurry up, no time to decode one more frame
                if (do_shutdown)
                    break;
            }

            // restore original size and pointer
            packet.size = packet_size;
            packet.data = packet_data;
        }
        // free packet data, reuse structure
        av_free_packet(&packet);
        // complete decoding thread shutdown
        if (do_shutdown) {
            do_shutdown = false;
            break;
        }

        if (seek_to > -1) {
            int flags = AVSEEK_FLAG_ANY;
            if (seek_to < position)
                flags = flags | AVSEEK_FLAG_BACKWARD;
            av_seek_frame(formatCtx, audioStream, seek_to, flags);
            seek_to = -1;
        }
    }
    av_free(shadow);
    eof = true;

    _io_object = 0;
}

void AVFile::allocSWR()
{
    swrCtx = swr_alloc_set_opts(0, av_get_default_channel_layout(1), AV_SAMPLE_FMT_FLT, 44100,
                                codecCtx->channel_layout, codecCtx->sample_fmt, codecCtx->sample_rate,
                                0, 0);

    if (!swrCtx)
        throw AVException("Unable to allocate swresample context");

    swr_init(swrCtx);
}

void AVFile::fillRing(float * buffer, size_t size)
{
    while (size > 0) {
        size_t ret = _io_object->push(buffer, size);
        buffer += ret;
        size -= ret;
    }
}
