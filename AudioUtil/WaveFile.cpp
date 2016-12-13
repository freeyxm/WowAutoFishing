#include "stdafx.h"
#include "WaveFile.h"
#include "CommUtil/CommUtil.hpp"

WaveFile::WaveFile()
{
	memset(&m_riff, 0, sizeof(m_riff));
	memset(&m_fmt, 0, sizeof(m_fmt));
	memset(&m_data, 0, sizeof(m_data));
}

WaveFile::~WaveFile()
{
	Clear();
}

void WaveFile::Clear()
{
	SAFE_DELETE_A(m_fmt.pExtParam);
	SAFE_DELETE_A(m_data.pData);
}

bool WaveFile::Load(const char *fileName)
{
	bool bOk = false;
	do
	{
		if (!BeginRead(fileName))
			break;

		m_data.pData = new char[m_data.chunkSize];
		if (!m_data.pData)
			break;

		if (ReadData(m_data.pData, m_data.chunkSize) != m_data.chunkSize)
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

		if (!WriteData(m_data.pData, m_data.chunkSize))
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
	file.read((char*)&m_riff, 12);
	if (!file
		|| strncmp(m_riff.chunkId, "RIFF", 4) != 0
		|| m_riff.chunkSize != fileLen - 8
		|| strncmp(m_riff.format, "WAVE", 4) != 0)
	{
		return false;
	}

	// format chunk
	file.read((char*)&m_fmt, 24);
	if (!file || strncmp(m_fmt.chunkId, "fmt ", 4) != 0)
		return false;

	if (m_fmt.chunkSize > 16)
	{
		file.read((char*)&m_fmt.extParamSize, 2);
		if (!file || m_fmt.chunkSize != m_fmt.extParamSize + 16 + 2)
			return false;

		if (m_fmt.extParamSize > 0)
		{
			m_fmt.pExtParam = new char[m_fmt.extParamSize];
			if (!m_fmt.pExtParam)
				return false;

			file.read(m_fmt.pExtParam, m_fmt.extParamSize);
			if (!file)
				return false;
		}
		else
		{
			m_fmt.pExtParam = NULL;
		}
	}
	else
	{
		m_fmt.extParamSize = 0;
		m_fmt.pExtParam = NULL;
	}

	// data chunk
	file.read((char*)&m_data, 8);
	if (!file
		|| strncmp(m_data.chunkId, "data", 4) != 0
		|| m_data.chunkSize != m_riff.chunkSize - m_fmt.chunkSize - 20)
	{
		return false;
	}

	return true;
}

bool WaveFile::WriteHead(std::fstream &file)
{
	if (m_riff.chunkSize != m_fmt.chunkSize + m_data.chunkSize + 20)
		return false;

	// riff chunk
	file.write((char*)&m_riff, 12);
	if (!file)
		return false;

	// format chunk
	file.write((char*)&m_fmt, 24);
	if (!file)
		return false;

	if (m_fmt.chunkSize > 16)
	{
		file.write((char*)&m_fmt.extParamSize, 2);
		if (!file)
			return false;

		if (m_fmt.extParamSize > 0)
		{
			file.write(m_fmt.pExtParam, m_fmt.extParamSize);
			if (!file)
				return false;
		}
	}

	// data chunk
	file.write((char*)&m_data, 8);
	if (!file)
		return false;

	return true;
}

bool WaveFile::BeginRead(const char *fileName)
{
	Clear();

	m_inFile.open(fileName, std::ios::in | std::ios::binary);
	if (!m_inFile.is_open())
		return false;

	return ReadHead(m_inFile);
}

int  WaveFile::ReadData(char *pData, uint32_t count)
{
	m_inFile.read(pData, count);
	if (m_inFile)
		return count;
	else
		return m_inFile.gcount();
}

void WaveFile::EndRead()
{
	m_inFile.close();
}

bool WaveFile::BeginWrite(const char *fileName, bool append)
{
	m_outFile.open(fileName, std::ios::out | std::ios::binary);
	if (!m_outFile.is_open())
		return false;

	m_bAppend = append;
	if (m_bAppend)
	{
		m_data.chunkSize = 0;
		m_riff.chunkSize = m_fmt.chunkSize + m_data.chunkSize + 20;
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
		m_riff.chunkSize += count;
		m_data.chunkSize += count;

		m_outFile.seekp(4, std::ios::beg);
		m_outFile.write((char*)&m_riff.chunkSize, 4);

		m_outFile.seekp(8 + 8 + m_fmt.chunkSize + 4);
		m_outFile.write((char*)&m_data.chunkSize, 4);
	}
	return true;
}

void WaveFile::EndWrite()
{
	m_outFile.close();
}
