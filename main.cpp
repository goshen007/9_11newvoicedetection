#include <iostream >
#include <vector>
#include "detection.h"
#include "voicedetection.h"
#include "CAudioTimeSandPitchS.h"
#include "CWavread.h"
#include "pcm2wav.h"
#include "CPitchShift.h"
#include "record.h"
#include "pcmaudio.h"
#include "denoise.h"
#include <cmath>
#include <string>
#include <fstream>
#include <memory>
#include <map>
#include <string>
#include <algorithm>
using namespace std;


//将检测到的人声片段写入文件
void WriteSpeechSegmentToFile( const float* buffer, const vector<SpeechSegment>& sgm, int hop );
//将最终旋律化的结果写入文件
void WriteFinalDataToWavFile( const vector<float>& data, const string& fileName );

//short -> float
float* Bit16toBit32( short* bit16, unsigned long bit16SampleCount );

//输出人声片段
void PrintSgm( const vector<SpeechSegment>& sgms );



//获取需要音频数据，用于人声片段的检测
float* GetDataForDetection( int& sampleCount, int& sampleRate );
//读 wav 文件获取数据，只能用于简单情况的wav文件（即真正音频数据从0x2c位置开始）
float* GetDataForDetectionFromWavFile( const string& fileName, int& sampleCount, int& sampleRate);
//通过录音获取数据
float* GetDataForDetectionFromRecord(int& sampleCount, int& sampleRate);
//进行旋律化
void DoMelody( float* buffer, const vector<SpeechSegment>& segments, int hop );


int main()
{
	//获取需要人声检查的声音数据
	//需要sample个数、采样率
	int sampleCount = 0;
	int sampleRate = 0;

	float* buffer = GetDataForDetection( sampleCount, sampleRate );
	cout << sampleRate << " "<< sampleCount << endl;
	//进行人声检测
	CVoiceDetection detection;
	auto speechSegments( detection.Detection( buffer, sampleCount, sampleRate ) );
	PrintSgm( speechSegments );

	//进行旋律化
	DoMelody(buffer, speechSegments, detection.m_hop);

	delete[] buffer;
}

float* GetDataForDetection(int& sampleCount, int& sampleRate)
{
	//return GetDataForDetectionFromRecord(sampleCount, sampleRate);
	return GetDataForDetectionFromWavFile("D:/iii.wav", sampleCount, sampleRate);


}

//************************************
// Method:    GetDataForDetectionFromWavFile
// Parameter@ fileName: wav文件名 
// Returns:   float*
// Comment:   暂定只能读 16bit 单声道wav文件
// Creator:	  HW
// Modifier:  
//************************************
float* GetDataForDetectionFromWavFile(const string& fileName,int& sampleCount, int& sampleRate)
{
	FILE* fd = fopen(fileName.c_str(), "rb");
	fseek(fd, 0, SEEK_END);
	unsigned long fileSize = ftell(fd) - 0x2c;
	fseek(fd, 0x2c, SEEK_SET);
	short* data = (short*)malloc(fileSize);
	fread(data, 1, fileSize, fd);
	fclose(fd);
	sampleRate = 44100;
	sampleCount = fileSize / sizeof(short);
	//CDeNoise::DeNoise(data, sampleCount, sampleRate);
	float* buffer = Bit16toBit32(data, sampleCount);
	return buffer;
}


//************************************
// Method:    GetDataForDetectionFromRecord
// Returns:   float*
// Comment:   录音暂时定为 16bit 单声道 44.1hz，因为使用其他参数的录音，后期处理有一些问题
// Creator:	  HW
// Modifier:  
//************************************
float* GetDataForDetectionFromRecord(int& sampleCount, int& sampleRate)
{
	//设置录音的参数 暂时定为 16bit 单声道 44.1hz
	RecordParameters recordParams;
	recordParams.audioFormat = AUDIO_SIN16;
	recordParams.framesPerBuffer = 512;
	recordParams.nChannels = 1;
	recordParams.nSeconds = 5;
	recordParams.sampleRate = 44100;

	//开始录音
	CRecord<short> recorder(recordParams);
	recorder.Start();

	//
	sampleCount = recorder.GetTotalSamples();
	sampleRate = recordParams.sampleRate;

	//short数据 --> float数据,后期处理需要用float数据进行
	float* buffer = Bit16toBit32(recorder.GetDataPointer(), recorder.GetTotalSamples());
	return buffer;
}

