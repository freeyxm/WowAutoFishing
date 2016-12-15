#include "stdafx.h"
#include "AudioRenderer.h"
#include <process.h>

static UINT __stdcall RenderTheadProc(LPVOID param);

AudioRenderer::AudioRenderer(bool bDefaultDevice)
	: AudioRender(bDefaultDevice)
	, m_bPlaying(false), m_hThreadRenderer(NULL)
{
}


AudioRenderer::~AudioRenderer()
{
}

void AudioRenderer::SetSource(const AudioFrameStorage *pStorage)
{
	m_pStorage = pStorage;
}

bool AudioRenderer::Start()
{
	SetDone(false);

	m_dataIter = m_pStorage->cbegin();
	m_dataIndex = 0;

	m_bPlaying = true;
	m_hThreadRenderer = (HANDLE)::_beginthreadex(NULL, 0, &RenderTheadProc, this, 0, NULL);
	if (m_hThreadRenderer == NULL)
	{
		m_bPlaying = false;
		return false;
	}

	return true;
}

void AudioRenderer::Stop()
{
	SetDone(true);

	if (m_hThreadRenderer != NULL)
	{
		::WaitForSingleObject(m_hThreadRenderer, INFINITE);
		::CloseHandle(m_hThreadRenderer);
		m_hThreadRenderer = NULL;
	}

	StopRender();
}

static UINT __stdcall RenderTheadProc(LPVOID param)
{
	HRESULT hr = S_OK;
	AudioRenderer *pRender = (AudioRenderer*)param;
	if (pRender)
	{
		hr = pRender->Render();
		pRender->SetDone(true);
		pRender->m_bPlaying = false;
	}
	return hr;
}

HRESULT AudioRenderer::SetFormat(WAVEFORMATEX *pwfx)
{
	AudioRender::SetFormat(pwfx);

	return S_OK;
}

HRESULT AudioRenderer::OnLoadData(BYTE *pData, UINT32 *pFrameCount, DWORD *pFlags)
{
	bool isDone = IsDone();
	if (!isDone)
	{
		UINT32 nDataLen = (*pFrameCount) * m_nBytesPerFrame;
		UINT32 nLoaded = 0;
		while (nLoaded < nDataLen && m_dataIter != m_pStorage->cend())
		{
			AudioFrameData *pFrame = *m_dataIter;
			UINT32 curLen = pFrame->nDataLen - m_dataIndex;
			UINT32 minLen = nDataLen < curLen ? nDataLen : curLen;

			::memcpy(pData + nLoaded, pFrame->pData + m_dataIndex, minLen);

			nLoaded += minLen;
			m_dataIndex += minLen;
			if (m_dataIndex >= pFrame->nDataLen)
			{
				++m_dataIter;
				m_dataIndex = 0;
			}
		}
		if (nLoaded < nDataLen)
		{
			isDone = true;
		}
		*pFrameCount = nLoaded / m_nBytesPerFrame;
	}

	if (isDone)
	{
		*pFlags = AUDCLNT_BUFFERFLAGS_SILENT;
	}

	return S_OK;
}

bool AudioRenderer::IsDone() const
{
	return m_bDone && !m_bPlaying;
}
