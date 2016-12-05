#include "stdafx.h"
#include "AudioFingerprint.h"
#include "Win32Util/AudioUtil.h"
#include "CommUtil/VectorUtil.hpp"

using comm_util::VectorUtil;

AudioFingerprint::AudioFingerprint()
{
}


AudioFingerprint::~AudioFingerprint()
{
}

vector<float> AudioFingerprint::getFingerprint(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx)
{
	vector<float> finger(source->GetSize());

	int nBytesPerSample = pwfx->wBitsPerSample / 8;
	int midValue = AudioUtil::GetMidValue(pwfx);
	float min, max, value;
	AudioFrameData *pFrames;

	int index = 0;
	auto it = source->cbegin();
	while (it != source->cend())
	{
		min = FLT_MAX;
		max = FLT_MIN;
		pFrames = *it;

		UINT count = pFrames->nDataLen / nBytesPerSample;
		for (UINT i = 0; i < count; i += pwfx->nChannels)
		{
			value = AudioUtil::ParseValue(pwfx, pFrames->pData, i, midValue);
			if (min > value)
				min = value;
			if (max < value)
				max = value;
		}

		finger[index++] = max - min;
		++it;
	}

	return finger;
}

vector<float> AudioFingerprint::getFingerprint_diff(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx)
{
	vector<float> finger = getFingerprint(source, pwfx);
	processDiff(finger);
	return finger;
}

vector<float> AudioFingerprint::getFingerprint_cutAvg(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx)
{
	vector<float> finger = getFingerprint(source, pwfx);
	processCutAvg(finger);
	return finger;
}

vector<float> AudioFingerprint::getFingerprint_ratioAvg(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx)
{
	vector<float> finger = getFingerprint(source, pwfx);
	processRatioAvg(finger);
	return finger;
}

void AudioFingerprint::processDiff(vector<float> &data)
{
	size_t count = data.size();
	for (size_t i = 1; i < count; ++i)
	{
		data[i - 1] -= data[i];
	}
	if (count > 1)
	{
		data[count - 1] = 0;
	}
}

void AudioFingerprint::processCutAvg(vector<float> &data)
{
	float avg = VectorUtil::getAvg(&data[0], data.size());
	for (size_t i = 0, count = data.size(); i < count; ++i)
	{
		data[i] -= avg;
	}
}

void AudioFingerprint::processRatioAvg(vector<float> &data)
{
	float avg = VectorUtil::getAvg(&data[0], data.size());
	for (size_t i = 0, count = data.size(); i < count; ++i)
	{
		data[i] /= avg;
	}
}
