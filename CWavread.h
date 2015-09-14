#pragma  once



#include <iostream> 
#include <fstream> 
#include <string.h> 
#include<math.h> 
#include<cmath> 
#include<stdlib.h> 
#include <bitset> 
#include <iomanip> 


struct wav_struct 
{ 
	const char* filename;
	unsigned long file_size;        //文件大小     
	unsigned short channel;         //通道数     
	unsigned long frequency;        //采样频率     
	unsigned long Bps;              //Byte率     
	unsigned short sample_num_bit;  //一个样本的位数     
	unsigned long data_size;        //数据大小     
	unsigned char *data;            //音频数据 ,这里要定义什么就看样本位数了，我这里只是单纯的复制数据 
};


class CWavread
{
private:
	CWavread(const CWavread& cp){}
	CWavread& operator = (const CWavread& cp){}

	int hex_char_value(char c);
	int hex_to_decimal(char *szHex);
public:
	CWavread();
	~CWavread();
	wav_struct ReadHead(const char* filename);
	//读取单声道音频数据
	float* ReadMonoData(wav_struct wav);
	//读取双声道音频数据
	float* ReadStereoData(wav_struct wav);
};