#include "stdafx.h"
#include "AudioRenderer.h"
#include <process.h>

UINT __stdcall RenderTheadProc(LPVOID param);

AudioRenderer::AudioRenderer()
{
}


AudioRenderer::~AudioRenderer()
{
}

void AudioRenderer::SetSource(AudioFrameStorage *pStorage)
{
	m_pStorage = pStorage;
}

bool AudioRenderer::StartRender()
{
	m_bDone = false;

	m_dataIter = m_pStorage->begin();
	m_dataIndex = 0;

	m_hThreadCapture = (HANDLE)::_beginthreadex(NULL, 0, &RenderTheadProc, this, 0, NULL);
	if (m_hThreadCapture == NULL)
		return false;

	return true;
}

bool AudioRenderer::StopRender()
{
	m_bDone = true;
	return true;
}

UINT __stdcall RenderTheadProc(LPVOID param)
{
	CoInitialize(NULL);

	AudioRenderer *pRender = (AudioRenderer*)param;
	pRender->Render();

	CoUninitialize();

	return S_OK;
}

HRESULT AudioRenderer::SetFormat(WAVEFORMATEX *pwfx)
{
	AudioRender::SetFormat(pwfx);

	return S_OK;
}

HRESULT AudioRenderer::OnLoadData(BYTE *pData, UINT32 nFrameCount, DWORD *pFlags)
{
	if (!m_bDone)
	{
		UINT32 nDataLen = nFrameCount * m_nBytesPerFrame;
		UINT32 nLoaded = 0;
		while (nLoaded < nDataLen && m_dataIter != m_pStorage->end())
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
			m_bDone = true;
		}
	}

	if (m_bDone)
	{
		*pFlags = AUDCLNT_BUFFERFLAGS_SILENT;
	}

	return S_OK;
}
