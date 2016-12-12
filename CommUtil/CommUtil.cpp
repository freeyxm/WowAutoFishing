#include "CommUtil.hpp"

namespace comm_util
{

const float CommUtil::FLOAT_EPSILON = 1E-7f;
const double CommUtil::DOUBLE_EPSILON = 1E-15;

bool CommUtil::isEqual(float a, float b)
{
	return isEqualZero(a - b);
}

bool CommUtil::isEqual(double a, double b)
{
	return isEqualZero(a - b);
}

bool CommUtil::isEqualZero(float a)
{
	return -FLOAT_EPSILON < a && a < FLOAT_EPSILON;
}

bool CommUtil::isEqualZero(double a)
{
	return -DOUBLE_EPSILON < a && a < DOUBLE_EPSILON;
}

} // namespace comm_util