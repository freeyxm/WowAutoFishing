#pragma once
#include <list>

typedef struct
{
	UINT nDataLen;
	BYTE *pData;
} AudioFrameData;

typedef std::list<AudioFrameData*> AudioFrameList;
typedef AudioFrameList::const_iterator AudioFrameCIter;

class AudioFrameStorage
{
public:
	AudioFrameStorage();
	~AudioFrameStorage();

	bool PushBack(BYTE *pData, UINT32 nDataLen);
	bool ReplaceFront(BYTE *pData, UINT32 nDataLen);

	bool PushBack(AudioFrameData *pFrame);
	AudioFrameData* PopFront();

	void Clear();

	inline UINT GetSize();
	inline UINT GetTotalBytes();

	inline AudioFrameCIter cbegin();
	inline AudioFrameCIter cend();

private:
	inline void PushBack(AudioFrameData *pFrame, BYTE *pData, UINT32 nDataLen);

private:
	AudioFrameList m_datas;
	UINT m_totalBytes;
};

inline UINT AudioFrameStorage::GetSize()
{
	return m_datas.size();
}

inline UINT AudioFrameStorage::GetTotalBytes()
{
	return m_totalBytes;
}

inline AudioFrameCIter AudioFrameStorage::cbegin()
{
	return m_datas.cbegin();
}

inline AudioFrameCIter AudioFrameStorage::cend()
{
	return m_datas.cend();
}
