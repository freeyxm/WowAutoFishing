#include "VectorUtil.h"
#include "CommUtil.h"
#include <cmath>
#include <cfloat>

namespace comm_util
{

VectorUtil::VectorUtil()
{
}


VectorUtil::~VectorUtil()
{
}

/*
Calculate the cosine of two n-vector's angle.
*/
float VectorUtil::getCosA(const float *v1, const float *v2, const size_t length)
{
	float dot = 0, sum1 = 0, sum2 = 0;
	for (size_t i = 0; i < length; ++i)
	{
		dot += v1[i] * v2[i];
		sum1 += v1[i] * v1[i];
		sum2 += v2[i] * v2[i];
	}
	return ::sqrtf((dot* dot) / (sum1 * sum2));
}

/*
计算向量夹角的余弦值（最优）。向量长度取小值。
*/
float VectorUtil::getCosA(const float *v1, size_t len1, const float *v2, size_t len2)
{
	const float *p1, *p2;
	if (len1 >= len2)
	{
		p1 = v1;
		p2 = v2;
	}
	else
	{
		p1 = v2;
		p2 = v1;
		swap(len1, len2);
	}

	float max_value = FLT_MIN;
	size_t count = len1 - len2;
	for (size_t i = 0; i <= count; ++i)
	{
		float value = getCosA(p1 + i, p2, len2);
		if (max_value < value) // cos is in [-1, 1].
		{
			max_value = value;
		}
	}

	return max_value;
}

/*
计算向量夹角的余弦值（最优）。向量长度取大值，并以较长向量的剩余部分填充较短向量。
*/
float VectorUtil::getCosA_Pad(const float *v1, size_t len1, const float *v2, size_t len2)
{
	const float *p1, *p2;
	if (len1 >= len2)
	{
		p1 = v1;
		p2 = v2;
	}
	else
	{
		p1 = v2;
		p2 = v1;
		swap(len1, len2);
	}

	float max_value = FLT_MIN;
	size_t count = len1 - len2;
	for (size_t k = 0; k <= count; ++k)
	{
		float dot, sum1, sum2;
		float tmp = 0;
		// left padding
		for (size_t i = 0; i < k; ++i)
		{
			tmp += p1[i] * p1[i];
		}
		dot = sum1 = sum2 = tmp;
		// middle
		for (size_t i = 0; i < len2; ++i)
		{
			tmp = p1[i + k];
			dot += tmp * p2[i];
			sum1 += tmp * tmp;
			sum2 += p2[i] * p2[i];
		}
		// right padding
		tmp = 0;
		for (size_t i = len2 + k; i < len1; ++i)
		{
			tmp += p1[i] * p1[i];
		}
		dot += tmp;
		sum1 += tmp;
		sum2 += tmp;
		// final calculate
		float value = ::sqrtf((dot* dot) / (sum1 * sum2));
		if (max_value < value) // cos is in [-1, 1].
		{
			max_value = value;
		}
	}

	return max_value;
}

float VectorUtil::getAvg(const float *data, const size_t length)
{
	double avg = 0;
	for (size_t i = 0; i < length; ++i)
	{
		avg += data[i];
	}
	if (length > 0)
	{
		avg /= length;
	}
	return avg;
}

} // namespace comm_util
