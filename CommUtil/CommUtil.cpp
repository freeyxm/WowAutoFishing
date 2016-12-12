#include "CommUtil.hpp"

namespace comm_util
{

const float CommUtil::EPSINON = 1E-7f;

bool CommUtil::isEqual(float a, float b)
{
	return isEqualZero(a - b);
}

bool CommUtil::isEqualZero(float a)
{
	return -EPSINON < a && a < EPSINON;
}

} // namespace comm_util