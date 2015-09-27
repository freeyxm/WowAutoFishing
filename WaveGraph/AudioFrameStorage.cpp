#include "stdafx.h"
#include "AudioFrameStorage.h"

AudioFrameStorage::AudioFrameStorage()
	: m_totalBytes(0), m_nCacheSize(0)
{
}

AudioFrameStorage::~AudioFrameStorage()
{
	Clear();
	ClearCache();
}

bool AudioFrameStorage::PushBack(AudioFrameData *pFrame)
{
	if(pFrame != NULL)
	{
		m_datas.push_back(pFrame);
		m_totalBytes += pFrame->nDataLen;
		return true;
	}
	return false;
}

AudioFrameData* AudioFrameStorage::PopFront()
{
	if(!m_datas.empty())
	{
		AudioFrameData *pFrame = m_datas.front();
		m_datas.pop_front();
		m_totalBytes -= pFrame->nDataLen;
		return pFrame;
	}
	return NULL;
}

bool AudioFrameStorage::PushBack(BYTE *pData, UINT32 nDataLen)
{
	AudioFrameData *pFrame = NULL;
	if(m_cache.size() > 0)
	{
		pFrame = m_cache.front();
		if(pFrame->nDataLen < nDataLen)
		{
			if(!ResizeFrame(pFrame, nDataLen))
				return false;
		}
		m_cache.pop_front();
	}
	else
	{
		pFrame = new AudioFrameData();
		if(pFrame == NULL)
		{
			printf("new AudioFrameData failed.\n");
			return false;
		}
		pFrame->pData = (BYTE*)::malloc(nDataLen);
		if(pFrame->pData == NULL)
		{
			delete pFrame;
			printf("malloc failed.\n");
			return false;
		}
		pFrame->nDataLen = nDataLen;
	}

	PushBack(pFrame, pData, nDataLen);

	return true;
}

inline void AudioFrameStorage::PushBack(AudioFrameData *pFrame, BYTE *pData, UINT32 nDataLen)
{
	if (pData == NULL) // silent data.
		::memset(pFrame->pData, 0, nDataLen);
	else
		::memcpy(pFrame->pData, pData, nDataLen);

	m_datas.push_back(pFrame);
	m_totalBytes += nDataLen;
}

bool AudioFrameStorage::ReplaceFront(BYTE *pData, UINT32 nDataLen)
{
	if(m_datas.empty())
		return false;

	AudioFrameData *pFrame = m_datas.front();
	UINT preLen = pFrame->nDataLen;
	if(pFrame->nDataLen < nDataLen)
	{
		if(!ResizeFrame(pFrame, nDataLen))
			return false;
	}

	m_datas.pop_front();
	m_totalBytes -= preLen;

	PushBack(pFrame, pData, nDataLen);

	return true;
}

inline bool AudioFrameStorage::ResizeFrame(AudioFrameData *pFrame, UINT32 nDataLen)
{
	void *pMem = ::realloc(pFrame->pData, nDataLen);
	if(pMem == NULL)
	{
		printf("realloc failed.\n");
		return false;
	}
	else
	{
		pFrame->pData = (BYTE*)pMem;
		pFrame->nDataLen = nDataLen;
		return true;
	}
}

void AudioFrameStorage::Clear()
{
	for (AudioFrameCIter it = m_datas.begin(); it != m_datas.end();)
	{
		AudioFrameData *pFrame = *it;
		if(m_cache.size() < m_nCacheSize)
		{
			m_cache.push_back(pFrame);
		}
		else
		{
			::free(pFrame->pData);
			delete pFrame;
		}
		it = m_datas.erase(it);
	}
	m_totalBytes = 0;
}

void AudioFrameStorage::ClearCache()
{
	for (AudioFrameCIter it = m_cache.begin(); it != m_cache.end();)
	{
		AudioFrameData *pFrame = *it;
		::free(pFrame->pData);
		delete pFrame;
		it = m_cache.erase(it);
	}
}

void AudioFrameStorage::SetCacheSize(UINT size)
{
	m_nCacheSize = size;
}
