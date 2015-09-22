#pragma once
#include "Win32Util/SoundCapture.h"
#include <list>
#include <process.h>

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

	void Paint(HWND hwnd, HDC hdc);

	void AddScale(float scale);
	float GetScale();

private:
	void ResetIter();
	UINT GetNext(UINT range, float *pMin, float *pMax);

private:
	CRITICAL_SECTION m_dataSection; // lock
	std::list<AudioData> m_dataList;
	UINT m_dataCurBytes;
	UINT m_dataMaxBytes;

	bool m_waveFormatFloat;

	bool m_bDone;
	HANDLE m_hThreadCapture;

	std::list<AudioData>::iterator m_dataIter;
	UINT m_dataIndex;

	UINT m_nBytesPerSample;
	int m_maxValue;
	int m_midValue;

	float m_scaleY;
};

