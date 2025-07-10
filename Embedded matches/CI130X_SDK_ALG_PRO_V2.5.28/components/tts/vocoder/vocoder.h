#pragma once
//#include "norm_acu_lab.h"
#include "ci_tts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


// Commands for FFT (This is the same as FFTW)
#define FFT_FORWARD 1
#define FFT_BACKWARD 2
#define FFT_ESTIMATE 3


/*#############################################################################################*/
/*############################################################################################*/
//��������
	// Complex number for FFT


typedef float fft_complex[2];
// Struct used for FFT
typedef struct {
	int n;
	int sign;
	unsigned int flags;
	fft_complex* c_in;
	float* in;
	fft_complex* c_out;
	float* out;
	float* input;
	int* ip;
	float* w;
} fft_plan;
// Minimum phase analysis from logarithmic power spectrum
typedef struct {
	int fft_size;
	float* log_spectrum;
	fft_complex* minimum_phase_spectrum;
	fft_complex* cepstrum;
	fft_plan inverse_fft;
	fft_plan forward_fft;
} MinimumPhaseAnalysis;
// Inverse FFT in the real sequence
typedef struct {
	int fft_size;
	float* waveform;
	fft_complex* spectrum;
	fft_plan inverse_fft;
} InverseRealFFT;
// Forward FFT in the real sequence
typedef struct {
	int fft_size;
	float* waveform;
	fft_complex* spectrum;
	fft_plan forward_fft;
} ForwardRealFFT;

/*############################################################################################*/
/*############################################################################################*/
//������

//��������ģ��
void world(float *Y, float* sp_fe, float* f0_fe, const float* ap_fe, int number_frame, Buffer_split* buffer);
//��������ģ��
void WaveformSynthesis(int f0_length, float frame_period, int fs, float* f0, float* spectrogram, float** aperiodicity, int y_length, float* Y, Buffer_split* buffer);

void mgc2sp_oneframe(float* mgc, float* sp_fe, Buffer_split* buffer);
//��������ģ��
int GetNumberOfPulses(float* interpolated_f0, int y_length,  int fs, int* pulse_locations_index, Buffer_split* buffer );
//��ֵ����
void interp1(const float* x, const float* y, int x_length, int fs, int xi_length, float* yi);
void histc(const float* x, int x_length, const int fs, int edges_length, int* index);
void GetTemporalParametersForTimeBase(const float* f0, int f0_length, int y_length, float* coarse_time_axis, float* coarse_f0, float* coarse_vuv);
//�Ƴ�DCֵ
void GetDCRemover(int fft_size, float* dc_remover);
/*############################################################################################*/
void InitializeMinimumPhaseAnalysis(int fft_size, MinimumPhaseAnalysis* minimum_phase);
void InitializeInverseRealFFT(int fft_size, InverseRealFFT* inverse_real_fft);
void InitializeForwardRealFFT(int fft_size, ForwardRealFFT* forward_real_fft);
fft_plan fft_plan_dft_r2c_1d(int n, float* in, fft_complex* out, unsigned int flags);
fft_plan fft_plan_dft_1d(int n, fft_complex* in, fft_complex* out, int sign, unsigned int flags);
fft_plan fft_plan_dft_c2r_1d(int n, fft_complex* in, float* out, unsigned int flags);


void makewt(int nw, int* ip, float* w);
void makect(int nc, int* ip, float* c);
void makeipt(int nw, int* ip);
/*############################################################################################*/
/*############################################################################################*/
/*void GetOneFrameSegment(float current_vuv, int noise_size,
	const float* const* spectrogram, int fft_size,
	const float* const* aperiodicity, int f0_length, float frame_period,
	float current_time, float fractional_time_shift, int fs,
	const ForwardRealFFT* forward_real_fft,
	const InverseRealFFT* inverse_real_fft,
	const MinimumPhaseAnalysis* minimum_phase, const float* dc_remover,
	float* response);
*/
void GetOneFrameSegment(int y_length, int origin, float* excitation_signal,
	float* spectrogram, int f0_length,
	ForwardRealFFT* forward_real_fft,
	//const InverseRealFFT* inverse_real_fft,
        MinimumPhaseAnalysis* minimum_phase,float* response, Buffer_split* buffer);
/*############################################################################################*/
void GetSpectralEnvelope(float current_time, float frame_period,
	int f0_length, const float* const* spectrogram, int fft_size,
	float* spectral_envelope);
/*############################################################################################*/
void GetAperiodicRatio(float current_time, float frame_period,
	int f0_length, const float* const* aperiodicity, int fft_size,
	float* aperiodic_spectrum);
float GetSafeAperiodicity(float x);
/*############################################################################################*/
void GetPeriodicResponse(int fft_size, const float* spectrum,
	const float* aperiodic_ratio, float current_vuv,
	const InverseRealFFT* inverse_real_fft,
	const MinimumPhaseAnalysis* minimum_phase, const float* dc_remover,
	float fractional_time_shift, int fs, float* periodic_response);
void GetMinimumPhaseSpectrum(const MinimumPhaseAnalysis* minimum_phase);
void GetSpectrumWithFractionalTimeShift(int fft_size,
	float coefficient, const InverseRealFFT* inverse_real_fft);
void RemoveDCComponent(const float* periodic_response, int fft_size,
	const float* dc_remover, float* new_periodic_response);
void fft_execute(fft_plan p);
void ForwardFFT(fft_plan p);
void BackwardFFT(fft_plan p);
void cdft(int n, int isgn, float* a, int* ip, float* w);
void rdft(int n, int isgn, float* a, int* ip, float* w);
/*############################################################################################*/
void GetAperiodicResponse(int noise_size, int fft_size,
	const float* spectrum, const float* aperiodic_ratio, float current_vuv,
	const ForwardRealFFT* forward_real_fft,
	const InverseRealFFT* inverse_real_fft,
	const MinimumPhaseAnalysis* minimum_phase, float* aperiodic_response);
void GetNoiseSpectrum(int noise_size, int fft_size,
	const ForwardRealFFT* forward_real_fft);
void fftshift(const float* x, int x_length, float* y);
float randn(void);
/*############################################################################################*/


void fft_destroy_plan(fft_plan p);
void DestroyMinimumPhaseAnalysis(MinimumPhaseAnalysis* minimum_phase);
void DestroyInverseRealFFT(InverseRealFFT* inverse_real_fft);
void DestroyForwardRealFFT(ForwardRealFFT* forward_real_fft);


/*############################################################################################*/
float MyMaxfloat(float x, float y);
float MyMinfloat(float x, float y);
int MyMinInt(int x, int y);
int MyMaxInt(int x, int y);