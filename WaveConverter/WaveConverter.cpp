// WaveConverter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "WaveUtil/WaveFile.h"
#include "WaveUtil/WaveUtil.h"
#include "WaveUtil/WaveStreamConverter.h"
#include "CommUtil/FTimer.h"
#include <string>
#include <sstream>
#include <memory>

using namespace comm_util;

int ConvertWave(std::string fileName, int sampleRate, int bitsPerSample, int channel);

int main(int argc, char *argv[])
{
	bool paramError = true;
	do
	{
		if (argc < 3 || argc > 5)
			break;

		int sampleRate = 0;
		int bitsPerSample = 0;
		int channel = 0;

		if (argc > 2)
			sampleRate = ::atoi(argv[2]);
		if (argc > 3)
			bitsPerSample = ::atoi(argv[3]);
		if (argc > 4)
			channel = ::atoi(argv[4]);

		ConvertWave(argv[1], sampleRate, bitsPerSample, channel);

		paramError = false;
	} while (false);

	if (paramError)
	{
		printf("Usage: WaveConverter [sampleRate] [bitsPerSample] [channel]\n");
		printf("\n[sampleRate]: e.g. 0, 44100, 48000\n");
		printf("\n[bitsPerSample]: e.g. 0, 16, 24, 32\n");
		printf("\n[channel]: e.g. 0, 1, 2\n");
		printf("\nIf param is 0, then will keep original.\n");
	}
}

int ConvertWave(std::string fileName, int sampleRate, int bitsPerSample, int channel)
{
	std::string inFileName = fileName;
	std::string outFileName;

	printf("process: %s\n", inFileName.c_str());

	FTimer timer;
	timer.Start();

	WaveFile inFile;
	if (!inFile.BeginRead(inFileName.c_str()))
	{
		return -1;
	}

	WAVEFORMATEX fwxIn;
	WaveUtil::ConvertFormat(inFile.GetFormat(), &fwxIn);

	if (sampleRate == 0)
		sampleRate = fwxIn.nSamplesPerSec;
	if (bitsPerSample == 0)
		bitsPerSample = fwxIn.wBitsPerSample;
	if (channel == 0)
		channel = fwxIn.nChannels;

	printf("src fmt: %d, %d, %d\n", fwxIn.nSamplesPerSec, fwxIn.wBitsPerSample, fwxIn.nChannels);
	printf("dst fmt: %d, %d, %d\n", sampleRate, bitsPerSample, channel);

	std::stringstream stream;
	stream << fileName.substr(0, fileName.find_last_of('.'));
	stream << "_" << sampleRate << "_" << bitsPerSample << "_" << channel;
	stream << ".wav";
	outFileName = stream.str();

	WAVEFORMATEX fwxOut = fwxIn;
	WaveFile::FormatChunk fmtOut;
	WaveUtil::SetFormat(&fwxOut, sampleRate, bitsPerSample, channel);
	WaveUtil::ConvertFormat(&fwxOut, &fmtOut);

	WaveStreamConverter converter;
	if (!converter.IsSupport(&fwxIn, &fwxOut))
	{
		printf("Convert format not support.\n");
		return -1;
	}

	WaveFile outFile;
	if (!outFile.SetFormat(fmtOut))
	{
		return -1;
	}
	if (!outFile.BeginWrite(outFileName.c_str(), true))
	{
		return -1;
	}

	uint32_t bufferFrameCount = 1000;
	std::shared_ptr<char> outBuffer(new char[bufferFrameCount * outFile.BytesPerFrame()], std::default_delete<char[]>());

	converter.SetStream(&inFile.InStream());
	converter.SetFormat(&fwxIn, &fwxOut, bufferFrameCount);

	float sampleRateRadio = ((float)outFile.GetFormat()->sampleRate / inFile.GetFormat()->sampleRate);
	uint32_t frameCount = (uint32_t)(inFile.FrameCount() * sampleRateRadio);

	for (uint32_t i = 0; i < frameCount;)
	{
		uint32_t count = min(frameCount - i, bufferFrameCount);
		uint32_t rcount = converter.ReadFrame(outBuffer.get(), count);
		if (rcount > 0)
			outFile.WriteFrame(outBuffer.get(), rcount);
		else
			break;
		i += rcount;
	}

	inFile.EndRead();
	outFile.EndWrite();

	timer.Stop();
	printf(" output: %s\n", outFileName.c_str());
	printf("   time: %f\n", timer.Seconds());

	return 0;
}

