//#include "wavread.h"
//#include "fft_complex.h"
#include "CFFT.h"
#include "CWavread.h"
#include <string.h>
#include <fstream>
using namespace std;

#define STH_ISTFT 2


class CAudioTimeSandPitchS
{
private:
	CAudioTimeSandPitchS(const CAudioTimeSandPitchS& cp){}
	CAudioTimeSandPitchS& operator = (const CAudioTimeSandPitchS& cp){}
	complex** STFT(float* dataIn);
	complex** PVsample(complex** dataIn);
	float** PVsampleRobot(complex** dataIn);
	float* ISTFT(complex** dataIn);
	float* ISTFTRobot(float** dataIn);

	unsigned long	m_PCMSize;
	int				m_hop;
	unsigned long	m_STFTOutRow;
	unsigned long   m_STFTOutCol;
	int				m_winSize;
	float			m_scale;
	unsigned long   m_timeScaleSize;
	CFFT			m_FFT;
	CWavread		m_wavread;
	float			m_sampleRateScale;
	unsigned long	m_resampleSize;		
	int				m_winSizeShift;
	int				m_hopShift;
	float*			m_windowShift;
	CFFT			m_doFFt;

public:
	CAudioTimeSandPitchS();
	~CAudioTimeSandPitchS();
	float*	WavReadFile(const char* filename);
	float*  WavReadBuffer(float* buffer,unsigned long bufferSize,int channel);
	float*	TimeScaling(float* dataIn,int winSize,int hop,float scale);
	float*	TimeScalingRobot(float* DataIn, int winSize, int hop, float scale);
	int		WavWrite(float* dataIn);	
	float*	wavresample(float * DataIn,int p,int q);
	unsigned long GetSize(){return m_timeScaleSize*sizeof(float);}
	unsigned long GetResampleSize(){ return m_resampleSize * sizeof(float); }
	float* PitchShiftingFile(float* dataIn,int winSize,int hop,int shift);
	float  GetSampleRateScale(){return m_sampleRateScale;}
	float* PitchShifting(float* dataIn,int winSize,int hop,int shift);
	float* PitchShifting(int dst_freq, float* dataIn, unsigned long dataInSize, int winSize);
	float* TimeScalingAndPitchShifting(int shift, float scale,float* dataIn,int winSize=1024,int hop=256);
	float* TimeScalingAndPitchShifting(float frequence, float scale,float* dataIn, int winSize = 1024, int hop = 256);
	float* TimeScalingAndPitchShifting(int dst_freq, float dst_time, float* dataIn, unsigned long dataInSize, int winSize, int hopScale);
	float* TimeScalingAndPitchShiftingRobot(int dst_freq, float dst_time, float* dataIn, int length, int winSize);


	float* resample(float* dataIn, double scale);
	float* SetWindow(int winSize, int hop);
};