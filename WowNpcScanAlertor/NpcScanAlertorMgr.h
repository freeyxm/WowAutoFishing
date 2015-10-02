#pragma once
#include "NpcScanAlertor.h"
#include "AudioUtil/AudioRenderer.h"
#include <list>

class NpcScanAlertorMgr
{
public:
	NpcScanAlertorMgr();
	~NpcScanAlertorMgr();

	bool Init();
	void Run();

private:
	void RegistAlertor(NpcScanAlertor *pAlertor);
	void ClearAlertor();

	bool InitAudioPlayer();

	void BindWindow(HWND hwnd);

private:
	static NpcScanAlertorMgr *m_pInstance;
	std::list<NpcScanAlertor*> m_alertors;

	AudioFrameStorage m_alarm;
	AudioRenderer m_audioPlayer;
};

