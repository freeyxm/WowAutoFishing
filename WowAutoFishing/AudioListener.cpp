﻿#include "stdafx.h"
#include "AudioListener.h"
#include "WaveGraph/Util/FFT.h"
#include "WaveGraph/Util/FastFourierTransform.h"
#include <functiondiscoverykeys.h>
#include <cstdio>
#include <map>

void TestData(BYTE *pData, UINT32 nDataLen);

AudioListener::AudioListener(Fisher *pFisher)
	:m_pFisher(pFisher), m_funCheckTimeout(NULL), m_funNotifyBite(NULL)
{
}

AudioListener::~AudioListener()
{
}

void AudioListener::SetCheckTimeout(Fun_CheckTimeout callback)
{
	m_funCheckTimeout = callback;
}

void AudioListener::SetNotifyBite(Fun_NotifyBite callback)
{
	m_funNotifyBite = callback;
}

HRESULT AudioListener::OnCaptureData(BYTE *pData, UINT32 nFrameCount)
{
	if (pData != NULL)
	{
		if (MatchSound(pData, nFrameCount))
		{
			if (m_pFisher != NULL && m_funNotifyBite != NULL)
			{
				(m_pFisher->*m_funNotifyBite)();
			}
			SetDone(true);
		}
		else
		{
			SetDone(false);
		}
	}
	
	//TestData(pData, nDataLen);
	
	return S_OK;
}

bool AudioListener::IsDone() const
{
	if (!m_bDone && m_pFisher != NULL && m_funCheckTimeout != NULL)
	{
		return (m_pFisher->*m_funCheckTimeout)();
	}
	return m_bDone;
}

bool AudioListener::MatchSound(BYTE *pData, UINT32 nFrameCount)
{
	static float silent_min = 0.001f;
	static float fish_min = 0.4f;
	static float g_min = 0, g_max = 0;
	static int g_globalIndex = 0;
	static int g_startIndex = -1;
	UINT32 count = nFrameCount * m_pwfx->nChannels;
	
	++g_globalIndex;
	if (pData != NULL)
	{
		float value = 0, min = 0, max = 0;
		for (UINT i = 0; i < count; i += m_pwfx->nChannels) // 只处理1个声道的数据
		{
			value = ParseValue(pData, i);

			if (value < min)
				min = value;
			if (value > max)
				max = value;
		}

		if (max > fish_min || min <-fish_min)
		{
			printf("Bite: %d, %f, %f\n", g_globalIndex, min, max);
			return true;
		}

		/*if (max > silent_min || min < -silent_min)
		{
			if (g_startIndex == -1)
			{
				g_min = g_max = 0;
				printf("----------------------------\n");
				printf("start at %d, %f, %f\n", g_globalIndex, min, max);
				g_startIndex = g_globalIndex;
			}
			if (min < g_min)
				g_min = min;
			if (max > g_max)
				g_max = max;
		}
		else
		{
			if (g_startIndex != -1)
			{
				printf("end at %d, %f, %f\n", g_globalIndex, min, max);
				printf("g_min = %f, g_max = %f\n", g_min, g_max);
				printf("----------------------------\n");
				g_startIndex = -1;
			}
		}*/
	}

	return false;
}

void TestData(BYTE *pData, UINT32 nDataLen)
{
	static const int SAMPLE_SIZE = 512;
	static std::vector<std::complex<double> > vecList;
	static CFastFourierTransform fft(SAMPLE_SIZE);
	static float g_buffer[SAMPLE_SIZE];
	static int total_count = 0;

	//vecList.clear();
	//for (int i = 0; i < nDataLen; ++i)
	//{
	//	vecList.push_back(std::complex<double>(pData[i], 0));
	//}

	//for (int i = nDataLen; i < maxLen; ++i)
	//{
	//	vecList.push_back(std::complex<double>(0, 0));
	//}
	//unsigned long len = nDataLen;
	//FFT::DoFFT(len, vecList);

	int size = min(nDataLen, SAMPLE_SIZE);
	for (int i = 0; i < size; ++i)
	{
		g_buffer[i] = pData[i] / 128.0f;
	}
	for (int i = size; i < SAMPLE_SIZE; ++i)
	{
		g_buffer[i] = 0.0f;
	}
	float *result = fft.Calculate(g_buffer, SAMPLE_SIZE);

	bool bFind = false;
	for (int i = 2; i < SAMPLE_SIZE; ++i)
	{
		if (result[i] > 0.4f)
		{
			printf("%d: %f, %d\n", i, result[i], total_count);
			bFind = true;
		}
	}

	if (bFind)
	{
		printf("---------------------------------------------------------\n");
	}

	//double sum = 0;
	//for (int i = 0; i < nDataLen; ++i)
	//{
	//	double real = vecList[i].real();
	//	double imag = vecList[i].imag();
	//	sum += (real * real + imag *imag);
	//}
	//double avg = sum / maxLen;
	//if (avg > 100)
	//{
	//	printf("max: %f, %d\n", avg, total_count);
	//}
}