#include "denoise.h"
#include "speex_preprocess.h"
#include <string.h>
#include <stdio.h>
CDeNoise::CDeNoise()
{

}

void CDeNoise::DeNoise(short* buffer, unsigned long sampleCount, unsigned int sampleRate)
{
	SpeexPreprocessState *st=nullptr;
	int i = 0;
	float f = 0.0;

//	int i;
	//st = speex_preprocess_state_init(frame_size, sampling_rate);
	//speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, &denoise);
	//speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noise_suppress);
	//i = 1;
	//speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &i);//增益
	//i = 100;
	//speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_MAX_GAIN, &i);
	//i = 1;
	//speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB, &i);


	//st = speex_preprocess_state_init(kFrameSize, sampleRate);
	//i = 1;
	//speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, &i);
	////i = 0;
	////speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &i);
	////i = sampleRate;
	////speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
	////i = 0;
	////speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB, &i);
	////f = .0;
	////speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
	////f = .0;
	////speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);

	st = speex_preprocess_state_init(kFrameSize, sampleRate);
	i = 1;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, &i);
	i = 1;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &i);//增益
	i = 100;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_MAX_GAIN, &i);
	i = -25;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &i);
	i = 1;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB, &i);



	unsigned long distance = 0;
	while (true)
	{
		if (distance + kFrameSize > sampleCount)
			break;
		speex_preprocess_run( st, buffer+distance);
		distance += kFrameSize;
	}
	speex_preprocess_state_destroy(st);
}