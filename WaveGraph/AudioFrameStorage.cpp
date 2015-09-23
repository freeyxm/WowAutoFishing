#include "stdafx.h"
#include "AudioFrameStorage.h"

AudioFrameStorage::AudioFrameStorage()
	: m_totalBytes(0)
{
}

AudioFrameStorage::~AudioFrameStorage()
{
	Clear();
}

void AudioFrameStorage::AddData(BYTE *pData, UINT32 nDataLen)
{
	AudioFrameData data;
	data.nDataLen = nDataLen;
	data.pData = (BYTE*)::malloc(nDataLen);
	if (pData == NULL) // silent data.
		::memset(data.pData, 0, nDataLen);
	else
		::memcpy(data.pData, pData, nDataLen);
	m_datas.push_back(data);
	m_totalBytes += nDataLen;
}

void AudioFrameStorage::Clear()
{
	for (AudioFrameIter it = m_datas.begin(); it != m_datas.end(); ++it)
	{
		::free(it->pData);
		it = m_datas.erase(it);
	}
	m_totalBytes = 0;
}

inline UINT AudioFrameStorage::GetTotalBytes()
{
	return m_totalBytes;
}

inline AudioFrameIter AudioFrameStorage::begin()
{
	return m_datas.begin();
}

inline AudioFrameIter AudioFrameStorage::end()
{
	return m_datas.end();
}
