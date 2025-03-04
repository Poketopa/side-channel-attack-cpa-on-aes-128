#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define _FOLD_ "/Users/lhs/Desktop/subchannel/"
#define TraceFN "AES.traces"
#define PlaintextFN "plaintext.txt"
#define CiphertextFN "ciphertext.txt"
#define startpoint 0
#define endpoint 125002

static unsigned char InvSbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

void CPA() {
    unsigned char **ciphertext = NULL;
    float **data;
    double *Sx, *Sxx, *Sxy, *corrT;
    double Sy, Syy, max;
    unsigned char temp[34], x, y, iv, hw_iv;
    char buf[256];
    int TraceLength, TraceNum, i, j, key, k, maxkey;
    FILE *rfp, *wfp;

    // 파일 경로 생성 및 파일 열기
    sprintf(buf, "%s%s", _FOLD_, TraceFN);
    rfp = fopen(buf, "rb");
    if (rfp == NULL) {
        printf("File Open Error: Unable to open %s\n", buf);
        return;
    }

    fread(&TraceLength, sizeof(int), 1, rfp);
    fread(&TraceNum, sizeof(int), 1, rfp);

    data = (float **)calloc(TraceNum, sizeof(float *));
    for (i = 0; i < TraceNum; i++) {
        data[i] = (float *)calloc(TraceLength, sizeof(float));
        fread(data[i], sizeof(float), TraceLength, rfp);
    }
    fclose(rfp);

    sprintf(buf, "%s%s", _FOLD_, CiphertextFN);
    rfp = fopen(buf, "r");
    ciphertext = (unsigned char **)calloc(TraceNum, sizeof(unsigned char *));
    for (i = 0; i < TraceNum; i++) {
        ciphertext[i] = (unsigned char *)calloc(16, sizeof(unsigned char));
        fgets(temp, 34, rfp);
        for (j = 0; j < 16; j++) {
            x = temp[2 * j];
            y = temp[2 * j + 1];
            if (x >= 'A' && x <= 'F') x = x - 'A' + 10;
            else if (x >= '0' && x <= '9') x -= '0';
            if (y >= 'A' && y <= 'F') y = y - 'A' + 10;
            else if (y >= '0' && y <= '9') y -= '0';
            ciphertext[i][j] = x * 16 + y;
        }
    }
    fclose(rfp);

    Sx = (double *)calloc(TraceLength, sizeof(double));
    Sxx = (double *)calloc(TraceLength, sizeof(double));
    Sxy = (double *)calloc(TraceLength, sizeof(double));
    corrT = (double *)calloc(TraceLength, sizeof(double));

    for (i = 0; i < TraceNum; i++) {
        for (j = startpoint; j < endpoint; j++) {
            Sx[j] += data[i][j];
            Sxx[j] += data[i][j] * data[i][j];
        }
    }

    for (i = 0; i < 16; i++) {
        max = 0;
        maxkey = 0;
        for (key = 0; key < 256; key++) {
            Sy = 0;
            Syy = 0;
            memset(Sxy, 0, sizeof(double) * TraceLength);

            for (j = 0; j < TraceNum; j++) {
                iv = InvSbox[ciphertext[j][i] ^ key];
                hw_iv = __builtin_popcount(iv);
                Sy += hw_iv;
                Syy += hw_iv * hw_iv;

                for (k = startpoint; k < endpoint; k++) {
                    Sxy[k] += hw_iv * data[j][k];
                }
            }

            for (k = startpoint; k < endpoint; k++) {
                corrT[k] = ((double)TraceNum * Sxy[k] - Sx[k] * Sy) /
                           sqrt(((double)TraceNum * Sxx[k] - Sx[k] * Sx[k]) * ((double)TraceNum * Syy - Sy * Sy));
                if (fabs(corrT[k]) > max) {
                    maxkey = key;
                    max = fabs(corrT[k]);
                }
            }

            // corrtrace 저장
            sprintf(buf, "%scorrtrace/%02dth_block_%02x.corrtrace", _FOLD_, i, key);
            wfp = fopen(buf, "wb");
            if (wfp != NULL) {
                fwrite(corrT, sizeof(double), TraceLength, wfp);
                fclose(wfp);
            }
        }
        printf("Key Byte %d: %02X, Max Correlation: %f\n", i, maxkey, max);
    }

    free(Sx);
    free(Sxx);
    free(Sxy);
    free(corrT);
    for (i = 0; i < TraceNum; i++) {
        free(data[i]);
        free(ciphertext[i]);
    }
    free(data);
    free(ciphertext);
}

int main() {
    CPA();
    return 0;
}
