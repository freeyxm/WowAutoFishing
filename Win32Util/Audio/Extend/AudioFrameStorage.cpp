﻿#include "stdafx.h"
#include "AudioFrameStorage.h"

AudioFrameStorage::AudioFrameStorage()
	: m_totalBytes(0), m_cacheSize(0)
{
}

AudioFrameStorage::~AudioFrameStorage()
{
	Clear();
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

bool AudioFrameStorage::PushBack(uint8_t *pData, uint32_t nDataLen)
{
	AudioFrameData *pFrame = NULL;
	if(!m_cache.empty())
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
		pFrame->pData = (uint8_t*)::malloc(nDataLen);
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

inline void AudioFrameStorage::PushBack(AudioFrameData *pFrame, uint8_t *pData, uint32_t nDataLen)
{
	if (pData == NULL) // silent data.
		::memset(pFrame->pData, 0, nDataLen);
	else
		::memcpy(pFrame->pData, pData, nDataLen);

	m_datas.push_back(pFrame);
	m_totalBytes += nDataLen;
}

bool AudioFrameStorage::ReplaceFront(uint8_t *pData, uint32_t nDataLen)
{
	if(m_datas.empty())
		return false;

	AudioFrameData *pFrame = m_datas.front();
	uint32_t preLen = pFrame->nDataLen;
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

inline bool AudioFrameStorage::ResizeFrame(AudioFrameData *pFrame, uint32_t nDataLen)
{
	void *pMem = ::realloc(pFrame->pData, nDataLen);
	if(pMem == NULL)
	{
		printf("realloc failed.\n");
		return false;
	}
	else
	{
		pFrame->pData = (uint8_t*)pMem;
		pFrame->nDataLen = nDataLen;
		return true;
	}
}

void AudioFrameStorage::Reset()
{
	for (AudioFrameCIter it = m_datas.begin(); it != m_datas.end();)
	{
		AudioFrameData *pFrame = *it;
		if(m_cache.size() < m_cacheSize)
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

void AudioFrameStorage::Clear()
{
	Clear(m_datas);
	Clear(m_cache);
	m_totalBytes = 0;
}

void AudioFrameStorage::ClearCache()
{
	Clear(m_cache);
}

void AudioFrameStorage::Clear(AudioFrameList &list)
{
	for (AudioFrameCIter it = list.begin(); it != list.end(); ++it)
	{
		AudioFrameData *pFrame = *it;
		::free(pFrame->pData);
		delete pFrame;
	}
	list.clear();
}

void AudioFrameStorage::SetCacheSize(uint32_t size)
{
	m_cacheSize = size;
}
