#pragma once
#include "Win32Util/Audio/AudioCapture.h"
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
	AudioRecorder(bool bLoopback = true, bool bDefaultDevice = true);
	~AudioRecorder(void);

	bool Start();
	void Stop();
	void Clear();

	inline void Lock();
	inline void Unlock();

	const AudioFrameStorage* GetStorage() const;

protected:
	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual HRESULT OnCaptureData(BYTE *pData, UINT32 nFrameCount);

private:
	CRITICAL_SECTION m_dataSection; // lock
	AudioFrameStorage m_dataStorage;
	UINT m_dataMaxBytes;

	HANDLE m_hThreadCapture;
};

