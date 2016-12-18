#include "stdafx.h"
#include "WaveFile.h"
#include "CommUtil/CommUtil.hpp"

WaveFile::WaveFile()
{
	memset(&m_info, 0, sizeof(m_info));
}

WaveFile::~WaveFile()
{
	Clear();
	m_inFile.close();
	m_outFile.close();
}

void WaveFile::Clear()
{
	SAFE_DELETE_A(m_info.fmt.pExtParam);
	SAFE_DELETE_A(m_info.data.pData);

	m_info.fmt.extParamSize = 0;
	m_info.fmt.chunkSize = 16; // !!!
	SetDataChunkSize(0);
}

bool WaveFile::Load(const char *fileName)
{
	bool bOk = false;
	do
	{
		if (!BeginRead(fileName))
			break;

		m_info.data.pData = new char[m_info.data.chunkSize];
		if (!m_info.data.pData)
			break;

		if (ReadData(m_info.data.pData, m_info.data.chunkSize) != m_info.data.chunkSize)
			break;

		bOk = true;
	} while (false);

	EndRead();

	return bOk;
}

bool WaveFile::Save(const char *fileName)
{
	bool bOk = false;
	do
	{
		if (!BeginWrite(fileName, false))
			break;

		if (!WriteData(m_info.data.pData, m_info.data.chunkSize))
			break;

		bOk = true;
	} while (false);

	EndWrite();

	return bOk;
}

bool WaveFile::ReadHead(std::fstream &file)
{
	file.seekg(0, std::ios::end);
	long fileLen = (long)file.tellg();
	file.seekg(0, std::ios::beg);

	// riff chunk
	file.read((char*)&m_info.riff, 12);
	if (!file
		|| strncmp(m_info.riff.chunkId, "RIFF", 4) != 0
		|| m_info.riff.chunkSize != fileLen - 8
		|| strncmp(m_info.riff.format, "WAVE", 4) != 0)
	{
		return false;
	}

	// format chunk
	file.read((char*)&m_info.fmt, 24);
	if (!file || strncmp(m_info.fmt.chunkId, "fmt ", 4) != 0)
		return false;

	if (m_info.fmt.chunkSize > 16)
	{
		file.read((char*)&m_info.fmt.extParamSize, 2);
		if (!file || m_info.fmt.chunkSize != m_info.fmt.extParamSize + 16 + 2)
			return false;

		if (m_info.fmt.extParamSize > 0)
		{
			m_info.fmt.pExtParam = new char[m_info.fmt.extParamSize];
			if (!m_info.fmt.pExtParam)
				return false;

			file.read(m_info.fmt.pExtParam, m_info.fmt.extParamSize);
			if (!file)
				return false;
		}
		else
		{
			m_info.fmt.pExtParam = NULL;
		}
	}
	else
	{
		m_info.fmt.extParamSize = 0;
		m_info.fmt.pExtParam = NULL;
	}

	// data chunk
	file.read((char*)&m_info.data, 8);
	if (!file || strncmp(m_info.data.chunkId, "data", 4) != 0)
		return false;

	// many wave file's data chunk size invalid, so here just correct it.
	uint32_t dataSize = m_info.riff.chunkSize - m_info.fmt.chunkSize - 20;
	if (m_info.data.chunkSize > dataSize)
	{
		SetDataChunkSize(dataSize);
	}
	else if (m_info.data.chunkSize < dataSize)
	{
		SetDataChunkSize(m_info.data.chunkSize);
	}

	UpdateFormat();

	return true;
}

bool WaveFile::WriteHead(std::fstream &file)
{
	if (m_info.riff.chunkSize != m_info.fmt.chunkSize + m_info.data.chunkSize + 20)
		return false;

	// riff chunk
	file.write((char*)&m_info.riff, 12);
	if (!file)
		return false;

	// format chunk
	file.write((char*)&m_info.fmt, 24);
	if (!file)
		return false;

	if (m_info.fmt.chunkSize > 16)
	{
		file.write((char*)&m_info.fmt.extParamSize, 2);
		if (!file)
			return false;

		if (m_info.fmt.extParamSize > 0)
		{
			file.write(m_info.fmt.pExtParam, m_info.fmt.extParamSize);
			if (!file)
				return false;
		}
	}

	// data chunk
	file.write((char*)&m_info.data, 8);
	if (!file)
		return false;

	return true;
}

