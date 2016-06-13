#pragma once
#include "AudioUtil/AudioExtractor.h"
#include <vector>
#include <list>
#include "aquila/aquila.h"

class Fisher;

class FishingSoundListener :
	public AudioExtractor
{
	typedef std::vector<double> SameData;

	struct SampleInfo
	{
		SameData sample;
		int hit;
	};

public:
	typedef bool (Fisher::*CheckTimeoutProc)(void);
	typedef void (Fisher::*NotifyBiteProc)(void);

public:
	FishingSoundListener(Fisher *pFisher);
	virtual ~FishingSoundListener();

	void SetCheckTimeoutProc(CheckTimeoutProc callback);
	void SetNotifyBiteProc(NotifyBiteProc callback);

	virtual bool Init();

	virtual HRESULT SetFormat(WAVEFORMATEX *pwfx);
	virtual bool IsDone() const;

	void SetAmpL(float ampL);
	void SetAmpH(float ampH);

protected:
	virtual void EndSegment();

	void AddSample(const char *str, int hit = 0);
	void SaveSample(const SameData &sample, int hit, std::ofstream &file);
	void SaveSample(const SameData &sample, int hit, FILE *file);
	void LoadSamples();
	void SaveSamples();
	void SortSamples();
	bool IsSampleMatch(const SameData &data, double &cosa);

private:
	const WAVEFORMATEX *m_pwfx;
	Fisher *m_pFisher;
	FILE *m_pSampleFile;
	CheckTimeoutProc m_procCheckTimeout;
	NotifyBiteProc m_procNotifyBite;

	std::list<SampleInfo> m_samples;
	SameData m_sampleData;
	int m_sampleCount;

	Aquila::Dtw m_dtw;
};

