// WavePlayer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "WaveUtil/WaveFile.h"
#include "WaveUtil/WaveUtil.h"
#include "Win32Util/Audio/Extend/AudioRenderer.h"

void PlayWave(const char *file);

int main(int argc, char *argv[])
{
    int res = ::CoInitialize(NULL);
    if (res != S_OK)
    {
        return res;
    }

	if (argc > 1)
	{
		PlayWave(argv[1]);
	}

	return 0;
}

void PlayWave(const char *file)
{
	AudioRenderer render;
	if (render.Init() == 0)
	{
		if (render.SetSourceFile(file))
		{
			render.Start();
			while (!render.IsDone())
			{
				Sleep(10);
			}
		}
	}
}



