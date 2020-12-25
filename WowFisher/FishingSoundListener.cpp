#include "stdafx.h"
#include "FishingSoundListener.h"
#include "Fisher.h"
#include "Win32Util/Audio/Extend/AudioFingerprint.h"
#include "CommUtil/StringUtil.hpp"
#include "CommUtil/VectorUtil.hpp"
#include <functiondiscoverykeys.h>
#include <ctime>
#include <map>

using namespace std;
using namespace comm_util;

#define CFG_HIT "#hit = "
#define SAMPLE_FILE "config/fishing sample.txt"

FishingSoundListener::FishingSoundListener(Fisher* pFisher)
    : AudioExtractor(true, true)
    , m_pwfx(NULL)
    , m_pFisher(pFisher)
    , m_sampleCount(0)
    , m_dtw(256, 256)
{
    m_pAudioFingerprint = new AudioFingerprint();
}

FishingSoundListener::~FishingSoundListener()
{
    if (m_sampleFile.is_open())
    {
        m_sampleFile.close();
    }

    SAFE_DELETE(m_pAudioFingerprint);
}

int FishingSoundListener::Init()
{
    if (!m_sampleFile.is_open())
    {
        char buf[128];
        time_t t = ::time(NULL);
        struct tm ti;
        ::localtime_s(&ti, &t);
        ::sprintf_s(buf, "config/fishing sample %04d-%02d-%02d %02d.%02d.%02d.txt", (ti.tm_year + 1900), (ti.tm_mon + 1), ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

        m_sampleFile.open(buf, ios::app | ios::out);
        if (!m_sampleFile.is_open())
        {
            printf("Can't open file: %s\n", buf);
        }
    }

    LoadSamples();

    return AudioExtractor::Init() == 0;
}

HRESULT FishingSoundListener::SetFormat(WAVEFORMATEX* pwfx)
{
    AudioCapture::SetFormat(pwfx);

    m_pwfx = pwfx;
    SetSilentMaxCount(10);
    SetSoundMinCount(20);
    SetSoundMaxCount(pwfx->nSamplesPerSec / 100 / pwfx->nChannels); // about 1 second.
    SetAmpZcr(480, 0.05f, 2.0f, 1.0f, 1.0f); // disable zcr

    return S_OK;
}

void FishingSoundListener::EndSegment()
{
    if (m_pCurSegment->GetSize() < 100)
    {
        m_pCurSegment->Reset();
        return;
    }

    auto sample = m_pAudioFingerprint->GetFingerprint(m_pCurSegment, m_pwfx, AudioFingerprint::ProcessCutAvg);
    m_pCurSegment->Reset();

    bool isMatch = IsSampleMatch(sample);

    if (m_sampleFile.is_open() && !isMatch)
    {
        SaveSample(sample, 0, m_sampleFile);
    }

    if (isMatch)
    {
        m_pFisher->NotifyBite();
    }
}

bool FishingSoundListener::IsDone() const
{
    return m_bDone;
}

void FishingSoundListener::SetAmpL(float ampL)
{
    m_sAmpZcr.ampL = ampL;
}

void FishingSoundListener::SetAmpH(float ampH)
{
    m_sAmpZcr.ampH = ampH;
}

void FishingSoundListener::Save()
{
    SaveSamples();
}

void FishingSoundListener::AddSample(const char* str, int hit)
{
    auto data = StringUtil::ParseValues<float>(str, ",", StringUtil::atof);
    if (!data.empty())
    {
        auto it = m_samples.begin();
        while (it != m_samples.end())
        {
            //float cosa = VectorUtil::getCosA_First(&it->sample[0], it->sample.size(), &data[0], data.size());
            float dtw = m_dtw.Calculate(&it->sample[0], it->sample.size(), &data[0], data.size());
            if (dtw < 3) // if (cosa >= 0.95f)
            {
                it->hit += hit;
                return;
            }
            ++it;
        }
        SampleInfo samp = { data, hit };
        m_samples.push_back(samp);
    }
}

bool FishingSoundListener::IsSampleMatch(const std::vector<float>& data)
{
    printf("sound: %zu\n", data.size());
    if (data.empty())
        return false;

    auto it = m_samples.begin();
    while (it != m_samples.end())
    {
        float cosa = VectorUtil::GetCosA_First(&it->sample[0], it->sample.size(), &data[0], data.size());
        float dtw = m_dtw.Calculate(&it->sample[0], it->sample.size(), &data[0], data.size());
        printf("cosa = %f, dtw = %f\n", cosa, dtw);
        if (dtw < 5) // if (cosa >= 0.88f)
        {
            it->hit++;
            printf("matched!\n");
            return true;
        }
        ++it;
    }
    printf("not matched!\n");
    return false;
}

void FishingSoundListener::LoadSamples()
{
    ifstream file(SAMPLE_FILE, ios::in);
    if (file.is_open())
    {
        char buf[10240] = { 0 };
        int hit = 0;
        while (file.getline(buf, sizeof(buf)))
        {
            if (buf[0] == '\0')
                continue;
            if (buf[0] == '#')
            {
                if (strncmp(buf, CFG_HIT, strlen(CFG_HIT)) == 0)
                {
                    hit = atoi(buf + strlen(CFG_HIT));
                }
                continue;
            }
            AddSample(buf, hit);
        }
        file.close();
    }

    SaveSamples();
}

void FishingSoundListener::SaveSamples()
{
    SortSamples();
    ofstream file(SAMPLE_FILE, ios::trunc | ios::out);
    if (file.is_open())
    {
        auto it = m_samples.begin();
        while (it != m_samples.end())
        {
            //if (it->hit > 0)
            {
                SaveSample(it->sample, it->hit, file);
            }
            ++it;
        }
        file.close();
    }
}

void FishingSoundListener::SaveSample(const std::vector<float>& sample, int hit, std::ofstream& file)
{
    file << "#size = " << sample.size() << endl;
    file << CFG_HIT << hit << endl;
    file.precision(3);
    bool isFirst = true;
    auto it = sample.cbegin();
    while (it != sample.cend())
    {
        if (!isFirst)
            file << ",";
        else
            isFirst = false;
        file << *it++;
    }
    file << endl << endl;
}

void FishingSoundListener::SortSamples()
{
    m_samples.sort([](SampleInfo a, SampleInfo b) {
        return a.hit > b.hit;
    });
}
