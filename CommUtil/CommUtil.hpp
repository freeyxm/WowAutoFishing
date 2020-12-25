#pragma once

#undef min
#undef max

#define SAFE_FREE(p) if(p) { free(p); p = NULL; }
#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#define SAFE_DELETE_A(p) if(p) { delete[] p; p = NULL; }

namespace comm_util
{

class CommUtil
{
public:
	static const float FLOAT_EPSILON;
	static const double DOUBLE_EPSILON;

	static bool IsEqual(float a, float b);
	static bool IsEqual(double a, double b);
	static bool IsEqualZero(float a);
	static bool IsEqualZero(double a);
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