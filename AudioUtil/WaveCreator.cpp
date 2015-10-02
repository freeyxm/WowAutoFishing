#include "stdafx.h"
#include "WaveCreator.h"
#include <cstdio>

#define BREAK_ON(cond, msg) \
	if (cond) {\
		if (msg) { printf(msg); }\
		break;\
			}

WaveCreator::WaveCreator()
{
}


WaveCreator::~WaveCreator()
{
}

bool WaveCreator::LoadWave(const char *path, WAVEFORMATEX *pwfx, BYTE **ppData, UINT32 *pDataLen, BYTE **ppExtData)
{
	FILE *fp;
	::fopen_s(&fp, path, "rb");
	if (!fp)
	{
		printf("Can't open %s\n", path);
		return false;
	}

	WaveHead head;
	head.fmt.pExtData = NULL;
	head.data.pData = NULL;

	size_t nread = 0;
	bool bSuccess = false;
	do
	{
		// read head
		nread = ::fread(head.chunkID, sizeof(head.chunkID), 1, fp);
		BREAK_ON(nread != 1, "read chunkID failed!\n");

		if (::strncmp(head.chunkID, "RIFF", 4) != 0)
		{
			printf("chunkID not equals to \"RIFF\".\n");
			break;
		}

		nread = ::fread(&head.chunkSize, sizeof(head.chunkSize), 1, fp);
		BREAK_ON(nread != 1, "read chunkSize failed!\n");

		nread = ::fread(head.format, sizeof(head.format), 1, fp);
		BREAK_ON(nread != 1, "read format failed!\n");

		if (::strncmp(head.format, "WAVE", 4) != 0)
		{
			printf("format not equals to \"WAVE\".\n");
			break;
		}

		// read format header
		nread = ::fread(head.fmt.chunkId, sizeof(head.fmt.chunkId), 1, fp);
		BREAK_ON(nread != 1, "read fmt.chunkId failed!\n");

		if (::strncmp(head.fmt.chunkId, "fmt ", 4) != 0)
		{
			printf("fmt.chunkId not equals to \"fmt \".\n");
			break;
		}

		nread = ::fread(&head.fmt.chunkSize, sizeof(head.fmt.chunkSize), 1, fp);
		BREAK_ON(nread != 1, "read fmt.chunkSize failed!\n");

		// read WAVEFORMATEX
		nread = ::fread(&head.fmt.wfx.wFormatTag, sizeof(head.fmt.wfx.wFormatTag), 1, fp);
		BREAK_ON(nread != 1, "read wFormatTag failed\n");

		nread = ::fread(&head.fmt.wfx.nChannels, sizeof(head.fmt.wfx.nChannels), 1, fp);
		BREAK_ON(nread != 1, "read nChannels failed\n");

		nread = ::fread(&head.fmt.wfx.nSamplesPerSec, sizeof(head.fmt.wfx.nSamplesPerSec), 1, fp);
		BREAK_ON(nread != 1, "read nSamplesPerSec failed\n");

		nread = ::fread(&head.fmt.wfx.nAvgBytesPerSec, sizeof(head.fmt.wfx.nAvgBytesPerSec), 1, fp);
		BREAK_ON(nread != 1, "read nAvgBytesPerSec failed\n");

		nread = ::fread(&head.fmt.wfx.nBlockAlign, sizeof(head.fmt.wfx.nBlockAlign), 1, fp);
		BREAK_ON(nread != 1, "read nBlockAlign failed\n");

		nread = ::fread(&head.fmt.wfx.wBitsPerSample, sizeof(head.fmt.wfx.wBitsPerSample), 1, fp);
		BREAK_ON(nread != 1, "read wBitsPerSample failed\n");

		nread = ::fread(&head.fmt.wfx.cbSize, sizeof(head.fmt.wfx.cbSize), 1, fp);
		BREAK_ON(nread != 1, "read cbSize failed\n");

		if (head.fmt.wfx.cbSize > 0 && ppExtData != NULL)
		{
			head.fmt.pExtData = (BYTE*)malloc(head.fmt.wfx.cbSize);
			BREAK_ON(!head.fmt.pExtData, "malloc failed\n");

			nread = ::fread(head.fmt.pExtData, head.fmt.wfx.cbSize, 1, fp);
			BREAK_ON(nread != head.fmt.wfx.cbSize, "read format extra data failed\n");
		}
		else
		{
			::fseek(fp, head.fmt.wfx.cbSize, SEEK_CUR);
		}

		// read format data
		nread = ::fread(head.data.chunkId, sizeof(head.data.chunkId), 1, fp);
		BREAK_ON(nread != 1, "read data.chunkId failed!\n");

		if (::strncmp(head.data.chunkId, "data", 4) != 0)
		{
			printf("data.chunkId not equals to \"data\".\n");
			break;
		}

		nread = ::fread(&head.data.chunkSize, sizeof(head.data.chunkSize), 1, fp);
		BREAK_ON(nread != 1, "read data.chunkSize failed!\n");

		if (head.data.chunkSize > 0)
		{
			head.data.pData = (BYTE*)malloc(head.data.chunkSize);
			BREAK_ON(!head.data.pData, "malloc failed\n");
			
			nread = ::fread(head.data.pData, 1, head.data.chunkSize, fp);
			BREAK_ON(nread != head.data.chunkSize, "read data failed\n");
		}
		else
		{
			head.data.pData = NULL;
		}

		bSuccess = true;
	} while (false);

	if (bSuccess)
	{
		*pwfx = head.fmt.wfx;
		*ppData = head.data.pData;
		*pDataLen = head.data.chunkSize;
		if (ppExtData != NULL)
		{
			*ppExtData = head.fmt.pExtData;
		}
	}
	else
	{
		if (head.fmt.pExtData)
		{
			::free(head.fmt.pExtData);
			head.fmt.pExtData = NULL;
		}
		if (head.data.pData)
		{
			::free(head.data.pData);
			head.data.pData = NULL;
		}
		*ppData = NULL;
		*pDataLen = 0;
		if (ppExtData != NULL)
		{
			*ppExtData = NULL;
		}
	}

	::fclose(fp);

	return bSuccess;
}
