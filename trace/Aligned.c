#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define _FOLD_ "/Users/lhs/Desktop/github/myArchive/School/3학년 2학기/부채널분석/실습/"
#define TraceFN "AES.traces"
#define AlignedTraceFN "AlignedAES.traces"

// 두 신호 배열 x와 y의 공분산 계산 함수
double cov(float *x, float *y, int size) {
    double Sxy = 0, Sx = 0, Sy = 0;
    int i;
    for (i = 0; i < size; i++) {
        Sxy += x[i] * y[i]; // E(XY)
        Sx += x[i];
        Sy += y[i];
    }
    return (Sxy - Sx * Sy / (double)size) / (double)size; // 공분산 계산
}

// data2를 data1에 맞춰 정렬하는 함수
void subalign(float *data1, float *data2, int windowsize, int stepsize, int threshold, int TraceLength) {
    int m, j, size, maxcovpos, k;
    float *x, *y;
    double covval, maxcov;

    // TraceLength 범위 내에서 stepsize 간격으로 부분 파형 정렬
    for (m = 0; m < (TraceLength - windowsize); m += stepsize) {
        maxcovpos = 0;
        maxcov = 0;

        // -threshold부터 threshold까지 이동하면서 공분산 최대값 찾기
        for (j = -threshold; j < threshold; j++) {
            if (j < 0) { // j가 음수일 때 data1과 data2의 시작 위치 조정
                x = data1 + m;
                y = data2 + m - j;
                size = windowsize + j;
            } else { // j가 양수일 때 data1과 data2의 시작 위치 조정
                x = data1 + m + j;
                y = data2 + m;
                size = windowsize - j;
            }

            // 현재 위치에서 공분산 계산
            covval = cov(x, y, size);

            // 최대 공분산 값과 그 위치 저장
            if (covval > maxcov) {
                maxcovpos = j;
                maxcov = covval;
            }
        }

        // 최대 공분산 위치에 따라 data2를 data1에 맞춰 정렬
        if (maxcovpos < 0) { // 음수일 경우 앞쪽으로 이동
            for (k = m; k < (TraceLength + maxcovpos); k++) {
                data2[k] = data2[k - maxcovpos];
            }
        } else { // 양수일 경우 뒤쪽으로 이동
            for (k = (TraceLength - maxcovpos - 1); k >= m; k--) {
                data2[k + maxcovpos] = data2[k];
            }
        }
    }
}

// AES 전력 파형을 정렬하여 새로운 파일에 저장하는 함수
void Alignment() {
    int windowsize = 500;  // 부분 파형의 길이
    int stepsize = 450;    // 다음 파형으로 넘어갈 때 이동할 간격
    int threshold = 100;   // 좌우로 이동할 범위 (최대 공분산 찾기 위해)
    char buf[256];
    FILE *rfp, *wfp;
    int TraceLength, TraceNum, i;

    float *data, *data1;

    // 원본 파일 경로 설정 및 열기
    sprintf(buf, "%s%s", _FOLD_, TraceFN);
    rfp = fopen(buf, "rb");
    if (rfp == NULL) {
        printf("File Open Error for reading: %s\n", buf);
        return;
    }

    // 정렬된 데이터를 저장할 파일 경로 설정 및 열기
    sprintf(buf, "%s%s", _FOLD_, AlignedTraceFN);
    wfp = fopen(buf, "wb");
    if (wfp == NULL) {
        printf("File Open Error for writing: %s\n", buf);
        fclose(rfp);
        return;
    }

    // TraceLength와 TraceNum을 읽어 저장 (각 파형의 길이와 총 개수)
    fread(&TraceLength, sizeof(int), 1, rfp);
    fwrite(&TraceLength, sizeof(int), 1, wfp);
    fread(&TraceNum, sizeof(int), 1, rfp);
    fwrite(&TraceNum, sizeof(int), 1, wfp);

    // 데이터 메모리 할당
    data = (float *)calloc(TraceLength, sizeof(float));
    data1 = (float *)calloc(TraceLength, sizeof(float));

    // 첫 번째 파형을 읽어 data에 저장하고 파일에 기록
    fread(data, sizeof(float), TraceLength, rfp);
    fwrite(data, sizeof(float), TraceLength, wfp);

    // 각 파형을 읽고 첫 번째 파형에 맞춰 정렬 후 파일에 기록
    for (i = 1; i < TraceNum; i++) {
        fread(data1, sizeof(float), TraceLength, rfp); // 다음 파형 읽기
        subalign(data, data1, windowsize, stepsize, threshold, TraceLength); // 정렬 수행
        fwrite(data1, sizeof(float), TraceLength, wfp); // 정렬된 파형을 파일에 기록
        printf("Aligned Trace %d/%d\n", i + 1, TraceNum); // 진행 상황 출력
    }

    // 파일 닫기 및 메모리 해제
    fclose(rfp);
    fclose(wfp);
    free(data);
    free(data1);

    printf("Alignment completed. Aligned traces saved to %s\n", AlignedTraceFN);
}

int main() {
    Alignment(); // 전력 파형 정렬 함수 호출
    return 0;
}