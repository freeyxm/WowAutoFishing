#pragma once
#include "WaveConverter.h"
#include <istream>

class WaveStreamConverter : public WaveConverter
{
public:
	WaveStreamConverter(std::istream *stream);
	~WaveStreamConverter();

	void SetStream(std::istream *stream);

protected:
	virtual uint32_t LoadSrcFrame(char *pData, uint32_t frameCount);

private:
	std::istream *m_stream;
};

