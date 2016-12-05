#pragma once
#pragma execution_character_set("utf-8")
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <list>

#define BREAK_ON_ERROR(hres)  \
	if (FAILED(hres)) { break; }
#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }

class AudioUtil
{
public:
	static float ParseValue(const WAVEFORMATEX *pwfx, const void *pData, size_t index, int midValue);

	static int GetMidValue(const WAVEFORMATEX *pwfx);

	static bool IsFloatFormat(const WAVEFORMATEX *pwfx);

	static bool SelectDevice(IMMDeviceEnumerator *pEnumerator, EDataFlow eDataFlow, bool bDefault, IMMDevice **ppDevice);

protected:
	struct DeviceInfo
	{
		IMMDevice *pDevice;
		wchar_t sName[128];
	};

	static UINT GetDevices(EDataFlow eDataFlow, IMMDeviceEnumerator *pEnumerator, std::list<DeviceInfo> &devices);
	static int GetSelectIndex(int min, int max);
};

