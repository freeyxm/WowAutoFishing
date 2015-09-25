#pragma once
#include <list>

typedef struct
{
	UINT nDataLen;
	BYTE *pData;
} AudioFrameData;

class AudioFrameStorage
{
public:
	typedef std::list<AudioFrameData*> AudioFrameList;
	typedef AudioFrameList::const_iterator AudioFrameCIter;

public:
	AudioFrameStorage();
	~AudioFrameStorage();

	bool PushBack(BYTE *pData, UINT32 nDataLen);
	bool ReplaceFront(BYTE *pData, UINT32 nDataLen);

	bool PushBack(AudioFrameData *pFrame);
	AudioFrameData* PopFront();

	void Clear();

	inline UINT GetSize() const;
	inline UINT GetTotalBytes() const;

	inline AudioFrameCIter cbegin() const;
	inline AudioFrameCIter cend() const;

private:
	inline void PushBack(AudioFrameData *pFrame, BYTE *pData, UINT32 nDataLen);

private:
	AudioFrameList m_datas;
	UINT m_totalBytes;
};

inline UINT AudioFrameStorage::GetSize() const
{
	return m_datas.size();
}

inline UINT AudioFrameStorage::GetTotalBytes() const
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
