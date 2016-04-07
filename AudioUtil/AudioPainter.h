#pragma once
#include "AudioFrameStorage.h"
#include <Audioclient.h>
#include <list>

class AudioPainter
{
public:
	AudioPainter();
	~AudioPainter();

	void SetFormat(const WAVEFORMATEX *pwfx);
	void AddSource(const AudioFrameStorage *pStorage);
	void Clear();

	void Paint(HDC hdc, RECT rect, float maxTime = 0.0f);

	void AddScale(float scale);
	float GetScale();

	bool IsEnable() const;
	void SetEnable(bool enable);

private:
	void ResetIter(const AudioFrameStorage *pStorage);
	UINT GetNext(UINT range, float *pMin, float *pMax);
	float ParseValue(BYTE *pData, UINT index);

private:
	std::list<const AudioFrameStorage*> m_dataList;
	const WAVEFORMATEX *m_pwfx;

	bool m_bFloatFormat;
	UINT m_nBytesPerSample;
	UINT m_nBytesPerFrame;
	int m_maxValue;
	int m_midValue;

	const AudioFrameStorage *m_pCurStorage;
	AudioFrameStorage::AudioFrameCIter m_dataIter;
	UINT m_dataIndex;

	float m_scaleY;
	bool m_bEnable;
};

