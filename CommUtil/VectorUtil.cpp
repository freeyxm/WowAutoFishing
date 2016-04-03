#include "VectorUtil.h"
#include <cmath>
#include <cfloat>

VectorUtil::VectorUtil()
{
}


VectorUtil::~VectorUtil()
{
}

/*
Calculate the cosine of two n-vector's angle.
*/
float VectorUtil::getCosA(const float *v1, const float *v2, const int length)
{
	float dot = 0;
	float sum1 = 0;
	float sum2 = 0;
	for (int i = 0; i < length; ++i)
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
float VectorUtil::getCosA(const float *v1, int len1, const float *v2, int len2)
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
	int count = len1 - len2;
	for (int i = 0; i <= count; ++i)
	{
		float value = getCosA(p1 + i, p2, len2);
		if (max_value < value)
		{
			max_value = value;
		}
	}

	return max_value;
}

/*
计算向量夹角的余弦值（最优）。向量长度取大值，并以较长向量的剩余部分填充较短向量。
*/
float VectorUtil::getCosA_Pad(const float *v1, int len1, const float *v2, int len2)
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
	int count = len1 - len2;
	for (int k = 0; k <= count; ++k)
	{
		float dot = 0;
		float sum1 = 0;
		float sum2 = 0;

		for (int i = 0; i < k; ++i)
		{
			float tmp = p1[i] * p1[i];
			dot += tmp;
			sum1 += tmp;
			sum2 += tmp;
		}
		for (int i = 0; i < len2; ++i)
		{
			float _v1 = p1[i + k];
			dot += _v1 * p2[i];
			sum1 += _v1 * _v1;
			sum2 += p2[i] * p2[i];
		}
		for (int i = len2 + k; i < len1; ++i)
		{
			float tmp = p1[i] * p1[i];
			dot += tmp;
			sum1 += tmp;
			sum2 += tmp;
		}

		float value = ::sqrtf((dot* dot) / (sum1 * sum2));
		if (max_value < value)
		{
			max_value = value;
		}
	}

	return max_value;
}
