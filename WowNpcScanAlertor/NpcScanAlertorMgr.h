#pragma once
#include "NpcScanAlertor.h"
#include "AudioUtil/AudioRenderer.h"
#include <list>
#include <string>

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

	std::list<NpcScanAlertor*>::iterator FindAlertorByPid(int pid);
	NpcScanAlertor* FindAlertor(int pid);
	void RemoveAlertor(int pid);
	void StartAlertor(int pid);
	void StopAlertor(int pid);
	void StartAllAlertor();
	void StopAllAlertor(bool printMsg);
	void PrintAlertors();

	bool InitAudioPlayer();

	void BindWindow(HWND hwnd);

	void ReadCmd();

private:
	static NpcScanAlertorMgr *m_pInstance;
	std::list<NpcScanAlertor*> m_alertors;

	bool m_bRunning;
	std::string m_cmdLine;
	std::list<std::string> m_cmdElems;

	AudioFrameStorage m_alarm;
	AudioRenderer m_audioPlayer;
};

