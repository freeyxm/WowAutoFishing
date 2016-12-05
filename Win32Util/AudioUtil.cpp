#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "AudioUtil.h"
#include <functiondiscoverykeys.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <assert.h>

float AudioUtil::ParseValue(const WAVEFORMATEX *pwfx, const void *pData, size_t index, int midValue)
{
	if (pData == nullptr)
		return 0;

	switch (pwfx->wBitsPerSample)
	{
	case 8:
		return (float)*((INT8*)pData + index) / midValue;
		break;
	case 16:
		return (float)*((INT16*)pData + index) / midValue;
		break;
	case 32:
		if (IsFloatFormat(pwfx))
			return (float)*((float*)pData + index);
		else
			return (float)*((INT32*)pData + index) / midValue;
		break;
	default:
		assert(false);
		break;
	}
	return 0;
}

int AudioUtil::GetMidValue(const WAVEFORMATEX *pwfx)
{
	return ((1L << (pwfx->wBitsPerSample - 1)) - 1) >> 1;
}

bool AudioUtil::IsFloatFormat(const WAVEFORMATEX *pwfx)
{
	if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		return true;
	}
	else if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		WAVEFORMATEXTENSIBLE *pfwxt = (WAVEFORMATEXTENSIBLE*)pwfx;
		if (pfwxt->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		{
			return true;
		}
	}
	return false;
}

bool AudioUtil::SelectDevice(IMMDeviceEnumerator *pEnumerator, EDataFlow eDataFlow, bool bDefault, IMMDevice **ppDevice)
{
	HRESULT hr;
	if (bDefault)
	{
		hr = pEnumerator->GetDefaultAudioEndpoint(eDataFlow, eConsole, ppDevice);
		return SUCCEEDED(hr) ? true : false;
	}
	else
	{
		std::list<DeviceInfo> devices;
		int count = GetDevices(eDataFlow, pEnumerator, devices);

		// Print device name list
		printf("---------------------------------------\n");
		printf("Current Available Devices:\n");
		int index = 1;
		for (std::list<DeviceInfo>::const_iterator it = devices.cbegin(); it != devices.cend(); ++it)
		{
			wprintf_s(L"%d. %ls\n", index, it->sName);
			++index;
		}
		printf("---------------------------------------\n");

		// Select device by index
		index = GetSelectIndex(1, count);
		if (index >= 1 && index <= count)
		{
			std::list<DeviceInfo>::const_iterator it = devices.begin();
			for (int i = 1; i < index; ++i)
			{
				++it;
			}
			*ppDevice = it->pDevice;
		}

		// Release unselected devices
		for (std::list<DeviceInfo>::const_iterator it = devices.cbegin(); it != devices.cend(); ++it)
		{
			if (*ppDevice != it->pDevice)
			{
				IMMDevice *pDevice = it->pDevice;
				SAFE_RELEASE(pDevice);
			}
		}
		devices.clear();

		return count > 0 && *ppDevice != NULL;
	}
}

UINT AudioUtil::GetDevices(EDataFlow eDataFlow, IMMDeviceEnumerator *pEnumerator, std::list<DeviceInfo> &devices)
{
	HRESULT hr;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice = NULL;
	IPropertyStore *pProps = NULL;
	//LPWSTR pwszID = NULL;
	UINT count = 0;

	do
	{
		hr = pEnumerator->EnumAudioEndpoints(eDataFlow, DEVICE_STATE_ACTIVE, &pCollection);
		BREAK_ON_ERROR(hr);

		hr = pCollection->GetCount(&count);
		BREAK_ON_ERROR(hr);

		for (UINT i = 0; i < count; ++i)
		{
			DeviceInfo devInfo = { 0 };
			do
			{
				hr = pCollection->Item(i, &pDevice);
				BREAK_ON_ERROR(hr);

				// Get the endpoint ID string.
				//hr = pDevice->GetId(&pwszID);
				//BREAK_ON_ERROR(hr);

				hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
				BREAK_ON_ERROR(hr);

				PROPVARIANT varName;
				::PropVariantInit(&varName);

				hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
				if (SUCCEEDED(hr))
				{
					wsprintf(devInfo.sName, L"%ls", varName.pwszVal);
				}
				else
				{
					wsprintf(devInfo.sName, L"Unknown name");
				}

				::PropVariantClear(&varName);

				devInfo.pDevice = pDevice;
			} while (false);

			//if (pwszID)
			//{
			//	CoTaskMemFree(pwszID);
			//	pwszID = NULL;
			//}
			SAFE_RELEASE(pProps);

			if (devInfo.pDevice != NULL)
			{
				devices.push_back(devInfo);
			}
		}
	} while (false);

	SAFE_RELEASE(pCollection);

	return count;
}

int AudioUtil::GetSelectIndex(int min, int max)
{
	if (min == max)
		return min;

	int selectIndex = 0;
	bool printTip = true;
	std::string line;
	do
	{
		if (printTip)
			printf("Please select a device (index): ");

		if (!getline(std::cin, line))
			break;

		if (line.empty())
		{
			printTip = false;
			continue;
		}

		selectIndex = ::atoi(line.c_str());
		if (selectIndex >= min && selectIndex <= max)
		{
			break;
		}
		else
		{
			printf("Device index between %d and %d, please re-choose.\n", min, max);
			printTip = true;
		}
	} while (true);

	return selectIndex;
}