bool WaveFile::BeginRead(const char *fileName)
{
	Clear();

	m_inFile.close();
	m_inFile.open(fileName, std::ios::in | std::ios::binary);
	if (!m_inFile.is_open())
		return false;

	return ReadHead(m_inFile);
}

uint32_t WaveFile::ReadData(char *pData, uint32_t count)
{
	m_inFile.read(pData, count);
	if (m_inFile)
		return count;
	else
		return (uint32_t)m_inFile.gcount();
}

uint32_t WaveFile::ReadFrame(char *pData, uint32_t frameCount)
{
	uint32_t count = frameCount * m_nBytesPerFrame;

	m_inFile.read(pData, count);
	if (m_inFile)
		return frameCount;
	else
		return (uint32_t)(m_inFile.gcount() / m_nBytesPerFrame);
}

void WaveFile::EndRead()
{
	m_inFile.close();
}

bool WaveFile::BeginWrite(const char *fileName, bool append)
{
	m_outFile.close();
	m_outFile.open(fileName, std::ios::out | std::ios::binary);
	if (!m_outFile.is_open())
		return false;

	memcpy_s(m_info.riff.chunkId, 4, "RIFF", 4);
	memcpy_s(m_info.riff.format, 4, "WAVE", 4);
	memcpy_s(m_info.fmt.chunkId, 4, "fmt ", 4);
	memcpy_s(m_info.data.chunkId, 4, "data", 4);

	m_bAppend = append;
	if (m_bAppend)
	{
		SetDataChunkSize(0);
	}

	return WriteHead(m_outFile);
}

bool WaveFile::WriteData(const char *pData, uint32_t count)
{
	m_outFile.write(pData, count);
	if (!m_outFile)
		return false;

	if (m_bAppend)
	{
		m_info.riff.chunkSize += count;
		m_info.data.chunkSize += count;
	}
	return true;
}

bool WaveFile::WriteFrame(const char *pData, uint32_t frameCount)
{
	return WriteData(pData, frameCount * m_nBytesPerFrame);
}

void WaveFile::EndWrite()
{
	if (m_bAppend)
	{
		m_outFile.seekp(4, std::ios::beg);
		m_outFile.write((char*)&m_info.riff.chunkSize, 4);
		m_outFile.seekp(24 + m_info.fmt.chunkSize, std::ios::beg);
		m_outFile.write((char*)&m_info.data.chunkSize, 4);
	}
	m_outFile.close();
}

bool WaveFile::SetFormat(FormatChunk fmt)
{
	if (fmt.chunkSize != 16 && fmt.chunkSize != fmt.extParamSize + 18)
		return false;

	if (fmt.chunkSize > 18 && fmt.pExtParam == NULL)
		return false;

	memcpy_s(&m_info.fmt, sizeof(m_info.fmt), &fmt, sizeof(fmt));
	memcpy_s(m_info.fmt.chunkId, 4, "fmt ", 4);
	fmt.pExtParam = NULL; // !!!

	UpdateFormat();
	return true;
}

const WaveFile::FormatChunk* WaveFile::GetFormat() const
{
	return &m_info.fmt;
}

void WaveFile::UpdateFormat()
{
	m_nBytesPerSample = m_info.fmt.bitsPerSample / 8;
	m_nBytesPerFrame = m_info.fmt.numChannels * m_nBytesPerSample;
}

void WaveFile::SetDataChunkSize(uint32_t size)
{
	m_info.data.chunkSize = size;
	m_info.riff.chunkSize = m_info.fmt.chunkSize + m_info.data.chunkSize + 20;
}

void WaveFile::TakeData(char **ppData, uint32_t *pDataLen)
{
	*ppData = m_info.data.pData;
	*pDataLen = m_info.data.chunkSize;

	m_info.data.pData = NULL;
	SetDataChunkSize(0);
}

void WaveFile::GiveData(char *pData, uint32_t dataLen)
{
	SAFE_DELETE_A(m_info.data.pData);

	m_info.data.pData = pData;
	SetDataChunkSize(dataLen);
}

std::fstream& WaveFile::InStream()
{
	return m_inFile;
}
