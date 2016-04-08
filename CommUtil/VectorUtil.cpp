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
Calculate vectorial angle cosine.
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
Calculate vectorial angle cosine. (optimal)
Length by the shorter one.
*/
float VectorUtil::getCosA_Short(const float *v1, size_t len1, const float *v2, size_t len2)
{
	if (len1 == len2)
	{
		return getCosA(v1, v2, len1);
	}

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
Calculate vectorial angle cosine. (optimal)
Length by the longer one, and padding the shorter with longer's spare part.
*/
float VectorUtil::getCosA_Long(const float *v1, size_t len1, const float *v2, size_t len2)
{
	if (len1 == len2)
	{
		return getCosA(v1, v2, len1);
	}

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

/*
Calculate vectorial angle cosine. (optimal)
Length by the first one.
*/
float VectorUtil::getCosA_First(const float *v1, size_t len1, const float *v2, size_t len2)
{
	if (len1 == len2)
		return getCosA(v1, v2, len1);
	else if (len1 < len2)
		return getCosA_Short(v2, len2, v1, len1);
	else
		return getCosA_Long(v1, len1, v2, len2);
}

float VectorUtil::getLengthSqr(const float *vec, const size_t length)
{
	double sum = 0;
	for (size_t i = 0; i < length; ++i)
	{
		sum += vec[i] * vec[i];
	}
	return sum;
}

float VectorUtil::getAvg(const float *vec, const size_t length)
{
	double avg = 0;
	for (size_t i = 0; i < length; ++i)
	{
		avg += vec[i];
	}
	if (length > 0)
	{
		avg /= length;
	}
	return avg;
}

void VectorUtil::Add(float *v1, const size_t length, float num)
{
	for (size_t i = 0; i < length; ++i)
	{
		v1[i] += num;
	}
}

void VectorUtil::Add(float *v1, const float *v2, const size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		v1[i] += v2[i];
	}
}

void VectorUtil::Sub(float *v1, const float *v2, const size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		v1[i] -= v2[i];
	}
}

} // namespace comm_util
