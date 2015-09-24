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
	AudioFrameData *pFrame = new AudioFrameData();
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

	PushBack(pFrame, pData, nDataLen);

	return true;
}

inline void AudioFrameStorage::PushBack(AudioFrameData *pFrame, BYTE *pData, UINT32 nDataLen)
{
	pFrame->nDataLen = nDataLen;

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
	if(pFrame->nDataLen < nDataLen)
	{
		void *pMem = ::realloc(pFrame->pData, nDataLen);
		if(pMem == NULL)
		{
			printf("realloc failed.\n");
			return false;
		}
		pFrame->pData = (BYTE*)pMem;
	}

	m_datas.pop_front();
	m_totalBytes -= pFrame->nDataLen;

	PushBack(pFrame, pData, nDataLen);

	return true;
}

void AudioFrameStorage::Clear()
{
	for (AudioFrameCIter it = m_datas.begin(); it != m_datas.end();)
	{
		AudioFrameData *p = *it;
		::free(p->pData);
		delete p;
		it = m_datas.erase(it);
	}
	m_totalBytes = 0;
}
