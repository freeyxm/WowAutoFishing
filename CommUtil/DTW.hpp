#include "DTW.h"
#include <cstring>


template <typename Numeric>
DTW<Numeric>::DTW(int m, int n)
	: M(m)
	, N(n)
{
	d = new Numeric*[m];
	D = new Numeric*[m];
	for (size_t i = 0; i < m; i++)
	{
		d[i] = new Numeric[n];
		D[i] = new Numeric[n];
	}
}

template <typename Numeric>
DTW<Numeric>::~DTW()
{
	for (size_t i = 0; i < M; i++)
	{
		delete[] d[i];
		delete[] D[i];
	}
	delete[] d;
	delete[] D;
}

template <typename Numeric>
Numeric DTW<Numeric>::Calculate(const Numeric* v1, int len1, const Numeric* v2, int len2)
{
	if (len1 > M)
		len1 = M;
	if (len2 > N)
		len2 = N;

	// Çå0
	for (int i = 0; i < M; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			d[i][j] = 0;
			D[i][j] = 0;
		}
	}

	// Æ¥Åä¾àÀë¾ØÕó
	for (size_t i = 0; i < len1; i++)
	{
		for (size_t j = 0; j < len2; j++)
		{
			Numeric dist = v1[i] - v2[j];
			d[i][j] = abs(dist);
		}
	}

	// ÀÛ»ý¾àÀë¾ØÕó
	D[0][0] = d[0][0];
	for (size_t i = 1; i < len1; i++)
	{
		D[i][0] += D[i - 1][0];
	}
	for (size_t j = 1; j < len2; j++)
	{
		D[0][j] += D[0][j - 1];
	}
	Numeric d1, d2, d3;
	for (size_t i = 1; i < len1; i++)
	{
		for (size_t j = 1; j < len2; j++)
		{
			d1 = D[i - 1][j];
			d2 = D[i - 1][j - 1];
			d3 = D[i][j - 1];
			D[i][j] = d[i][j] + min(min(d1, d2), d3);
		}
	}

	Numeric result = D[len1 - 1][len2 - 1];
	return result;
}