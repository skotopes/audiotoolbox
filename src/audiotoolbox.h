#ifndef AUDIOTOOLBOX_H
#define AUDIOTOOLBOX_H

class AVFile;
class AVImageRGBA;
class AVHistogram;
class AVSpectrogram;
class AVSplitter;

class AudioToolbox {
public:
    AudioToolbox(int argc, char **argv);
    virtual ~AudioToolbox();

    int run();

private:
    int             _argc;
    char            **_argv;
    bool            _verbose;
    float           _threshold;
    int             _width, _height;

    char            *_input_filename;
    char            *_histogram_filename;
    char            *_spectrogram_filename;
    char            *_combo_filename;

    AVFile          *_input;
    AVSplitter      *_splitter;

    AVImageRGBA     *_spectrogram_image;
    AVImageRGBA     *_histogram_image;
    AVImageRGBA     *_combo_image;

    AVHistogram     *_histogram;
    AVSpectrogram   *_spectrogram;

    void _log(const char *message);
    void _report();
    void _showUsage();
};

#endif // AUDIOTOOLBOX_H
