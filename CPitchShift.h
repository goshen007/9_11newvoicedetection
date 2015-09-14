#include <math.h>
#include "CFFT.h" 
#include <memory>

class CpitchShiftwithHop{
private:
	CpitchShiftwithHop(const CpitchShiftwithHop& C){}
	CpitchShiftwithHop& operator = (const CpitchShiftwithHop& C){}
	int m_winSize;
	int m_hop;
	float* m_window;
	CFFT m_doFFt;
	/**
	* @brief : 设置窗的类型，用于变调，hop直接决生成新频率
	*/
	int SetWindow(int winSize,int hop);	
public:
	CpitchShiftwithHop(){};
	~CpitchShiftwithHop(){};

	/**
	* @brief : 通过窗移，生成新频率
	*/
	float* pitchShift(int dst_freq,float* dataIn,unsigned long dataInSize,int winSize);
};


int CpitchShiftwithHop::SetWindow(int winSize,int hop)
{
	m_hop=hop;
	m_winSize=winSize;
	m_window=new float[winSize];
	//set window as hanning
	for (int i=0;i<m_winSize;i++){
		m_window[i]=0.5*(1+cos(PI*(i-m_winSize/2.0)/(m_winSize/2.0-1)));
	}
	return 0;

}

/*******************************
** FunctionName：pitchShift
**@Param dst_freq: 目标频率
**@Param dataIn：语音数据
**@Param dataInSize：语音数据大小
**@Param winSize： 窗大小
** Comment ：通过窗移，生成新频率
** return  ：变频之后的数据
** Creator ：CGZ
** Date：
*******************************/
float* CpitchShiftwithHop::pitchShift(int dst_freq,float* dataIn,unsigned long dataInSize,int winSize)
{
	SetWindow(winSize,44100/dst_freq);
	int pin(0),pout(0),pend(dataInSize-winSize);
	float* dataOut=new float[dataInSize];
	memset(dataOut,0,dataInSize*sizeof(float));
	complex* dataFFT=new complex[winSize];
	memset(dataFFT,0,winSize*sizeof(float));
	float* dataIFFT=new float[winSize];
	memset(dataIFFT,0,winSize*sizeof(float));
	while (pin<pend)
	{
		for (int i=0;i<winSize;i++)
		{
			dataFFT[i].real=dataIn[i+pin]*m_window[i];
			dataFFT[i].imag=0;
		}
		m_doFFt.fft(winSize,dataFFT);
		m_doFFt.c_abs(dataFFT,dataIFFT,winSize);
		m_doFFt.ifft(winSize,dataIFFT);
		//m_doFFt.ifft(winSize, dataFFT);
		float* dataTemp=new float[winSize/2];
		memset(dataTemp,0,winSize/2*sizeof(float));

		//参照MATLAB的fftshift
		//  -------------------------             --------------------------
		//  |     a     |     b     |     ---->   |    b       |     a     |              
		//  -------------------------             --------------------------
		memcpy(dataTemp,dataIFFT,winSize/2*sizeof(float));
		memcpy(dataIFFT,dataIFFT+winSize/2,winSize/2*sizeof(float));
		memcpy(dataIFFT+winSize/2,dataTemp,winSize/2*sizeof(float));
		delete[] dataTemp;

		//综合的时候进行加窗
		for (int i=0;i<winSize;i++)
		{
			if (pout+i<dataInSize)
			{
				dataIFFT[i]=dataIFFT[i]*m_window[i];
				dataOut[pout+i]=dataOut[pout+i]+dataIFFT[i];
			}
		}
		pin+=m_hop;
		pout+=m_hop;
	}
	delete[] dataFFT;
	delete[] dataIFFT;
	return dataOut;

}
