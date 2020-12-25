#include "stdafx.h"
#include "AudioFingerprint.h"
#include "Win32Util/Audio/AudioUtil.h"
#include "CommUtil/VectorUtil.hpp"

using comm_util::VectorUtil;

AudioFingerprint::AudioFingerprint()
{
}


AudioFingerprint::~AudioFingerprint()
{
}


const vector<float>& AudioFingerprint::GetFingerprint(const AudioFrameStorage *source, const WAVEFORMATEX *pwfx, ProcessFunc processFunc)
{
    m_finger.resize(source->GetSize());

    int nBytesPerSample = pwfx->wBitsPerSample / 8;
    int midValue = AudioUtil::GetMidValue(pwfx);

    int index = 0;
    for (auto it = source->cbegin(); it != source->cend(); ++it)
    {
        float min = FLT_MAX;
        float max = FLT_MIN;
        AudioFrameData *pFrames = *it;

        UINT count = pFrames->nDataLen / nBytesPerSample;
        for (UINT i = 0; i < count; i += pwfx->nChannels)
        {
            float value = AudioUtil::ParseValue(pwfx, pFrames->pData, i, midValue);
            if (min > value)
                min = value;
            if (max < value)
                max = value;
        }

        m_finger[index++] = max - min;
    }

    if (processFunc != nullptr)
    {
        processFunc(m_finger);
    }

    return m_finger;
}

void AudioFingerprint::ProcessDiff(vector<float> &data)
{
    size_t count = data.size();
    for (size_t i = 1; i < count; ++i)
    {
        data[i - 1] -= data[i];
    }
    if (count > 1)
    {
        data[count - 1] = 0;
    }
}

void AudioFingerprint::ProcessCutAvg(vector<float> &data)
{
    float avg = VectorUtil::GetAvg(&data[0], data.size());
    for (size_t i = 0, count = data.size(); i < count; ++i)
    {
        data[i] -= avg;
    }
}

void AudioFingerprint::ProcessRatioAvg(vector<float> &data)
{
    float avg = VectorUtil::GetAvg(&data[0], data.size());
    for (size_t i = 0, count = data.size(); i < count; ++i)
    {
        data[i] /= avg;
    }
}
