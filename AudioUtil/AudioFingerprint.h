#pragma once
#include "AudioFrameStorage.h"
#include <Audioclient.h>
#include <vector>

using std::vector;

class AudioFingerprint
{
public:
	AudioFingerprint();
	~AudioFingerprint();

	static vector<float> getFingerprint(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx);
};

