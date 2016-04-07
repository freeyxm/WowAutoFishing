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
	static vector<float> getFingerprint_diff(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx);
	static vector<float> getFingerprint_cutAvg(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx);
	static vector<float> getFingerprint_ratioAvg(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx);

private:
	static void processDiff(vector<float> &data);
	static void processCutAvg(vector<float> &data);
	static void processRatioAvg(vector<float> &data);
};

