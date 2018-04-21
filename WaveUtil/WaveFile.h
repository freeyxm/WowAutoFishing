#pragma once
#include <cstdint>
#include <fstream>

class WaveFile
{
public:
#pragma pack(push)
#pragma pack(2)
	const static uint32_t RIFF_CHUNK_ID; // "RIFF" big-endian
	const static uint32_t RIFF_CHUNK_FMT; // "WAVE" big-endian
	const static uint32_t FORMAT_CHUNK_ID; // "fmt " big-endian
	const static uint32_t DATA_CHUNK_ID; // "data" big-endian

	struct RiffChunk
	{
		uint32_t chunkId; // "RIFF"
		uint32_t chunkSize;
		uint32_t format; // "WAVE"
	};

	struct FormatChunk
	{
		uint32_t chunkId; // "fmt "
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
		uint32_t chunkId; // "data"
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
	bool WriteFrame(const char *pData, uint32_t frameCount);
	void EndWrite();

	bool BeginRead(const char *fileName);
	uint32_t ReadData(char *pData, uint32_t count);
	uint32_t ReadFrame(char *pData, uint32_t frameCount);
	void EndRead();

	inline uint32_t FrameCount() const;
	inline uint32_t BytesPerSample() const;
	inline uint32_t BytesPerFrame() const;

	bool SetFormat(const FormatChunk &fmt);
	const FormatChunk* GetFormat() const;

	void TakeData(char **ppData, uint32_t *pDataLen);
	void GiveData(char *pData, uint32_t dataLen);

	std::fstream& InStream();

protected:
	bool ReadHead(std::fstream &file);
	bool WriteHead(std::fstream &file);

	void UpdateFormat();
	void SetDataChunkSize(uint32_t size);

protected:
	WaveInfo m_info;

	std::fstream m_inFile;
	std::fstream m_outFile;
	bool m_bAppend;

	uint32_t m_nBytesPerSample;
	uint32_t m_nBytesPerFrame;
};


inline uint32_t WaveFile::BytesPerSample() const
{
	return m_nBytesPerSample;
}

inline uint32_t WaveFile::BytesPerFrame() const
{
	return m_nBytesPerFrame;
}

inline uint32_t WaveFile::FrameCount() const
{
	return m_info.data.chunkSize / BytesPerFrame();
}
