#pragma once

namespace comm_util
{

class VectorUtil
{
public:
	VectorUtil();
	~VectorUtil();

	static float getCosA(const float *v1, const float *v2, const int length);
	static float getCosA(const float *v1, int len1, const float *v2, int len2);
	static float getCosA_Pad(const float *v1, int len1, const float *v2, int len2);

	template<typename T>
	inline static void swap(T a, T b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}
};

} //namespace comm_util
