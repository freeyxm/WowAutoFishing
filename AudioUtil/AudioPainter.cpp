#include "stdafx.h"
#include "AudioPainter.h"
#include "Win32Util/AudioCapture.h"


AudioPainter::AudioPainter()
	: m_pwfx(NULL), m_scaleY(1.0f), m_bEnable(true)
{
}


AudioPainter::~AudioPainter()
{
}


void AudioPainter::SetFormat(const WAVEFORMATEX *pwfx)
{
	m_pwfx = pwfx;

	m_bFloatFormat = AudioCapture::IsFloatFormat(pwfx);

	m_nBytesPerSample = pwfx->wBitsPerSample / 8;
	m_nBytesPerFrame = m_nBytesPerSample * m_pwfx->nChannels;
	m_maxValue = (1L << (pwfx->wBitsPerSample - 1)) - 1;
	m_midValue = m_maxValue >> 1;
}

void AudioPainter::AddSource(const AudioFrameStorage *pStorage)
{
	m_dataList.push_back(pStorage);
}

void AudioPainter::Clear()
{
	m_dataList.clear();
}

void AudioPainter::Paint(HWND hwnd, HDC hdc, RECT rect, float maxTime)
{
	if (!m_bEnable || m_pwfx == NULL || m_dataList.empty())
		return;

	UINT maxBytes = 0;
	if (maxTime > 0.01f)
	{
		maxBytes = (UINT)(m_pwfx->nAvgBytesPerSec * maxTime);
	}
	else
	{
		for (std::list<const AudioFrameStorage *>::const_iterator it = m_dataList.begin(); it != m_dataList.end(); ++it)
		{
			maxBytes += (*it)->GetTotalBytes();
		}
	}
	

	int x = rect.left;
	int y = 0;

	int w = rect.right - rect.left;
	int h = rect.top;
	int h2 = h / 2;
	int m = rect.bottom;

	::MoveToEx(hdc, x, m, NULL);
	::LineTo(hdc, x + w, m);

	::MoveToEx(hdc, x, m - h2, NULL);
	::LineTo(hdc, x + w, m - h2);

	::MoveToEx(hdc, x, m + h2, NULL);
	::LineTo(hdc, x + w, m + h2);

	UINT step = (maxBytes / w);
	if (step < m_nBytesPerFrame)
		step = m_nBytesPerFrame;

	float min, max;
	for (std::list<const AudioFrameStorage *>::const_iterator it = m_dataList.cbegin(); it != m_dataList.cend(); ++it)
	{
		ResetIter(*it);
		while (GetNext(step, &min, &max) > 0)
		{
			y = (int)(min * m_scaleY * h2);
			::MoveToEx(hdc, x, m, NULL);
			::LineTo(hdc, x, m + y);

			y = (int)(max * m_scaleY * h2);
			::MoveToEx(hdc, x, m, NULL);
			::LineTo(hdc, x, m + y);

			++x;
		}
	}
}


void AudioPainter::ResetIter(const AudioFrameStorage *pStorage)
{
	m_pCurStorage = pStorage;
	m_dataIter = m_pCurStorage->cbegin();
	m_dataIndex = 0;
}

UINT AudioPainter::GetNext(UINT range, float *pMin, float *pMax)
{
	UINT count = 0;
	range /= m_nBytesPerFrame;

	float value = 0, min = 0, max = 0;
	while (count < range && m_dataIter != m_pCurStorage->cend())
	{
		BYTE *pData = (*m_dataIter)->pData;
		UINT maxIndex = (*m_dataIter)->nDataLen / m_nBytesPerSample;

		for (; m_dataIndex < maxIndex; m_dataIndex += m_pwfx->nChannels) // 只处理1个声道
		{
			if (count >= range)
				break;

			value = ParseValue(pData, m_dataIndex);

			if (value < min)
				min = value;
			if (value > max)
				max = value;

			++count;
		}

		if (count < range)
		{
			++m_dataIter;
			m_dataIndex = 0;
		}
	}

	if (count > 0)
	{
		*pMin = min;
		*pMax = max;
	}

	return count;
}

float AudioPainter::ParseValue(BYTE *pData, UINT index)
{
	switch (m_pwfx->wBitsPerSample)
	{
	case 8:
		return (float)*(pData + index) / m_midValue;
		break;
	case 16:
		return (float)*((INT16*)pData + index) / m_midValue;
		break;
	case 32:
		if (m_bFloatFormat)
			return (float)*((float*)pData + index);
		else
			return (float)*((int*)pData + index) / m_midValue;
		break;
	default:
		break;
	}
	return 0;
}

void AudioPainter::AddScale(float scale)
{
	m_scaleY += scale;

	if (m_scaleY < 0)
		m_scaleY = 0;
}

float AudioPainter::GetScale()
{
	return m_scaleY;
}

bool AudioPainter::IsEnable() const
{
	return m_bEnable;
}

void AudioPainter::SetEnable(bool enable)
{
	m_bEnable = enable;
}