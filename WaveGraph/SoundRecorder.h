#pragma once
#include "Win32Util/SoundCapture.h"
#include <list>

typedef struct
{
	int nDataLen;
	BYTE *pData;
} AudioData;

class SoundRecorder :
	public SoundCapture
{
public:
	SoundRecorder(void);
	~SoundRecorder(void);

	bool StartRecord();
	void StopRecord();
	void Clear();

	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone);
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual bool LoopDone();

	void SetDone(bool bDone);

	bool Paint(HWND hwnd, HDC hdc);

	void Reset();
	UINT GetNext(UINT range, float *pValue);

private:
	std::list<AudioData> m_dataList;
	UINT m_dataCount;
	bool m_bDone;
	HANDLE m_hThreadCapture;

	std::list<AudioData>::iterator m_dataListIter;
	UINT m_dataIndex;

	UINT m_bytesPerSample;
	int m_maxValue;
	int m_midValue;
};

