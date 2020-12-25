#include "CommUtil.hpp"

namespace comm_util
{

const float CommUtil::FLOAT_EPSILON = 1E-7f;
const double CommUtil::DOUBLE_EPSILON = 1E-15;

bool CommUtil::IsEqual(float a, float b)
{
	return IsEqualZero(a - b);
}

bool CommUtil::IsEqual(double a, double b)
{
	return IsEqualZero(a - b);
}

bool CommUtil::IsEqualZero(float a)
{
	return -FLOAT_EPSILON < a && a < FLOAT_EPSILON;
}

bool CommUtil::IsEqualZero(double a)
{
	return -DOUBLE_EPSILON < a && a < DOUBLE_EPSILON;
}

} // namespace comm_util