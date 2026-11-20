#ifndef __ZBAR_API_H__
#define __ZBAR_API_H__

#ifdef __cplusplus
    extern "C" {
#endif


int zxing_scan_qrcode(int width, int height, unsigned char *yuvdata, char *stack, int stacklen);

#ifdef __cplusplus
    }
#endif

#endif

