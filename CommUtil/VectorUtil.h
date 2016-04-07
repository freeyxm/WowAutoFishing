#pragma once

namespace comm_util
{

class VectorUtil
{
public:
	VectorUtil();
	~VectorUtil();

	static float getCosA(const float *v1, const float *v2, const size_t length);
	static float getCosA(const float *v1, size_t len1, const float *v2, size_t len2);
	static float getCosA_Pad(const float *v1, size_t len1, const float *v2, size_t len2);

	static float getAvg(const float *data, const size_t length);
};

} //namespace comm_util
