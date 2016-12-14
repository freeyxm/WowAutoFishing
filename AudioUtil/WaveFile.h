#pragma once
#include <cstdint>
#include <fstream>

class WaveFile
{
public:
#pragma pack(push)
#pragma pack(2)
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

	struct WaveInfo
	{
		RiffChunk riff;
		FormatChunk fmt;
		DataChunk data;
	};
#pragma pack(pop)

public:
	WaveFile();
	~WaveFile();

	void Clear();

	bool Load(const char *fileName);
	bool Save(const char *fileName);

	bool BeginWrite(const char *fileName, bool append);
	bool WriteData(const char *pData, uint32_t count);
	void EndWrite();

	bool BeginRead(const char *fileName);
	int  ReadData(char *pData, uint32_t count);
	void EndRead();

	uint32_t GetDataChunkSize();

	bool SetFormat(FormatChunk fmt);

	void TakeData(char **ppData, uint32_t *pDataLen);
	void GiveData(char *pData, uint32_t dataLen);

protected:
	bool ReadHead(std::fstream &file);
	bool WriteHead(std::fstream &file);

	void SetDataChunkSize(uint32_t size);

protected:
	WaveInfo m_info;
	std::fstream m_inFile;
	std::fstream m_outFile;
	bool m_bAppend;
};

