#include "StringUtil.h"

namespace comm_util
{

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

} // namespace comm_util