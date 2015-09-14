#include "stdafx.h"
#include "SoundListener.h"
#include "Lib/FFT.h"
#include "Lib/FastFourierTransform.h"
#include <functiondiscoverykeys.h>
#include <cstdio>
#include <hash_map>

void TestData(BYTE *pData, UINT32 nDataLen);

SoundListener::SoundListener(Fisher *pFisher)
	:m_pFisher(pFisher)
{
}

SoundListener::~SoundListener()
{
}

void SoundListener::SetCheckTimeout(FUN_CheckTimeout callback)
{
	m_cbCheckTimeout = callback;
}

void SoundListener::SetNotifyBite(FUN_NotifyBite callback)
{
	m_cbNotifyBite = callback;
}

const int COUNTER_SIZE = 256;
static int m_counter[COUNTER_SIZE];
static int total_count = 0;
static std::hash_map<int, int> m_sumCounter;

HRESULT SoundListener::RecordData(BYTE *pData, UINT32 nDataLen, BOOL *bDone)
{
	++total_count;
	//::memset(m_counter, 0, sizeof(m_counter));

	int sum = 0;
	BYTE pre = 0;
	for (int i = 0; i < nDataLen; ++i)
	{
		//++m_counter[pData[i]];
		sum += pData[i] - pre;
		pre = pData[i];
	}

	//float sum2 = 0;
	//for (int i = 0; i < COUNTER_SIZE; ++i)
	//{
	//	sum2 += i * m_counter[i] * 1.0f / nDataLen;
	//}

	if (m_sumCounter.find(sum) == m_sumCounter.end())
	{
		m_sumCounter.insert(std::pair<int, int>(sum, 1));
		printf("get new sum: %d, %d\n", sum, nDataLen);
	}
	else
	{
		m_sumCounter[sum]++;
	}

	//printf("RecordData: %d, sum = %d, sum2 = %.2f, total_count: %d\n", nDataLen, sum, sum2, total_count);
	
	// 采用简单的“特征值”计算方式，抗干扰能力差，有待改进！！！
	if (sum == 62)
	{
		//printf("sum: %d, count: %d, total_count: %d\n", sum, m_sumCounter[sum], total_count);
		if (m_cbNotifyBite != NULL)
		{
			(m_pFisher->*m_cbNotifyBite)();
		}
		*bDone = true;
	}
	else
	{
		*bDone = false;
	}

	//TestData(pData, nDataLen);
	
	return S_OK;
}

BOOL SoundListener::NotifyLoop()
{
	if (m_cbCheckTimeout != NULL)
	{
		return (m_pFisher->*m_cbCheckTimeout)();
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