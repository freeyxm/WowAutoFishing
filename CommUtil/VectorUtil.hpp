#pragma once
#include <vector>
#include "CommUtil.hpp"
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
	static Numeric getCosA(const Numeric *v1, const Numeric *v2, const size_t length);
	template <typename Numeric>
	static Numeric getCosA_Short(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2);
	template <typename Numeric>
	static Numeric getCosA_Long(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2);
	template <typename Numeric>
	static Numeric getCosA_First(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2);

	template <typename Numeric>
	static Numeric getCosA_MinSub(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2);

	template <typename Numeric>
	static Numeric getLengthSqr(const Numeric *vec, const size_t length);

	template <typename Numeric>
	static Numeric getAvg(const Numeric *vec, const size_t length);

	template <typename Numeric>
	static Numeric getDTW(const Numeric *v1, const size_t len1, const Numeric *v2, const size_t len2);

	template <typename Numeric>
	static void Add(Numeric *v1, const size_t length, Numeric num);
	template <typename Numeric>
	static void Add(Numeric *v1, const Numeric *v2, const size_t length);
	template <typename Numeric>
	static void Sub(Numeric *v1, const Numeric *v2, const size_t length);
	template <typename Numeric>
	static void Sub(Numeric *v1, size_t len1, const Numeric *v2, size_t len2);

	template <typename Numeric>
	static vector<int> getRiseAndFall(Numeric *v1, const size_t length);
};


/*
Calculate vectorial angle cosine.
*/
template <typename Numeric>
Numeric VectorUtil::getCosA(const Numeric *v1, const Numeric *v2, const size_t length)
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
Numeric VectorUtil::getCosA_Short(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	if (len1 == len2)
	{
		return getCosA(v1, v2, len1);
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
		Numeric value = getCosA(p1 + i, p2, len2);
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
Numeric VectorUtil::getCosA_Long(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	if (len1 == len2)
	{
		return getCosA(v1, v2, len1);
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
Numeric VectorUtil::getCosA_First(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	if (len1 == len2)
		return getCosA(v1, v2, len1);
	else if (len1 < len2)
		return getCosA_Short(v2, len2, v1, len1);
	else
		return getCosA_Long(v1, len1, v2, len2);
}

template <typename Numeric>
Numeric VectorUtil::getCosA_MinSub(const Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	Numeric base = 1.0f; // getAvg(v1, len1);
	vector<Numeric> samp(len1, base);
	vector<Numeric> temp(len1);
	memcpy(&temp[0], v1, len1 * sizeof(v1[0]));
	Sub(&temp[0], len1, v2, len2);
	Add(&temp[0], len1, base);

	Numeric cosa = getCosA(&samp[0], &temp[0], len1);
	return cosa;
}

template <typename Numeric>
Numeric VectorUtil::getLengthSqr(const Numeric *vec, const size_t length)
{
	double sum = 0;
	for (size_t i = 0; i < length; ++i)
	{
		sum += vec[i] * vec[i];
	}
	return sum;
}

template <typename Numeric>
Numeric VectorUtil::getAvg(const Numeric *vec, const size_t length)
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
Numeric VectorUtil::getDTW(const Numeric *v1, const size_t len1, const Numeric *v2, const size_t len2)
{
	std::unique_ptr<Numeric[]> dp(new Numeric[len1*len2]);
	Numeric *d = dp.get();

	// ÷°∆•≈‰æ‡¿Îæÿ’Û
	for (size_t i = 0; i < len1; i++)
	{
		const size_t ii = i * len2;
		for (size_t j = 0; j < len2; j++)
		{
			d[ii + j] = ::abs(v1[i] - v2[j]);
		}
	}

	// ¿€ª˝æ‡¿Îæÿ’Û
	for (size_t i = 1; i < len1; i++)
	{
		const size_t ii = i * len2;
		const size_t ii_1 = (i - 1) * len2;
		d[ii] += d[ii_1];
	}
	for (size_t j = 1; j < len2; j++)
	{
		d[j] += d[j - 1];
	}
	Numeric d1, d2, d3;
	for (size_t i = 1; i < len1; i++)
	{
		const size_t ii = i * len2;
		const size_t ii_1 = (i - 1) * len2;
		for (size_t j = 1; j < len2; j++)
		{
			const size_t j_1 = j - 1;
			d1 = d[ii_1 + j];
			d2 = d[ii_1 + j_1];
			d3 = d[ii + j_1];
			d[ii + j] += min(d1, d2, d3);
		}
	}

	return d[len1*len2 - 1];
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

template <typename Numeric>
void VectorUtil::Sub(Numeric *v1, size_t len1, const Numeric *v2, size_t len2)
{
	if (len1 < len2)
	{
		Numeric min = FLT_MAX, sum;
		size_t min_offset = 0;
		size_t count = len2 - len1;
		for (size_t k = 0; k <= count; ++k)
		{
			sum = 0;
			for (size_t i = 0; i < len1; ++i)
			{
				sum += abs(v1[i] - v2[i + k]);
			}
			if (min > sum)
			{
				min = sum;
				min_offset = k;
			}
		}
		Sub(v1, v2 + min_offset, len1);
	}
	else if (len1 > len2)
	{
		Numeric min = FLT_MAX, sum;
		size_t min_offset = 0;
		size_t count = len1 - len2;
		for (size_t k = 0; k <= count; ++k)
		{
			sum = 0;
			for (size_t i = 0; i < k; ++i)
			{
				sum += (v1[i] >= 0) ? v1[i] : -v1[i];
			}
			for (size_t i = 0; i < len2; ++i)
			{
				sum += abs(v1[i + k] - v2[i]);
			}
			for (size_t i = k + len2; i < len1; ++i)
			{
				sum += (v1[i] >= 0) ? v1[i] : -v1[i];
			}
			if (min > sum)
			{
				min = sum;
				min_offset = k;
			}
		}
		Sub(v1 + min_offset, v2, len2);
	}
	else
	{
		Sub(v1, v2, len1);
	}
}

template <typename Numeric>
vector<int> VectorUtil::getRiseAndFall(Numeric *v1, const size_t length)
{
	vector<int> result(length);
	result[0] = 0;
	for (size_t i = 1; i < length; ++i)
	{
		Numeric diff = v1[i] - v1[i - 1];
		if (diff > CommUtil::EPSINON)
			result[i] = 1;
		else if (diff < -CommUtil::EPSINON)
			result[i] = -1;
		else
			result[i] = 0;
	}
	return result;
}

} //namespace comm_util
