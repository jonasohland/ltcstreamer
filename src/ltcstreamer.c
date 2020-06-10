#include <errno.h>
#include <ltc.h>
#include <printf.h>
#include <unistd.h>


int main(int argc, const char** argv)
{
    LTCDecoder* decoder = ltc_decoder_create(1920, 32);
    LTCFrameExt frame;

    unsigned char data[256];

    int fcount          = 0;
    long long int total = 0;

#ifdef _WIN32
    // see https://msdn.microsoft.com/en-us/library/ktss1a9b.aspx and
    // https://github.com/x42/libltc/issues/18
    _set_fmode(_O_BINARY);
    freopen(NULL, "r", stdin);
#endif


    while (1) {
        ssize_t red = read(STDIN_FILENO, data, 256);

        if (red == -1)
            break;

        ltc_decoder_write(decoder, data, red, total);

        while (ltc_decoder_read(decoder, &frame)) {

            ++fcount;

            if (fcount == 2) {
                fcount = 0;

                SMPTETimecode stime;
                ltc_frame_to_time(&stime, &frame.ltc, 1);

                printf("%02d:%02d:%02d%c%02d\n",
                       stime.hours,
                       stime.mins,
                       stime.secs,
                       (frame.ltc.dfbit) ? '.' : ':',
                       stime.frame);
            }
        }
        total += red;
    }

    ltc_decoder_free(decoder);
}
