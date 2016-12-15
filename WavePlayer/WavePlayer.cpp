// WavePlayer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AudioUtil/WaveFile.h"
#include "AudioUtil/WaveUtil.h"
#include "AudioUtil/AudioRenderer.h"

void PlayWave(const char *file);

int main(int argc, char *argv[])
{
	::CoInitialize(NULL);

	if (argc > 1)
	{
		PlayWave(argv[1]);
	}

	return 0;
}

void PlayWave(const char *file)
{
	WaveFile waveFile;
	if (waveFile.BeginRead(file))
	{
		WAVEFORMATEX wfmx;
		WaveUtil::ConvertFormat(waveFile.GetFormat(), &wfmx);

		AudioRenderer render;
		if (render.Init(&wfmx))
		{
			render.SetSource(&waveFile);
			render.Start();
			while (!render.IsDone())
			{
				Sleep(10);
			}
		}
	}
}



