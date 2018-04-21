#pragma once
#include <Audioclient.h>
#include "WaveFile.h"

class WaveUtil
{
public:
	WaveUtil();
	~WaveUtil();

	static bool LoadWave(const char *path, WAVEFORMATEX *pwfx, char **ppData, uint32_t *pDataLen);
	static bool SaveWave(const char *path, WAVEFORMATEX *pwfx, char *pData, uint32_t dataLen);

	static void ConvertFormat(const WaveFile::FormatChunk *pfmt, WAVEFORMATEX *pwfx);
	static void ConvertFormat(const WAVEFORMATEX *pwfx, WaveFile::FormatChunk *pfmt);

	static void SetFormat(WAVEFORMATEX *pwfx, int sampleRate, int bitsPerSample, int channel);

	static bool IsSameFormat(const WAVEFORMATEX *pwfx, const WaveFile::FormatChunk *pfmt);
	static bool IsSameFormat(const WAVEFORMATEX *pwfx, const WAVEFORMATEX *pwfx2);
};

