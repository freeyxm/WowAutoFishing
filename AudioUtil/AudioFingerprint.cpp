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
	int midValue = ((1L << (pwfx->wBitsPerSample - 1)) - 1) >> 1;
	bool isFloat = AudioCapture::IsFloatFormat(pwfx);
	float min, max, value;
	AudioFrameData *pFrames;

	int index = 0;
	auto it = source->cbegin();
	while (it != source->cend())
	{
		min = FLT_MAX;
		max = FLT_MIN;
		pFrames = *it;

		size_t count = pFrames->nDataLen / nBytesPerSample;
		for (size_t i = 0; i < count; i += pwfx->nChannels)
		{
			switch (pwfx->wBitsPerSample)
			{
			case 8:
				value = (float)*(pFrames->pData + i) / midValue;
				break;
			case 16:
				value = (float)*((INT16*)pFrames->pData + i) / midValue;
				break;
			case 32:
				if (isFloat)
					value = (float)*((float*)pFrames->pData + i);
				else
					value = (float)*((int*)pFrames->pData + i) / midValue;
				break;
			default:
				value = 0;
				break;
			}
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

