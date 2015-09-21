#include "stdafx.h"
#include "SoundListener.h"
#include "WaveGraph/Util/FFT.h"
#include "WaveGraph/Util/FastFourierTransform.h"
#include <functiondiscoverykeys.h>
#include <cstdio>
#include <map>

void TestData(BYTE *pData, UINT32 nDataLen);

SoundListener::SoundListener(Fisher *pFisher)
	:m_pFisher(pFisher), m_funCheckTimeout(NULL), m_funNotifyBite(NULL)
{
}

SoundListener::~SoundListener()
{
}

void SoundListener::SetCheckTimeout(Fun_CheckTimeout callback)
{
	m_funCheckTimeout = callback;
}

void SoundListener::SetNotifyBite(Fun_NotifyBite callback)
{
	m_funNotifyBite = callback;
}

HRESULT SoundListener::OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone)
{
	if (pData != NULL)
	{
		//if (MatchSound2(pData, nDataLen))
		if (MatchSound(pData, nDataLen))
		{
			if (m_pFisher != NULL && m_funNotifyBite != NULL)
			{
				(m_pFisher->*m_funNotifyBite)();
			}
			*bDone = true;
		}
		else
		{
			*bDone = false;
		}
	}
	
	//TestData(pData, nDataLen);
	
	return S_OK;
}

bool SoundListener::LoopWait()
{
	if (m_pFisher != NULL && m_funCheckTimeout != NULL)
	{
		return (m_pFisher->*m_funCheckTimeout)();
	}
	return false;
}

bool SoundListener::MatchSound(BYTE *pData, UINT32 nDataLen)
{
	static int global_index = 0;
	static int start_index = -1;
	int count = nDataLen / (m_pwfx->wBitsPerSample >> 3);
	UINT32 maxValue = 1 << (m_pwfx->wBitsPerSample - 1);
	UINT32 midValue = maxValue >> 1;
	std::map<int, int> counter;

	++global_index;

	UINT32 value;
	// 只处理1个声道的数据
	for (int i = 0; i < count; i += m_pwfx->nChannels)
	{
		switch (m_pwfx->wBitsPerSample)
		{
		case 8:
			value = *(pData + i);
			break;
		case 16:
			value = *((UINT16*)pData + i);
			break;
		case 32:
			value = *((float*)pData + i);
			break;
		default:
			value = midValue;
			break;
		}
		value = (UINT32)(value * 100.0f / maxValue) % 100;
		counter[value]++;
	}

	int total_num = count / m_pwfx->nChannels;
	float sum = 0.0f;
	std::map<int, int>::iterator it = counter.begin();
	while (it != counter.end())
	{
		sum += (float)(it->first * it->second) / total_num;
		++it;
	}
	if (sum > 47.5) // 目前只检查声音大小
	{
		//printf("%.2f, %d\n", sum, global_index);
		return true;
	}

	//if (sum > 44)
	//{
	//	if (start_index == -1)
	//	{
	//		start_index = global_index;
	//		printf("start at %d\n", global_index);
	//	}
	//}
	//else
	//{
	//	if (start_index >= 0)
	//	{
	//		start_index = -1;
	//		printf("end at %d\n", global_index);
	//	}
	//}
	

	return false;
}


const int COUNTER_SIZE = 256;
static int m_counter[COUNTER_SIZE];
static int total_count = 0;
static std::map<int, int> m_sumCounter;

bool SoundListener::MatchSound2(BYTE *pData, UINT32 nDataLen)
{
	++total_count;

	int sum = 0;
	BYTE pre = 0;
	for (UINT32 i = 0; i < nDataLen; ++i)
	{
		sum += pData[i] - pre;
		pre = pData[i];
	}

	if (m_sumCounter.find(sum) == m_sumCounter.end())
	{
		m_sumCounter.insert(std::pair<int, int>(sum, 1));
		//printf("get new sum: %d, %d\n", sum, nDataLen);
	}
	else
	{
		m_sumCounter[sum]++;
	}

	// 采用简单的“特征值”计算方式，抗干扰能力差，有待改进！！！
	if (sum == 62)
	{
		//printf("sum: %d, count: %d, total_count: %d\n", sum, m_sumCounter[sum], total_count);
		return true;
	}

	return false;
}


std::vector<std::complex<double> > vecList;
#define SAMPLE_SIZE 512
CFastFourierTransform fft(SAMPLE_SIZE);
float g_buffer[SAMPLE_SIZE];

void TestData(BYTE *pData, UINT32 nDataLen)
{
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