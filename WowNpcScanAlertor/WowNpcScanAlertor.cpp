// WowNpcScanAlertor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NpcScanAlertorMgr.h"
#include <cstdio>

int _tmain(int argc, _TCHAR* argv[])
{
    int ret = ::CoInitialize(NULL);
    if (ret != 0)
    {
        return ret;
    }

	NpcScanAlertorMgr alertorMgr;
	if (alertorMgr.Init())
	{
		alertorMgr.Run();
	}

	::CoUninitialize();
	
	printf("Press Enter to quit ... ");
	int c = getchar();

	return 0;
}

