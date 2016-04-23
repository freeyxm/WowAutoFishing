#pragma once

namespace comm_util
{

class CommUtil
{
public:
	static const float EPSINON;

	static bool isEqual(float a, float b);
	static bool isEqualZero(float a);
};

template<typename T>
inline void swap(T &a, T &b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

template<typename T>
inline T min(T a, T b)
{
	return a < b ? a : b;
}

template<typename T>
inline T max(T a, T b)
{
	return a > b ? a : b;
}

template<typename T>
inline T min(T a, T b, T c)
{
	if (a < b)
		return min(a, c);
	else
		return min(b, c);
}

template<typename T>
inline T max(T a, T b, T c)
{
	if (a > b)
		return max(a, c);
	else
		return max(b, c);
}

} // namespace comm_util