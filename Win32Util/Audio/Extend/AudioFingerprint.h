#pragma once
#include "AudioFrameStorage.h"
#include <Audioclient.h>
#include <vector>

using std::vector;

class AudioFingerprint
{
public:
    typedef void(*ProcessFunc)(vector<float> &data);

public:
    AudioFingerprint();
    ~AudioFingerprint();

    const vector<float>& GetFingerprint(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx, ProcessFunc processFunc = nullptr);

    static void ProcessDiff(vector<float> &data);
    static void ProcessCutAvg(vector<float> &data);
    static void ProcessRatioAvg(vector<float> &data);

private:
    vector<float> m_finger;
};

