#include "stdafx.h"
#include "WaveUtil.h"

WaveUtil::WaveUtil()
{
}

WaveUtil::~WaveUtil()
{
}

void WaveUtil::ConvertFormat(const WaveFile::FormatChunk *pfmt, WAVEFORMATEX *pwfx)
{
	pwfx->wFormatTag = pfmt->audioFormat;
	pwfx->nChannels = pfmt->numChannels;
	pwfx->nSamplesPerSec = pfmt->sampleRate;
	pwfx->nAvgBytesPerSec = pfmt->byteRate;
	pwfx->nBlockAlign = pfmt->blockAlign;
	pwfx->wBitsPerSample = pfmt->bitsPerSample;
	pwfx->cbSize = 0;
}

void WaveUtil::ConvertFormat(const WAVEFORMATEX *pwfx, WaveFile::FormatChunk *pfmt)
{
	pfmt->audioFormat = pwfx->wFormatTag;
	pfmt->numChannels = pwfx->nChannels;
	pfmt->sampleRate = pwfx->nSamplesPerSec;
	pfmt->byteRate = pwfx->nAvgBytesPerSec;
	pfmt->blockAlign = pwfx->nBlockAlign;
	pfmt->bitsPerSample = pwfx->wBitsPerSample;
	pfmt->extParamSize = 0;
}

bool WaveUtil::LoadWave(const char *path, WAVEFORMATEX *pwfx, char **ppData, uint32_t *pDataLen)
{
	WaveUtil wave;
	if (wave.Load(path))
	{
		ConvertFormat(&wave.m_info.fmt, pwfx);
		wave.TakeData(ppData, pDataLen);
		return true;
	}
	return false;
}

bool WaveUtil::SaveWave(const char *path, WAVEFORMATEX *pwfx, char *pData, uint32_t dataLen)
{
	WaveUtil wave;
	ConvertFormat(pwfx, &wave.m_info.fmt);

	wave.GiveData(pData, dataLen);
	bool ret = wave.Save(path);
	wave.TakeData(&pData, &dataLen);
	return ret;
}
