#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <memory.h>
#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifndef HAVE_STRRCHR
//#define strrchr rindex
#endif
#endif

#include <stdlib.h>
#include <ctype.h>

#define LINEBUFSIZE 256
#define MAXVALUE 1e23
#define SEED 1
#define abs(x) ((x>=0) ? (x) : (-(x)))
#define sign(x) ((x>=0) ? 1 : (-1))
#define LOG2 (0.693147180559945f)
#define MEL 1127.01048f
#define EZERO (-1.0E10f)
#define	B0    0x00000001
#define	B28   0x10000000
#define	B31   0x80000000
#define	B31_  0x7fffffff
#define	Z     0x00000000
#define _RAND_MAX 32767

#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4


#ifdef float
#define real float
#define FORMAT "float"
#else
#define real float
#define FORMAT "float"
#endif


#ifndef PI
#define PI  3.14159265358979323846f
#endif                          /* PI */

#ifndef PI2
#define PI2 6.28318530717958647692f
#endif                          /* PI2 */

#ifndef M_PI
#define M_PI  3.1415926535897932384626433832795
#endif                          /* M_PI */

#ifndef M_2PI
#define M_2PI 6.2831853071795864769252867665590f
#endif                          /* M_2PI */

#define LN_TO_LOG 4.3429448190325182765f

#define LZERO (-1.0e+10f)
#define LSMALL (-0.5e+10f)

/* #ifndef ABS(x) */
#define ABS(x) ((x < 0.0f) ? -x : x)
/* #endif */

#ifdef __BIG_ENDIAN
#if __BYTE_ORDER == __BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif
#endif

/* enum for Boolean */
typedef enum _Boolean { FA, TR } Boolean;

/* enum for window type */
typedef enum _Window {
    BLACKMAN,
    HAMMING,
    HANNING,
    BARTLETT,
    TRAPEZOID,
    RECTANGULAR
} Window;

/* struct for complex */
typedef struct {
    float re;
    float im;
} complex;

/* struct for Gaussian distribution */
typedef struct _Gauss {
    float* mean;
    float* var;
    float** cov;
    float** inv;
    float gconst;
} Gauss;

/* structure for GMM */
typedef struct _GMM {
    int nmix;
    int dim;
    Boolean full;
    float* weight;
    Gauss* gauss;
} GMM;

typedef struct _deltawindow {
    size_t win_size;
    size_t win_max_width;
    int* win_l_width;
    int* win_r_width;
    float** win_coefficient;
} DELTAWINDOW;

/* structure for wavsplit and wavjoin */
typedef struct _wavfile {
    int file_size;               /* file size */
    int fmt_chunk_size;          /* size of 'fmt ' chunk (byte) */
    int data_chunk_size;         /* size of 'data' chunk (byte) */
    short format_id;             /* format ID (PCM(1) or IEEE float(3)) */
    short channel_num;           /* mono:1¡¤stereo:2 */
    int sample_freq;             /* sampling frequency (Hz) */
    int byte_per_sec;            /* byte per second */
    short block_size;            /* 16bit, mono => 16bit*1=2byte */
    short bit_per_sample;        /* bit per sample */
    short extended_size;         /* size of 'extension' */

    char input_data_type;
    char format_type;

    char* data;                  /* waveform data */

} Wavfile;

typedef struct _filelist {
    int num;
    char** name;
} Filelist;

/* library routines */

/*-
int fwritex(void* ptr, const size_t size, const int nitems, FILE* fp);
int freadx(void* ptr, const size_t size, const int nitems, FILE* fp);
int fwritef(float* ptr, const size_t size, const int nitems, FILE* fp);
int freadf(float* ptr, const size_t size, const int nitems, FILE* fp);
*/
void fillz(void* ptr, const size_t size, const int nitem);


short* sgetmem(const int leng);
float* dgetmem(const int leng);
float* fgetmem(const int leng);
/* real *rgetmem (const int leng); */
char* getmem(const size_t leng, const size_t size);

void movem(void* a, void* b, const size_t size, const int nitem);


void c2sp(float* c, const int m, float* x, float* y, const int l);
int dft(float* pReal, float* pImag, const int nDFTLength);



int fft(float* x, float* y, const int m);
int ifft(float* x, float* y, const int m);


int fftr(float* x, float* y, const int m);

void freqt(float* c1, const int m1, float* c2, const int m2, const float a);
void gc2gc(float* c1, const int m1, const float g1, float* c2, const int m2,
    const float g2);
void gnorm(float* c1, float* c2, int m, const float g);
void ignorm(float* c1, float* c2, int m, const float g);
void mgc2mgc(float* c1, const int m1, const float a1, const float g1,
    float* c2, const int m2, const float a2, const float g2);
void mgc2sp(float* mgc, const int m, const float a, const float g, float* x,
    float* y, const int flng);
