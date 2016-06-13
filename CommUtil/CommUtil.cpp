#include "CommUtil.hpp"

namespace comm_util
{

const float CommUtil::EPSINON = 1e-6f;

bool CommUtil::isEqual(float a, float b)
{
	return isEqualZero(a - b);
}

bool CommUtil::isEqualZero(float a)
{
	return a < EPSINON && a > -EPSINON;
}

} // namespace comm_util