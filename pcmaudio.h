#pragma once
#include "RtAudio.h"
#include <assert.h>
#include <iostream>
using std::cout;
using std::endl;
/*!
*  音频格式
*  目前只支持 float32、int16、int32
*/
typedef unsigned long AudioFormat;
//static const AudioFormat AUDIO_SIN8	= RTAUDIO_SINT8;	//对应类型请使用char
static const AudioFormat AUDIO_SIN16 = RTAUDIO_SINT16;		//对应类型请使用short
															//static const AudioFormat AUDIO_SIN24 = RTAUDIO_SINT24;	//对应类型请使用...我也不知道
static const AudioFormat AUDIO_SIN32 = RTAUDIO_SINT32;		//对应类型请使用int
static const AudioFormat AUDIO_FLOAT32 = RTAUDIO_FLOAT32;	//对应类型请使用float
															//static const AudioFormat AUDIO_FLOAT64 = RTAUDIO_FLOAT64;	//对应类型请使用double

															//录音时所需要的参数
struct RecordParameters
{
	unsigned int nSeconds;
	unsigned int nChannels;
	unsigned int sampleRate;
	unsigned int framesPerBuffer;
	AudioFormat audioFormat;
};

//播放pcm文件时需要的参数
struct PcmPlayParameters
{
	unsigned int nChannels;
	unsigned int sampleRate;
	unsigned int framesPerBuffer;
	AudioFormat audioFormat;
};

//存放pcm数据的结构体
template <typename T>
struct PcmData
{
	T* buffer;						//真正的数据地址
	unsigned long bufferBytes;		//每一帧需要的bytes
	unsigned long totalFrames;		//一共有多少帧
	unsigned long frameCounter;		//帧 计数器
	unsigned long channels;			//声道数
};

#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define TRY_CATCH_RTAUDIOERROR(x) try {x;}catch(RtAudioError& e){cout<<'\n'<<e.getMessage()<<endl;}