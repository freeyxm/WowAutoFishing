#pragma once

namespace comm_util
{

class VectorUtil
{
public:
	VectorUtil();
	~VectorUtil();

	static float getCosA(const float *v1, const float *v2, const size_t length);
	static float getCosA_Short(const float *v1, size_t len1, const float *v2, size_t len2);
	static float getCosA_Long(const float *v1, size_t len1, const float *v2, size_t len2);
	static float getCosA_First(const float *v1, size_t len1, const float *v2, size_t len2);

	static float getLengthSqr(const float *vec, const size_t length);

	static float getAvg(const float *vec, const size_t length);

	static void Add(float *v1, const size_t length, float num);
	static void Add(float *v1, const float *v2, const size_t length);
	static void Sub(float *v1, const float *v2, const size_t length);
};

} //namespace comm_util