void DoMelody(float* buffer, const vector<SpeechSegment>& segments, int hop)
{
	CAudioTimeSandPitchS pitch;
	vector<float> afterMelodySamples;
	float dst_freq = 260;
	float dst_time = 0.0;
	//int melody[] = { C3,C3,G3,G3,A3,A3,G3,F3,F3,E3,E3,D3,D3,C3 };
	//float thythm[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2  };
	float velocity[] = { 4, 1, 1, 1, 4, 1, 1, 1, 3, 2, 2, 4, 3, 3, 1,  1,  1, 1, 1,  1,  1,  1,  1, 1,1, 1, 1, 1, 1 };
	int melody[] = { C3,D3,E3,C3,C3,D3,E3,C3,E3,F3,G3,E3,F3,G3,G3,A3, G3, F3, E3,C3,G3, A3, G3, F3, E3,C3,D3,G2,C3 };
	float thythm[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2,0.5,0.5,0.5,0.5,1, 1, 0.5,0.5,0.5,0.5,1, 1, 1, 1, 2 };
	int melodySize = sizeof(melody) / sizeof(melody[0]);
	for (int i = 0; i < melodySize;)
	{
		for (auto it = segments.begin(); it != segments.end(); ++it)
		{
			if (i >= melodySize)
				break;
			dst_freq = melody[i];
			dst_time = it->segTime / thythm[i]*3;
			int distance = it->start * hop;
			//int length = (it->end - it->start) * sizeof(float) * hop;
			int length= (it->end - it->start) * hop;
			//auto datain = pitch.WavReadBuffer(buffer + distance, length, 1);
			//auto datain = pitch.WavReadFile( "denoise_16.wav" );
			//音强处理
			transform(buffer + distance, buffer + distance + length, buffer + distance, [&](float x){return x*velocity[i];});
			//对语音段进行声音处理
			auto dataResult = pitch.TimeScalingAndPitchShiftingRobot(dst_freq, dst_time, buffer+distance, length,1024);
			//delete[] datain;
			for (int k = 0; k < pitch.GetSize() / sizeof(float); k++)
			{
				afterMelodySamples.push_back(dataResult[k]);
			}
			delete[] dataResult;
			i++;
		}
	}
	WriteFinalDataToWavFile(afterMelodySamples, "D:/final.wav");
	WriteSpeechSegmentToFile(buffer, segments, hop);
}
void WriteSpeechSegmentToFile( const float* buffer, const vector<SpeechSegment>& sgm, int hop )
{

	FILE* file = fopen( "result.raw", "wb" );
	for( auto it = sgm.begin(); it != sgm.end(); ++it )
	{
		int distance = it->start * hop;
		fwrite( buffer + distance, 1,(it->end - it->start)*hop*sizeof(float) , file );
	}
	fclose( file );
	//写到wav文件		
	CPcm2Wav convert("result.raw","segment.wav");
	Pcm2WavParameter params;
	params.channels = 1;
	params.formatTag = 3;
	params.sampleBits = 32;
	params.sampleRate = 44100;
	convert.Pcm2Wav(params);
}
void WriteFinalDataToWavFile(const vector<float>& data, const string& fileName)
{
	float* testout = new float[data.size()];
	memset(testout, 0, data.size());
	copy(data.begin(), data.end(), testout);
	//写到wav文件		
	CPcm2Wav convert(testout, data.size()*sizeof(float), fileName);
	Pcm2WavParameter params;
	params.channels = 1;
	params.formatTag = 3;
	params.sampleBits = 32;
	params.sampleRate = 44100;
	convert.Pcm2Wav(params);
	delete[] testout;
}
void PrintSgm( const vector<SpeechSegment>& sgms )
{
	for( auto it = sgms.begin(); it != sgms.end(); ++it  )
	{
		cout << it->start << " " << it->end << " freq:" << it->frequence << " beat:" << it->segTime << endl;
	}
}
float* Bit16toBit32(short* bit16, unsigned long bit16SampleCount)
{
	unsigned char* buffer = (unsigned char*)bit16;
	unsigned long totalBytes = bit16SampleCount * sizeof(short);
	float *Out = new float[bit16SampleCount];

	for (unsigned long i = 0; i < totalBytes; i += 2)
	{
		//右边为大端         
		unsigned long data_low = buffer[i];
		unsigned long data_high = buffer[i+1];
		float float_data;
		double data_true = data_high * 256 + data_low;
			//printf("%d ",data_true);         
			long data_complement = 0;
		//取大端的最高位（符号位）         
		int my_sign = (int)(data_high / 128);
		//printf("%d ", my_sign);         
		if (my_sign == 1)
		{
			data_complement = data_true - 65536;
		}
		else
		{
			data_complement = data_true;
		}
		float_data = (float)(data_complement / (double)32768);
		Out[i/2] = float_data;
	}
	return Out;
}
