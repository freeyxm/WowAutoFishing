#include "StringUtil.h"

StringUtil::StringUtil()
{
}


StringUtil::~StringUtil()
{
}

float StringUtil::atof(const char *str)
{
	return (float)::atof(str);
}

