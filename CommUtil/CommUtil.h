#pragma once

namespace comm_util
{

class CommUtil
{
public:
	CommUtil();
	~CommUtil();
};

template<typename T>
inline void swap(T a, T b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

} // namespace comm_util