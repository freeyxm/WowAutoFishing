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
	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);

	const AudioFrameStorage* GetStorage() const;

private:
	CRITICAL_SECTION m_dataSection; // lock
	AudioFrameStorage m_dataStorage;
	UINT m_dataMaxBytes;

	HANDLE m_hThreadCapture;
};
