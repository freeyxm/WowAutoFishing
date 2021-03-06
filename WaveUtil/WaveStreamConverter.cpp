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
	m_stream->read(pData, (std::streamsize)frameCount * m_srcBytesPerFrame);
	if (*m_stream)
	{
		return frameCount;
	}
	else
	{
        std::streamsize readCount = m_stream->gcount();
        std::streamsize readFrameCount = readCount / m_srcBytesPerFrame;
        std::streamsize count = readFrameCount * m_srcBytesPerFrame;
        if (count < readCount)
        {
            m_stream->seekg(-(std::streamoff)(readCount - count), std::ios::cur);
        }
        return (uint32_t)readFrameCount;
	}
}