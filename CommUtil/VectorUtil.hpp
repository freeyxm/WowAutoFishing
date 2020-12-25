#pragma once
#include "CommUtil.hpp"
#include <vector>
#include <cmath>
#include <cfloat>
#include <vector>
#include <memory>

using std::vector;

namespace comm_util
{

class VectorUtil
{
public:
	template <typename Numeric>
	static Numeric GetCosA(const Numeric *v1, const Numeric *v2, const size_t length);
	template <typename Numeric>
	static Numeric GetCosA_Short(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2);
	template <typename Numeric>
	static Numeric GetCosA_Long(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2);
	template <typename Numeric>
	static Numeric GetCosA_First(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2);

	template <typename Numeric>
	static Numeric GetLengthSqr(const Numeric *vec, const size_t length);

	template <typename Numeric>
	static Numeric GetAvg(const Numeric *vec, const size_t length);

	template <typename Numeric>
	static void Add(Numeric *v1, const size_t length, Numeric num);
	template <typename Numeric>
	static void Add(Numeric *v1, const Numeric *v2, const size_t length);
	template <typename Numeric>
	static void Sub(Numeric *v1, const Numeric *v2, const size_t length);
};


/*
Calculate vectorial angle cosine.
*/
template <typename Numeric>
Numeric VectorUtil::GetCosA(const Numeric *v1, const Numeric *v2, const size_t length)
{
	Numeric dot = 0, sum1 = 0, sum2 = 0;
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
template <typename Numeric>
Numeric VectorUtil::GetCosA_Short(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	if (len1 == len2)
	{
		return GetCosA(v1, v2, len1);
	}

	const Numeric *p1, *p2;
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

	Numeric max_value = FLT_MIN;
	size_t count = len1 - len2;
	for (size_t i = 0; i <= count; ++i)
	{
		Numeric value = GetCosA(p1 + i, p2, len2);
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
template <typename Numeric>
Numeric VectorUtil::GetCosA_Long(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	if (len1 == len2)
	{
		return GetCosA(v1, v2, len1);
	}

	const Numeric *p1, *p2;
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

	Numeric max_value = FLT_MIN;
	size_t count = len1 - len2;
	for (size_t k = 0; k <= count; ++k)
	{
		Numeric dot, sum1, sum2;
		Numeric tmp = 0;
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
		Numeric value = ::sqrtf((dot* dot) / (sum1 * sum2));
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
template <typename Numeric>
Numeric VectorUtil::GetCosA_First(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	if (len1 == len2)
		return GetCosA(v1, v2, len1);
	else if (len1 < len2)
		return GetCosA_Short(v2, len2, v1, len1);
	else
		return GetCosA_Long(v1, len1, v2, len2);
}

template <typename Numeric>
Numeric VectorUtil::GetLengthSqr(const Numeric *vec, const size_t length)
{
	double sum = 0;
	for (size_t i = 0; i < length; ++i)
	{
		sum += vec[i] * vec[i];
	}
	return sum;
}

template <typename Numeric>
Numeric VectorUtil::GetAvg(const Numeric *vec, const size_t length)
{
	Numeric avg = 0;
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

template <typename Numeric>
void VectorUtil::Add(Numeric *v1, const size_t length, Numeric num)
{
	for (size_t i = 0; i < length; ++i)
	{
		v1[i] += num;
	}
}

template <typename Numeric>
void VectorUtil::Add(Numeric *v1, const Numeric *v2, const size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		v1[i] += v2[i];
	}
}

template <typename Numeric>
void VectorUtil::Sub(Numeric *v1, const Numeric *v2, const size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		v1[i] -= v2[i];
	}
}

} //namespace comm_util
