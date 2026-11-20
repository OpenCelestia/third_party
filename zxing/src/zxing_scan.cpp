#include <stdio.h>
#include <string.h>
#include <string>

#include "zxing_scan.h"
#include "ReadBarcode.h"

using namespace ZXing;

/*
 * WIFI QR entry for tablet setting / nebula pair.
 *
 * Engine: modern zxing-cpp (vendored under zxing-cpp/, v3.1.1) with QR-only
 * ReaderOptions. Replaces the old ~2008 C++ ZXing amalgamation and the former
 * quirc fallback — zxing-cpp already solves the clear Xiaomi WIFI stills that
 * used to hit Detector OK / Decoder FAIL (e.g. 04/01.png).
 */

static int zxing_cpp_try_once(unsigned char *ydata, int width, int height,
                              char *stack, int stacklen)
{
        if (!ydata || width <= 0 || height <= 0 || !stack || stacklen <= 1)
                return -1;

        try {
                ImageView image(ydata, width, height, ImageFormat::Lum);
                ReaderOptions opts;
                opts.setFormats(BarcodeFormat::QRCode);
                opts.setTryHarder(true);
                opts.setTryRotate(true);
                /* Camera preview is not a pure synthetic bitmap. */
                opts.setIsPure(false);

                Barcode r = ReadBarcode(image, opts);
                if (!r.isValid())
                        return -1;

                const std::string &text = r.text();
                if (text.empty() || (int)text.size() >= stacklen)
                        return -1;
                memcpy(stack, text.data(), text.size());
                stack[text.size()] = '\0';
                return (int)text.size();
        } catch (...) {
                return -1;
        }
}

extern "C" int zxing_scan_qrcode(int width, int height, unsigned char *ydata,
                                 char *stack, int stacklen)
{
        if (!ydata || !stack || stacklen <= 1 || width <= 0 || height <= 0)
                return -1;

        int result = zxing_cpp_try_once(ydata, width, height, stack, stacklen);
        if (result > 0) {
                fprintf(stderr, "zxing_scan: zxing-cpp ok len=%d\n", result);
                return result;
        }

        /* Preview frames often fail; avoid spam at QR_PAIR_ZXING_EVERY_N. */
        return -1;
}
