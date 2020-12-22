#pragma once

template <typename Numeric>
class DTW
{
public:
	DTW(int m, int n);
	~DTW();

	Numeric Calculate(const Numeric* data1, size_t len1, const Numeric* data2, size_t len2);

private:
	Numeric** m_dist;
	Numeric** m_cost;
	int M;
	int N;
};

