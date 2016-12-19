#pragma once
#include<chrono>

namespace comm_util
{

class FTimer
{
public:
	FTimer();
	~FTimer();

	void Start();
	void Stop();
	void Reset();

	double Seconds();
	double Milliseconds();

private:
	std::chrono::steady_clock::time_point m_begin;
	std::chrono::duration<double> m_duration;
	bool m_bStart;
};

}

