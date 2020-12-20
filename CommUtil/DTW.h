#pragma once

template <typename Numeric>
class DTW
{
public:
	DTW(int m, int n);
	~DTW();

	Numeric Calculate(const Numeric* data1, int len1, const Numeric* data2, int len2);

private:
	Numeric** d;
	Numeric** D;
	int M;
	int N;
};

