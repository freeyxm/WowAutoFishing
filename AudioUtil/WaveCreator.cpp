#include "stdafx.h"
#include "WaveCreator.h"

WaveCreator::WaveCreator()
{
}

WaveCreator::~WaveCreator()
{
}

bool WaveCreator::LoadWave(const char *path, WAVEFORMATEX *pwfx, char **ppData, uint32_t *pDataLen)
{
	WaveCreator wave;
	if (wave.Load(path))
	{
		pwfx->wFormatTag = wave.m_info.fmt.audioFormat;
		pwfx->nChannels = wave.m_info.fmt.numChannels;
		pwfx->nSamplesPerSec = wave.m_info.fmt.sampleRate;
		pwfx->nAvgBytesPerSec = wave.m_info.fmt.byteRate;
		pwfx->nBlockAlign = wave.m_info.fmt.blockAlign;
		pwfx->wBitsPerSample = wave.m_info.fmt.bitsPerSample;
		pwfx->cbSize = 0;
		wave.TakeData(ppData, pDataLen);
		return true;
	}
	return false;
}

bool WaveCreator::SaveWave(const char *path, WAVEFORMATEX *pwfx, char *pData, uint32_t dataLen)
{
	WaveCreator wave;
	wave.m_info.fmt.audioFormat = pwfx->wFormatTag;
	wave.m_info.fmt.numChannels = pwfx->nChannels;
	wave.m_info.fmt.sampleRate = pwfx->nSamplesPerSec;
	wave.m_info.fmt.byteRate = pwfx->nAvgBytesPerSec;
	wave.m_info.fmt.blockAlign = pwfx->nBlockAlign;
	wave.m_info.fmt.bitsPerSample = pwfx->wBitsPerSample;
	wave.m_info.fmt.extParamSize = 0;

	wave.GiveData(pData, dataLen);
	bool ret = wave.Save(path);
	wave.TakeData(&pData, &dataLen);
	return ret;
}
