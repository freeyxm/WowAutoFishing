#pragma once
#include <list>

typedef struct
{
	int nDataLen;
	BYTE *pData;
} AudioFrameData;

typedef std::list<AudioFrameData> AudioFrameList;
typedef AudioFrameList::iterator AudioFrameIter;

class AudioFrameStorage
{
public:
	AudioFrameStorage();
	~AudioFrameStorage();

	void AddData(BYTE *pData, UINT32 nDataLen);
	void Clear();

	inline UINT GetTotalBytes();

	inline AudioFrameIter begin();
	inline AudioFrameIter end();

private:
	AudioFrameList m_datas;
	UINT m_totalBytes;
};

