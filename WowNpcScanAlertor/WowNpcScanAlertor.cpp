// WowNpcScanAlertor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NpcScanAlertorMgr.h"
#include <cstdio>

int _tmain(int argc, _TCHAR* argv[])
{
	::CoInitialize(NULL);

	NpcScanAlertorMgr alertorMgr;
	if (alertorMgr.Init())
	{
		alertorMgr.Run();
	}

	::CoUninitialize();
	
	printf("Press Enter to quit ... ");
	getchar();

	return 0;
}

