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

	void Clear();

	inline UINT GetTotalBytes();

	inline AudioFrameCIter begin();
	inline AudioFrameCIter end();

private:
	inline void PushBack(AudioFrameData *pFrame, BYTE *pData, UINT32 nDataLen);

private:
	AudioFrameList m_datas;
	UINT m_totalBytes;
};

inline UINT AudioFrameStorage::GetTotalBytes()
{
	return m_totalBytes;
}

inline AudioFrameCIter AudioFrameStorage::begin()
{
	return m_datas.begin();
}

inline AudioFrameCIter AudioFrameStorage::end()
{
	return m_datas.end();
}
