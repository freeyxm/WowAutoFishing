#include "stdafx.h"
#include "NpcScanAlertorMgr.h"
#include "Win32Util/Utility.h"
#include "AudioUtil/WaveCreator.h"

NpcScanAlertorMgr::NpcScanAlertorMgr()
{
}

NpcScanAlertorMgr::~NpcScanAlertorMgr()
{
	ClearAlertor();
}

void NpcScanAlertorMgr::RegistAlertor(NpcScanAlertor *pAlertor)
{
	if (pAlertor)
	{
		m_alertors.push_back(pAlertor);
	}
}

void NpcScanAlertorMgr::ClearAlertor()
{
	for (std::list<NpcScanAlertor*>::iterator it = m_alertors.begin(); it != m_alertors.end();)
	{
		delete *it;
		it = m_alertors.erase(it);
	}
}

bool NpcScanAlertorMgr::InitAudioPlayer()
{
	AudioFrameData *pFrame = new AudioFrameData();
	if (!pFrame)
		return false;

	WAVEFORMATEX wfx;
	if (!WaveCreator::LoadWave("./npcscan.wav", &wfx, &pFrame->pData, &pFrame->nDataLen, NULL))
		return false;

	m_alarm.PushBack(pFrame);
	m_audioPlayer.SetSource(&m_alarm);
	m_audioPlayer.SetSourceFormat(&wfx);

	if (!m_audioPlayer.Init())
	{
		Utility::printf_t("Init audio player failed.\n");
		return false;
	}

	return true;
}

bool NpcScanAlertorMgr::Init()
{
	if (!InitAudioPlayer())
		return false;

	HWND preHWnd = NULL;
	do
	{
		preHWnd = ::FindWindowEx(NULL, preHWnd, NULL, L"魔兽世界");
		if (preHWnd != NULL)
		{
			BindWindow(preHWnd);
		}
	} while (preHWnd != NULL);

	Utility::printf_t("Find %d window.\n", m_alertors.size());

	return m_alertors.size() > 0;
}

void NpcScanAlertorMgr::Run()
{
	Utility::printf_t("Start ...\n");

	int delta = 100;
	while (true)
	{
		for (std::list<NpcScanAlertor*>::iterator it = m_alertors.begin(); it != m_alertors.end(); ++it)
		{
			NpcScanAlertor *p = *it;
			if (p && p->IsRunning())
			{
				(*it)->Update(delta); // need to repair!!!
			}
		}
		::Sleep(delta);
	}
}

void NpcScanAlertorMgr::BindWindow(HWND hwnd)
{
	NpcScanAlertor *pAlertor = new NpcScanAlertor(hwnd, &m_audioPlayer);
	if (!pAlertor)
		return;

	if (!pAlertor->Init())
	{
		delete pAlertor;
		return;
	}

	int colorType = (int)NpcScanAlertor::ColorType::Color_Red | (int)NpcScanAlertor::ColorType::Color_Yellow;
	pAlertor->Start(colorType, false);

	RegistAlertor(pAlertor);
}
