#pragma once
#include <Audioclient.h>
#include "AudioFrameStorage.h"

struct WaveRiffFormat
{
	char chunkId[4]; // "fmt "
	UINT32 chunkSize;
	WAVEFORMATEX wfx;
	BYTE *pExtData;
};

struct WaveRiffData
{
	char chunkId[4]; // "data"
	UINT32 chunkSize;
	BYTE *pData;
};

struct WaveHead
{
	char chunkID[4]; // "RIFF"
	UINT32 chunkSize;
	char format[4]; // "WAVE"
	WaveRiffFormat fmt;
	WaveRiffData data;
};

class WaveCreator
{
public:
	WaveCreator();
	~WaveCreator();

	static bool LoadWave(const char *path, WAVEFORMATEX *pwfx, BYTE **ppData, UINT32 *pDataLen, BYTE **ppExtData);
	static bool SaveWave(const char *path, WAVEFORMATEX *pwfx, BYTE *pData);

	bool BeginSave();
	bool AppendData(BYTE *pData, UINT32 nDataLen);
	bool EndSave();
};

