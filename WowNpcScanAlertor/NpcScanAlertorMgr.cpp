#include "stdafx.h"
#include "NpcScanAlertorMgr.h"
#include "Win32Util/Utility.h"
#include "AudioUtil/WaveCreator.h"
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

	int count = Utility::Split(m_cmdLine, m_cmdElems);
	if (count == 0)
		return;

	std::list<string>::const_iterator it = m_cmdElems.cbegin();
	if (strcmp("start", it->c_str()) == 0)
	{
		if (count > 1)
		{
			++it;
			int pid = ::atoi(it->c_str());
			StartAlertor(pid);
		}
	}
	else if (strcmp("stop", it->c_str()) == 0)
	{
		if (count > 1)
		{
			++it;
			int pid = ::atoi(it->c_str());
			StopAlertor(pid);
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
			SetAlertorName(pid, name);
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

void NpcScanAlertorMgr::StartAlertor(int pid)
{
	std::list<NpcScanAlertor*>::iterator it = FindAlertorByPid(pid);
	if (it != m_alertors.end())
	{
		(*it)->Start();
		printf("Alerter[%d] \"%s\" : started.\n", pid, (*it)->GetName());
	}
}

void NpcScanAlertorMgr::StopAlertor(int pid)
{
	std::list<NpcScanAlertor*>::iterator it = FindAlertorByPid(pid);
	if (it != m_alertors.end())
	{
		(*it)->Stop();
		printf("Alerter[%d] \"%s\" : stopped.\n", pid, (*it)->GetName());
	}
}

void NpcScanAlertorMgr::SetAlertorName(int pid, const char* name)
{
	std::list<NpcScanAlertor*>::iterator it = FindAlertorByPid(pid);
	if (it != m_alertors.end())
	{
		(*it)->SetName(name);
		printf("Alerter[%d] set name to \"%s\".\n", pid, name);
	}
}

void NpcScanAlertorMgr::PrintAlertors()
{
	int index = 0;
	for (std::list<NpcScanAlertor*>::iterator it = m_alertors.begin(); it != m_alertors.end(); ++it)
	{
		++index;
		NpcScanAlertor *pAlertor = *it;
		printf("%d. pid: %d, name: \"%s\", status: %d\n", index, pAlertor->GetPid(), pAlertor->GetName(), pAlertor->IsRunning());
	}
}
