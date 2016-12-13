#pragma once
#include <cstdint>

class WaveFile
{
private:
	struct RiffChunk
	{
		char chunkId[4]; // "RIFF"
		uint32_t chunkSize;
		char format[4]; // "WAVE"
	};

	struct FormatChunk
	{
		char chunkId[4]; // "fmt "
		uint32_t chunkSize;
		uint16_t audioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
		uint16_t extParamSize;
		char* pExtParam;
	};

	struct DataChunk
	{
		char chunkId[4]; // "data"
		uint32_t chunkSize;
		char *pData;
	};

public:
	WaveFile();
	~WaveFile();

	bool Load(const char *fileName);
	bool Save(const char *fileName);

private:
	RiffChunk m_riff;
	FormatChunk m_fmt;
	DataChunk m_data;
};

