#pragma once

class CDeNoise
{
public:
	CDeNoise();
	~CDeNoise() {}
	static void DeNoise(short* buffer, unsigned long sampleCount, unsigned int sampleRate);
private:
	static const int kFrameSize = 160;
private:
	CDeNoise(const CDeNoise& n) {}
	CDeNoise& operator=(const CDeNoise& n) {}
};