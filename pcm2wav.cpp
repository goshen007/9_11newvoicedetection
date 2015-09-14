#include <string>
#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include "pcm2wav.h"

CPcm2Wav::CPcm2Wav(const string& inFile, const string& outFile):
	m_inFileName(inFile), m_outFileName(outFile), m_isFile2File(true)
{
}

CPcm2Wav::CPcm2Wav( void* buffer, int bufferSize, const string& outFileName):
	m_outFileName(outFileName), m_pbuffer((void*)buffer),m_bufferSize(bufferSize),m_isFile2File(false)
{

}

CPcm2Wav::~CPcm2Wav()
{
	//如果是 ‘文件’->'wav文件'，需要将输入文件的内容拷贝至m_pbuffer中
	//所以在这里进行释放
	//否则是 'buffer' -> ‘wav文件’，不需要释放
	if (m_isFile2File)
	{
		free(m_pbuffer);
	}
}

int CPcm2Wav::Pcm2Wav(const Pcm2WavParameter& para)
{
	assert( para.channels == 1 || para.channels == 2 );
	assert( para.formatTag > 0 );
	assert( para.sampleBits==8 || para.sampleBits==16 || para.sampleBits==24 | para.sampleBits==32);
	assert( para.sampleRate > 0 );
	m_parameter = para;
	if (m_isFile2File)
	{
		Pcm2Wav_File2File();
	}
	else
	{
		Pcm2Wav_Buffer2File();
	}
	return 0;
}
void CPcm2Wav::Pcm2Wav_File2File()
{
	// ‘文件’-》‘内存’
	ReadFileToMemory();
	Pcm2Wav_Buffer2File();
}
void CPcm2Wav::Pcm2Wav_Buffer2File()
{
	//初始化输出的参数
	m_outFileIndex = 0;
	OpenOutputFile();
	WriteToOutputFile();
	CloseOutputFile();
}
void CPcm2Wav::ReadFileToMemory()
{
	m_pInFile = fopen(m_inFileName.c_str(), "rb");
	if (m_pInFile == NULL)
	{
		fprintf(stderr, "Eorror:Cannnot open the input file.\n");
		exit(-1);
	}
	//读入文件内容
	fseek(m_pInFile, 0, SEEK_END);
	m_bufferSize = ftell(m_pInFile);
	fseek(m_pInFile, 0, SEEK_SET);
	m_pbuffer = malloc(m_bufferSize);
	fread(m_pbuffer, 1, m_bufferSize, m_pInFile);
	fclose(m_pInFile);
}
void CPcm2Wav::OpenOutputFile()
{
	m_pOutFile = fopen(m_outFileName.c_str(), "wb");
	if (m_pOutFile == NULL)
	{
		fprintf(stderr, "Eorror:Cannnot open the output file.\n");
		exit(-1);
	}
}
void CPcm2Wav::WriteToOutputFile()
{
	WriteRIFFChunk();
	WriteFMTChunk();
	WriteDataChunk();
}
void CPcm2Wav::WriteSomeDataToOutFile( const void* str, int size )
{
	fseek( m_pOutFile, m_outFileIndex, SEEK_SET );
	fwrite( str, 1, size, m_pOutFile);
	m_outFileIndex += size;
}
unsigned long CPcm2Wav::GetDataFileSize()
{
	return m_bufferSize;
}


void CPcm2Wav::WriteRIFFChunk()
{
	WriteRIFFID();
	WriteRIFFChunkSize();
	WriteWAVEID();
}
void CPcm2Wav::WriteRIFFID()
{
	const char *riff = "RIFF";
	WriteSomeDataToOutFile( riff, DWORD_SIZE );

}
void CPcm2Wav::WriteRIFFChunkSize()
{
	//写CHUNKE_SIZE = 音频实际大小 + 文件头（44个字节） - 8
	 DWORD DWORD_num = GetDataFileSize() + 0x2c - 8;
	 WriteSomeDataToOutFile( &DWORD_num, DWORD_SIZE );
}
void CPcm2Wav::WriteWAVEID()
{
	//写WAVE
	const char* wave = "WAVE";
	WriteSomeDataToOutFile( wave,DWORD_SIZE  );
}

void CPcm2Wav::WriteFMTChunk()
{
	WriteFMTID();
	WriteFMTChunkSize();
	WriteFormatTag();
	WriteChannels();
	WriteSampleRate();
	WriteDataRate();
	WriteBlockAlign();
	WriteBitsPerSample();
}
void CPcm2Wav::WriteFMTID()
{
	//写FMT
	const char* fmt = "fmt ";
	WriteSomeDataToOutFile( fmt, DWORD_SIZE);
}
void CPcm2Wav::WriteFMTChunkSize()
{
	//写size(16 or 18)通常16
	DWORD DWORD_num = 16;
	WriteSomeDataToOutFile( &DWORD_num,DWORD_SIZE );
}
void CPcm2Wav::WriteFormatTag()
{
	//写AudioFormat
	WORD WORD_num = m_parameter.formatTag;	//pcm数据
	WriteSomeDataToOutFile( &WORD_num,WORD_SIZE );
}
void CPcm2Wav::WriteChannels()
{
	//写声道数
	WORD WORD_num = m_parameter.channels;
	WriteSomeDataToOutFile( &WORD_num, WORD_SIZE);
}
void CPcm2Wav::WriteSampleRate()
{
	//写采样率
	DWORD DWORD_num = m_parameter.sampleRate;
	WriteSomeDataToOutFile( &DWORD_num, DWORD_SIZE );
}
void CPcm2Wav::WriteDataRate()
{
	DWORD DWORD_num = m_parameter.sampleRate * m_parameter.channels * m_parameter.sampleBits / 8;
	WriteSomeDataToOutFile( &DWORD_num, DWORD_SIZE );
}
void CPcm2Wav::WriteBlockAlign()
{
	WORD WORD_num = m_parameter.channels * m_parameter.sampleBits / 8;
	WriteSomeDataToOutFile( &WORD_num, WORD_SIZE );
}
void CPcm2Wav::WriteBitsPerSample()
{
	WORD WORD_num = m_parameter.sampleBits;
	WriteSomeDataToOutFile( &WORD_num, WORD_SIZE );
}

void CPcm2Wav::WriteDataChunk()
{
	WriteDataID();
	WriteDataSize();
	WriteAudioData();
}
void CPcm2Wav::WriteDataID()
{
	const char* data = "data";
	WriteSomeDataToOutFile( data, DWORD_SIZE );
}
void CPcm2Wav::WriteDataSize()
{
	unsigned long dataSize = GetDataFileSize();
	WriteSomeDataToOutFile( &dataSize, DWORD_SIZE );
}
void CPcm2Wav::WriteAudioData()
{
	unsigned long dataSize = GetDataFileSize();

	WriteSomeDataToOutFile( m_pbuffer, dataSize );
}

