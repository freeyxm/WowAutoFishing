#include "stdafx.h"
#include "WaveFile.h"
#include "CommUtil/CommUtil.hpp"
#include <fstream>

WaveFile::WaveFile()
{
	memset(&m_riff, 0, sizeof(m_riff));
	memset(&m_fmt, 0, sizeof(m_fmt));
	memset(&m_data, 0, sizeof(m_data));
}

WaveFile::~WaveFile()
{
	SAFE_DELETE_A(m_fmt.pExtParam);
	SAFE_DELETE_A(m_data.pData);
}

bool WaveFile::Load(const char *fileName)
{
	bool bOk = false;
	std::ifstream file;

	do
	{
		file.open(fileName, std::ios::in | std::ios::binary);
		if (!file.is_open())
			break;

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
			break;
		}

		// format chunk
		file.read((char*)&m_fmt, 24);
		if (!file || strncmp(m_fmt.chunkId, "fmt ", 4) != 0)
			break;

		if (m_fmt.chunkSize > 16)
		{
			file.read((char*)&m_fmt.extParamSize, 2);
			if (!file || m_fmt.chunkSize != m_fmt.extParamSize + 16 + 2)
				break;

			if (m_fmt.extParamSize > 0)
			{
				m_fmt.pExtParam = new char[m_fmt.extParamSize];
				file.read(m_fmt.pExtParam, m_fmt.extParamSize);
				if (!file)
					break;
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
			break;
		}

		m_data.pData = new char[m_data.chunkSize];
		file.read(m_data.pData, m_data.chunkSize);
		if (!file)
			break;

		bOk = true;
	} while (false);

	file.close();
	return bOk;
}

bool WaveFile::Save(const char *fileName)
{
	bool bOk = false;
	std::ofstream file;

	do
	{
		if (m_riff.chunkSize != m_fmt.chunkSize + m_data.chunkSize + 20)
			break;

		file.open(fileName, std::ios::out | std::ios::binary);
		if (!file.is_open())
			break;

		// riff chunk
		file.write((char*)&m_riff, 12);
		if (!file)
			break;

		// format chunk
		file.write((char*)&m_fmt, 24);
		if (!file)
			break;

		if (m_fmt.chunkSize > 16)
		{
			file.write((char*)&m_fmt.extParamSize, 2);
			if (!file)
				break;

			if (m_fmt.extParamSize > 0)
			{
				file.write(m_fmt.pExtParam, m_fmt.extParamSize);
				if (!file)
					break;
			}
		}

		// data chunk
		file.write((char*)&m_data, 8);
		if (!file)
			break;

		file.write(m_data.pData, m_data.chunkSize);
		if (!file)
			break;

		bOk = true;
	} while (false);

	file.close();
	return bOk;
}
