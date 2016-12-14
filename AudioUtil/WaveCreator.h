﻿#pragma once
#include <Audioclient.h>
#include "WaveFile.h"
#include "AudioFrameStorage.h"

class WaveCreator : public WaveFile
{
public:
	WaveCreator();
	~WaveCreator();

	static bool LoadWave(const char *path, WAVEFORMATEX *pwfx, char **ppData, uint32_t *pDataLen);
	static bool SaveWave(const char *path, WAVEFORMATEX *pwfx, char *pData, uint32_t dataLen);
};

