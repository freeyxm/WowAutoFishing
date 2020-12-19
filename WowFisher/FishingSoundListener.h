#pragma once
#include "Win32Util/Audio/Extend/AudioExtractor.h"
#include <vector>
#include <list>
#include <fstream>

class Fisher;

class FishingSoundListener :
	public AudioExtractor
{
	struct SampleInfo
	{
		std::vector<float> sample;
		int hit;
	};

public:
	FishingSoundListener(Fisher *pFisher);
	virtual ~FishingSoundListener();

	virtual int Init();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual bool IsDone() const;

	void SetAmpL(float ampL);
	void SetAmpH(float ampH);

	void Save();

protected:
	virtual void EndSegment();

	void AddSample(const char *str, int hit = 0);
	void SaveSample(const std::vector<float> &sample, int hit, std::ofstream &file);
	void LoadSamples();
	void SaveSamples();
	void SortSamples();
	bool IsSampleMatch(const std::vector<float> &data);
	bool IsSampleMatchDtw(const std::vector<float> &data);

private:
	const WAVEFORMATEX *m_pwfx;
	Fisher *m_pFisher;
	std::ofstream m_sampleFile;

	std::list<SampleInfo> m_samples;
	int m_sampleCount;
};

