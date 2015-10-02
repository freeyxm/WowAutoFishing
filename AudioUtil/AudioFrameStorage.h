#pragma once
#include <list>
#include <stdint.h>

typedef struct
{
	uint32_t nDataLen;
	uint8_t *pData;
} AudioFrameData;

class AudioFrameStorage
{
public:
	typedef std::list<AudioFrameData*> AudioFrameList;
	typedef AudioFrameList::const_iterator AudioFrameCIter;

public:
	AudioFrameStorage();
	~AudioFrameStorage();

	bool PushBack(uint8_t *pData, uint32_t nDataLen);
	bool ReplaceFront(uint8_t *pData, uint32_t nDataLen);

	bool PushBack(AudioFrameData *pFrame);
	AudioFrameData* PopFront();

	void Clear();
	void ClearCache();

	void SetCacheSize(uint32_t size);

	inline uint32_t GetSize() const;
	inline uint32_t GetTotalBytes() const;

	inline AudioFrameCIter cbegin() const;
	inline AudioFrameCIter cend() const;

private:
	inline void PushBack(AudioFrameData *pFrame, uint8_t *pData, uint32_t nDataLen);
	inline bool ResizeFrame(AudioFrameData *pFrame, uint32_t nDataLen);

private:
	AudioFrameList m_datas;
	AudioFrameList m_cache;
	uint32_t m_totalBytes;
	uint32_t m_nCacheSize;
};

inline uint32_t AudioFrameStorage::GetSize() const
{
	return m_datas.size();
}

inline uint32_t AudioFrameStorage::GetTotalBytes() const
{
	return m_totalBytes;
}

inline AudioFrameStorage::AudioFrameCIter AudioFrameStorage::cbegin() const
{
	return m_datas.cbegin();
}

inline AudioFrameStorage::AudioFrameCIter AudioFrameStorage::cend() const
{
	return m_datas.cend();
}
