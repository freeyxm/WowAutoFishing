#include "stdafx.h"
#include "NpcScanAlertorMgr.h"
#include "Win32Util/Utility.h"
#include "AudioUtil/WaveUtil.h"
#include <conio.h>
#include <iostream>
#include <string>

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
	if (!WaveUtil::LoadWave("./npcscan.wav", &wfx, (char**)&pFrame->pData, &pFrame->nDataLen))
		return false;

	m_alarm.PushBack(pFrame);
	m_audioPlayer.SetSource(&m_alarm);

	if (!m_audioPlayer.Init(&wfx))
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

	StopAllAlertor(false);

	return m_alertors.size() > 0;
}

void NpcScanAlertorMgr::Run()
{
	Utility::printf_t("Ready ...\n");

	m_bRunning = true;

	int delta = 100;
	while (m_bRunning)
	{
		if (_kbhit())
		{
			ReadCmd();
		}
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

void NpcScanAlertorMgr::ReadCmd()
{
	m_cmdLine.clear();
	if (!getline(std::cin, m_cmdLine))
		return;

	size_t count = Utility::Split(m_cmdLine, m_cmdElems);
	if (count == 0)
		return;

	std::list<string>::const_iterator it = m_cmdElems.cbegin();
	if (strcmp("start", it->c_str()) == 0)
	{
		if (count > 1)
		{
			++it;
			if (strcmp("all", it->c_str()) == 0)
			{
				StartAllAlertor();
			}
			else
			{
				int pid = ::atoi(it->c_str());
				StartAlertor(pid);
			}
		}
	}
	else if (strcmp("stop", it->c_str()) == 0)
	{
		if (count > 1)
		{
			++it;
			if (strcmp("all", it->c_str()) == 0)
			{
				StopAllAlertor(true);
			}
			else
			{
				int pid = ::atoi(it->c_str());
				StopAlertor(pid);
			}
		}
	}
	else if (strcmp("remove", it->c_str()) == 0)
	{
		if (count > 1)
		{
			++it;
			int pid = ::atoi(it->c_str());
			RemoveAlertor(pid);
		}
	}
	else if (strcmp("setname", it->c_str()) == 0)
	{
		if (count > 2)
		{
			++it;
			int pid = ::atoi(it->c_str());
			++it;
			const char* name = it->c_str();

			NpcScanAlertor* pAlertor = FindAlertor(pid);
			if (pAlertor != NULL)
			{
				pAlertor->SetName(name);
				printf("Alerter[%d] set name to \"%s\".\n", pid, name);
			}
		}
	}
	else if (strcmp("setrare", it->c_str()) == 0)
	{
		if (count > 2)
		{
			++it;
			int pid = ::atoi(it->c_str());
			++it;
			int rare = ::atoi(it->c_str());

			NpcScanAlertor* pAlertor = FindAlertor(pid);
			if (pAlertor != NULL)
			{
				pAlertor->SetRare(rare ? true : false);
				printf("Alerter[%d] set rare to %d.\n", pid, rare);
			}
		}
	}
	else if (strcmp("setcolor", it->c_str()) == 0)
	{
		if (count > 2)
		{
			++it;
			int pid = ::atoi(it->c_str());
			++it;
			int color = ::atoi(it->c_str());

			NpcScanAlertor* pAlertor = FindAlertor(pid);
			if (pAlertor != NULL)
			{
				pAlertor->SetColorType(color);
				printf("Alerter[%d] set color type to %d.\n", pid, color);
			}
		}
	}
	else if (strcmp("list", it->c_str()) == 0)
	{
		PrintAlertors();
	}
	else if (strcmp("exit", it->c_str()) == 0)
	{
		m_bRunning = false;
	}
	else
	{
		printf("Unexpected CMD: %s\n", m_cmdLine.c_str());
	}
}

void NpcScanAlertorMgr::RemoveAlertor(int pid)
{
	std::list<NpcScanAlertor*>::iterator it = FindAlertorByPid(pid);
	if (it != m_alertors.end())
	{
		NpcScanAlertor *pAlertor = *it;
		printf("Alerter[%d] \"%s\" : removed.\n", pid, (*it)->GetName());
		delete pAlertor;
		m_alertors.erase(it);
	}
}

std::list<NpcScanAlertor*>::iterator NpcScanAlertorMgr::FindAlertorByPid(int pid)
{
	std::list<NpcScanAlertor*>::iterator it = m_alertors.begin();
	while (it != m_alertors.end())
	{
		if ((*it)->GetPid() == pid)
			break;
		++it;
	}
	if (it == m_alertors.end())
	{
		printf("Can't find alerter [%d].\n", pid);
	}
	return it;
}

NpcScanAlertor* NpcScanAlertorMgr::FindAlertor(int pid)
{
	std::list<NpcScanAlertor*>::iterator it = FindAlertorByPid(pid);
	if (it != m_alertors.end())
		return *it;
	else
		return NULL;
}

void NpcScanAlertorMgr::StartAlertor(int pid)
{
	NpcScanAlertor* pAlertor = FindAlertor(pid);
	if (pAlertor != NULL)
	{
		pAlertor->Start();
		printf("Alerter[%d] \"%s\" : started.\n", pid, pAlertor->GetName());
	}
}

void NpcScanAlertorMgr::StopAlertor(int pid)
{
	NpcScanAlertor* pAlertor = FindAlertor(pid);
	if (pAlertor != NULL)
	{
		pAlertor->Stop();
		printf("Alerter[%d] \"%s\" : stopped.\n", pid, pAlertor->GetName());
	}
}

void NpcScanAlertorMgr::StartAllAlertor()
{
	for (std::list<NpcScanAlertor*>::iterator it = m_alertors.begin(); it != m_alertors.end(); ++it)
	{
		(*it)->Start();
		printf("Alerter[%d] \"%s\" : started.\n", (*it)->GetPid(), (*it)->GetName());
	}
}

void NpcScanAlertorMgr::StopAllAlertor(bool printMsg)
{
	for (std::list<NpcScanAlertor*>::iterator it = m_alertors.begin(); it != m_alertors.end(); ++it)
	{
		(*it)->Stop();
		if (printMsg)
		{
			printf("Alerter[%d] \"%s\" : stopped.\n", (*it)->GetPid(), (*it)->GetName());
		}
	}
}

void NpcScanAlertorMgr::PrintAlertors()
{
	int index = 0;
	for (std::list<NpcScanAlertor*>::iterator it = m_alertors.begin(); it != m_alertors.end(); ++it)
	{
		++index;
		NpcScanAlertor *pAlertor = *it;
		printf("%d. pid: %d, name: \"%s\", rare: %d, color: %d, status: %d\n", index
			, pAlertor->GetPid(), pAlertor->GetName(), pAlertor->IsRare(), pAlertor->GetColorType(), pAlertor->IsRunning());
	}
}
