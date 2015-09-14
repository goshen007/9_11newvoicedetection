#ifndef PCM2WAV_H
#define PCM2WAV_H

#include <string>
using std::string;

//wave格式有比较复杂的几种情况
//这里，我们使用最简单的情况，能够支持播放器播放
//具体复杂情况可以参照 http://blog.csdn.net/yueyihua/article/details/5494905
//----------------以下定义一些地址

//RIFF,资源交换标志
#define RIFFID			0x00
//从下一个地址开始到文件尾的总字节数
#define CHUNKE_SIZE		0x04
//WAVE, 代表WAV文件格式
#define WAVE			0x08
//FMT,波形格式标志
#define FMT				0x0c
//16或者18，18表示有额外信息
#define FMT_CKSIZE		0x10
//为1时表示线性PCM编码，大于1时表示有压缩的编码
//WAVE_FORMAT_IEEE_FLOAT为32bit float准备
#define AUDIO_FORMAT	0x14
//声道数
#define CHANNELS		0x16
//采样率
#define SAMPLE_RATE		0x18
//BYTE率=采样频率*音频通道数*每次采样得到的样本位数/8
#define BYTE_RATE		0x1c
//块对齐=通道数*每次采样得到的样本位数/8
#define BLOCKALIGN		0x20
//样本数据位数
#define BITS_PER_SAMPLE	0x22
//data, 一个标示而已
#define DATAID			0x24
//Wav文件实际音频数据所占大小
#define DATA_SIZE		0x28
//实际数据开始处
#define DATA_BEGIN		0x2c

#define DWORD_SIZE		4
#define WORD_SIZE		2

struct Pcm2WavParameter{
	unsigned int channels;		//声道数
	unsigned int sampleRate;		//采样率
	unsigned int sampleBits;		//采样的位数
	unsigned int formatTag;		//格式标志
									//3 为 32位float
									//1 位 16位pcm
};


class CPcm2Wav{
private:
	CPcm2Wav() {}
	CPcm2Wav( const CPcm2Wav& cp ){}
	CPcm2Wav& operator=( const CPcm2Wav& cp ){}
	
	void Pcm2Wav_File2File();
	void Pcm2Wav_Buffer2File();
	void ReadFileToMemory();
	void OpenOutputFile();
	void CloseOutputFile() { fclose(m_pOutFile); }
	void WriteToOutputFile();
	unsigned long GetDataFileSize();
	void WriteSomeDataToOutFile( const void* str, int size );

	void WriteRIFFChunk();
	void WriteRIFFID();
	void WriteRIFFChunkSize();
	void WriteWAVEID();

	void WriteFMTChunk();
	void WriteFMTID();
	void WriteFMTChunkSize();
	void WriteFormatTag();
	void WriteChannels();
	void WriteSampleRate();
	void WriteDataRate();
	void WriteBlockAlign();
	void WriteBitsPerSample();
	
	void WriteDataChunk();
	void WriteDataID();
	void WriteDataSize();
	void WriteAudioData();
private:
	Pcm2WavParameter m_parameter;
	bool m_isFile2File;
	FILE* m_pOutFile;
	FILE* m_pInFile;
	void* m_pbuffer;
	unsigned long m_bufferSize;
	int m_outFileIndex;
	string m_inFileName;
	string m_outFileName;
public:
	CPcm2Wav( const string& inFile, const string& outFile );
	CPcm2Wav( void* buffer, int bufferSize, const string& outFileName );
	~CPcm2Wav();
	/**
	* @brief : 根据infomation进行封装 
	*/
	int Pcm2Wav( const Pcm2WavParameter& info );
};

#endif