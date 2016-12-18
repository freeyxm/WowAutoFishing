#include "stdafx.h"
#include "WaveStreamConverter.h"

WaveStreamConverter::WaveStreamConverter(std::istream *stream)
{
	SetStream(stream);
}

WaveStreamConverter::~WaveStreamConverter()
{
}

void WaveStreamConverter::SetStream(std::istream *stream)
{
	m_stream = stream;
}

uint32_t WaveStreamConverter::LoadSrcFrame(char *pData, uint32_t frameCount)
{
	m_stream->read(pData, frameCount * m_srcBytesPerFrame);
	if (*m_stream)
	{
		return frameCount;
	}
	else
	{
		uint32_t readCount = m_stream->gcount();
		uint32_t readFrameCount = readCount / m_srcBytesPerFrame;
		uint32_t count = readFrameCount * m_srcBytesPerFrame;
		if (count < readCount)
		{
			m_stream->seekg(-(int)(readCount - count), std::ios::cur);
		}
		return readFrameCount;
	}
}