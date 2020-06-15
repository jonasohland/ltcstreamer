#include <errno.h>
#include <ltc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if _WIN32
#include <fcntl.h>
#include <io.h>
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define read _read
#define write _write
#ifdef _WIN64
#define ssize_t __int64
#else
#define ssize_t long
#endif
#endif

int usage(int retcode)
{
    puts("usage: ltcstreamer <framerate> <samplerate> <ltc_frame_div>");
    return retcode;
}

int main(int argc, const char** argv)
{
    if ((argc != 2) && (argc != 4))
        return usage(1);

    if (argc == 2) {
        if (strcmp(*(argv + 1), "-h") || strcmp(*(argv + 1), "--help"))
            return usage(0);
        return usage(1);
    }

    double frate = atof(*(argv + 1));
    long srate   = atol(*(argv + 2));
    long fdiv    = atol(*(argv + 3));

    if (fdiv < 1)
        return 1;

    LTCDecoder* decoder = ltc_decoder_create((srate / frate), 32);
    LTCFrameExt frame;

    unsigned char data[256];
    char outbf[13];

    long fcount         = 0;
    long long int total = 0;

#ifdef _WIN32
    // see https://msdn.microsoft.com/en-us/library/ktss1a9b.aspx and
    // https://github.com/x42/libltc/issues/18
    _set_fmode(_O_BINARY);
    freopen(NULL, "r", stdin);
#endif

    while (1) {
        ssize_t red = read(STDIN_FILENO, data, 256);

        if (red < 1)
            break;

        ltc_decoder_write(decoder, data, red, total);

        while (ltc_decoder_read(decoder, &frame)) {

            ++fcount;

            if (fcount == fdiv) {
                fcount = 0;

                SMPTETimecode stime;
                ltc_frame_to_time(&stime, &frame.ltc, 1);

                snprintf(outbf, 13, "%02d:%02d:%02d%c%02d\n",
                       stime.hours,
                       stime.mins,
                       stime.secs,
                       (frame.ltc.dfbit) ? '.' : ':',
                       stime.frame);

                write(STDOUT_FILENO, outbf, strlen(outbf));
            }
        }
        total += red;
    }

    ltc_decoder_free(decoder);
    return errno;
}
