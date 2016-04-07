#include "stdafx.h"
#include "AudioFingerprint.h"
#include "Win32Util/AudioCapture.h"
#include "CommUtil/VectorUtil.h"

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
	int midValue = AudioCapture::GetMidValue(pwfx);
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
			value = AudioCapture::ParseValue(pwfx, pFrames->pData, i, midValue);
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

