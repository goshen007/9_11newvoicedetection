#pragma once
#include "RtAudio.h"
#include "pcmaudio.h"
#include "scopeguard.h"


template <typename T>
int outputCallback(void *outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
	double, RtAudioStreamStatus, void* data)
{
	PcmData<T> *oData = (PcmData<T>*) data;
	unsigned int frames = nBufferFrames;
	if (oData->frameCounter + nBufferFrames > oData->totalFrames) {
		frames = oData->totalFrames - oData->frameCounter;
		oData->bufferBytes = frames * oData->channels * sizeof(T);
	}
	//计算目前播放的时间轴（偏移量）
	unsigned long offset = oData->frameCounter * oData->channels;
	//将数据拷贝至输出流，进行播放
	memcpy(outputBuffer, oData->buffer + offset, oData->bufferBytes);
	oData->frameCounter += frames;
	//return 2 表示结束
	if (oData->frameCounter >= oData->totalFrames)
		return 2;
	return 0;
}

template <typename T>
class CPcmPlayer
{
public:
	explicit CPcmPlayer( T* buffer, unsigned long totalBytes, const PcmPlayParameters & params);
	~CPcmPlayer() {}

	void Start();
private:
	void InitOutputParams(RtAudio::StreamParameters& params, RtAudio& adc);
	void PrintPlayMessage()const;
	void WaitForFinish(const RtAudio& adc )const;
private:
	PcmData<T> m_data;
	PcmPlayParameters m_params;

	CPcmPlayer(){}
	CPcmPlayer(const CPcmPlayer& p) {}
	CPcmPlayer& operator=(const CPcmPlayer& p){}
};


template<typename T>
inline CPcmPlayer<T>::CPcmPlayer(T * buffer, unsigned long totalBytes, const PcmPlayParameters & params)
{
	m_params = params;
	m_data.buffer = buffer;
	m_data.channels = params.nChannels;
	m_data.bufferBytes = params.framesPerBuffer * params.nChannels * sizeof(T);
	m_data.totalFrames = totalBytes / params.nChannels / sizeof(T);
	m_data.frameCounter = 0;
}

template<typename T>
inline void CPcmPlayer<T>::Start()
{

	RtAudio adc;
	assert(adc.getDeviceCount() >= 1);
	//初始化流参数
	RtAudio::StreamParameters oParams;
	InitOutputParams(oParams, adc);
	//重置时间轴
	m_data.frameCounter = 0;
	//打开音频流，同时做安全的捕获异常处理
	TRY_CATCH_RTAUDIOERROR(adc.openStream(&oParams, NULL, m_params.audioFormat, 
		m_params.sampleRate,&m_params.framesPerBuffer, &outputCallback<T>, (void*)&m_data));
	//开始播放，同时做安全的捕获异常处理
	TRY_CATCH_RTAUDIOERROR(adc.startStream());
	//打印播放信息
	PrintPlayMessage();
	//等待播放结束
	WaitForFinish(adc);
	if (adc.isStreamOpen()) adc.closeStream();
}

template<typename T>
inline void CPcmPlayer<T>::InitOutputParams(RtAudio::StreamParameters & params, RtAudio & adc)
{
	//简单起见，使用默认输出设备
	params.deviceId = adc.getDefaultOutputDevice();
	params.nChannels = m_data.channels;
	params.firstChannel = 0;
}

template<typename T>
inline void CPcmPlayer<T>::PrintPlayMessage()const
{
	cout << "\nPlay for " << m_data.totalFrames << " Frames.(buffer frames = " << m_params.framesPerBuffer << endl;
	cout << "\nChannnels:" << m_params.nChannels << "\nSample Rate:" << m_params.sampleRate << endl;
}

template<typename T>
inline void CPcmPlayer<T>::WaitForFinish(const RtAudio & adc) const
{
	while (adc.isStreamRunning())
	{
		SLEEP(100);
	}
}
