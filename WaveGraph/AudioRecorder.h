#pragma once
#include "Win32Util/AudioCapture.h"
#include "AudioFrameStorage.h"
#include <list>
#include <process.h>

typedef struct
{
	int nDataLen;
	BYTE *pData;
} AudioData;

class AudioRecorder :
	public AudioCapture
{
public:
	AudioRecorder(void);
	~AudioRecorder(void);

	bool StartRecord();
	void StopRecord();
	void Clear();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nDataLen, BOOL *bDone);
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
	AudioFrameStorage m_dataStorage;
	UINT m_dataMaxBytes;

	bool m_bDone;
	HANDLE m_hThreadCapture;

	AudioFrameCIter m_dataIter;
	UINT m_dataIndex;

	UINT m_nBytesPerSample;
	int m_maxValue;
	int m_midValue;

	float m_scaleY;
};

