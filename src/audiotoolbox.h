#ifndef AUDIOTOOLBOX_H
#define AUDIOTOOLBOX_H

class AudioToolbox {
public:
    AudioToolbox(int argc, char **argv);
    virtual ~AudioToolbox();

    int run();

private:
    int     _argc;
    char    **_argv;
    bool    _verbose;
    float   _threshold;
    int     _width, _height;
    char    *_input, *_histogram, *_spectrogram, *_combo;

    void _showUsage();
};

#endif // AUDIOTOOLBOX_H
