#ifndef VOICEDETECTION_H_INCLUDED
#define VOICEDETECTION_H_INCLUDED
#include "calcthreshold.h"
#include <map>
#include <vector>
using std::map;
using std::vector;
using std::pair;

#define MIN_VOICE_FREQUENCY 80.0
#define MAX_VOICE_FREQUENCY 500
#define MAX(x,y) ( (x)>(y)?(x):(y) )
#define MIN(x,y) ( (x)<(y)?(x):(y) )
#define AMP_MIN_MUL 30

struct SpeechSegment{
	int frequence;		//人声的基频，然而采用了新的方法，未使用这个变量
	unsigned int start;	//人声起始点，第start个窗开始
	unsigned int end;	//人声终止点，第end个窗结束
	float segTime;		//这一段人声的时长
	float velocity;		//这一段人声的音强
	SpeechSegment( int fre, int s, int e, float b,float v ):
	frequence(fre), start(s), end(e), segTime(b), velocity(v)
	{
	}
};

class CVoiceDetection{
private:
    CVoiceDetection( const CVoiceDetection& ){}
    void operator=( const CVoiceDetection& ){}

	//是否是声音
	bool IsVoice( double amp, int zcr );
	//是否有可能是声音
	bool IsMaybeVoice( double amp, int zcr );
	//计算过零率
    void CalcZeroCrossRate();
	//计算能量
    void CalcAmplitude();
	//计算能量的门限
    void CalcAmpThreshold();
	/**
	* @brief : 语音分帧（窗）
	*/
	void EnFrame( const float* dataIn, int sampleSize, int winSize, int hop );
	
	/**
	* @brief : 人声的开始点和终止点检测
	*/
    void StartEndPointDetection();

	/**
	* @brief : 检测人声段基频、长度、开始和终止点（未使用）
	*/
	vector<SpeechSegment> FindSpeechSegment( const float* buffer, int sampleRate );
	/**
	* @brief : 计算短时平均幅度差，用于基频检测（未使用）
	*/
	vector<float> AMDFCalc( const vector<float>& amdfData );
	/**
	* @brief : 计算人声基频（未使用）
	*/
	int VoiceFrequenceCalc( const vector<float>& amdfResult, int sampleRate );

	/**
	* @brief : 获取人声段长度、开始和终止点。
	*/
	vector<SpeechSegment> GetSpeechSegmentInfo( int sampleRate );

	/**
	* @brief : 计算一段人声的强度。
	*/
	float CalcVelocity(int start, int end);


public:
    CVoiceDetection();
	~CVoiceDetection();
    vector<SpeechSegment> Detection( const float* buffer, int sampleCount, int sampleRate );

public:
    vector<int> m_zeroCrossRate;			//存放每个语音窗的过零率
    vector<double> m_amplitude;				//存放每个语音窗的能量
	vector<SpeechSegment> m_speechSegment;	//存放人声段的信息
	map<int, int> m_startEndMap;
    int m_winSize;
	int m_hop;
	float m_ampThreshold;
	int m_zcrThreshold;

	int m_minSilence;
	int m_minVoice;
	int m_voiceCount;
	int m_silenceCount;

	int m_voiseStatus;

	int m_frameCount;
	float** m_frameData;
};


#endif // VOICEDETECTION_H_INCLUDED
