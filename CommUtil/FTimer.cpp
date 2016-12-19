#include "FTimer.h"

namespace comm_util
{

using namespace std::chrono;

FTimer::FTimer()
	: m_duration(0), m_bStart(false)
{
}

FTimer::~FTimer()
{
}

void FTimer::Start()
{
	m_begin = steady_clock::now();
	m_bStart = true;
}

void FTimer::Stop()
{
	if (m_bStart)
	{
		auto end = steady_clock::now();
		m_duration += duration_cast<duration<double>>(end - m_begin);
		m_bStart = false;
	}
}

void FTimer::Reset()
{
	m_duration = m_duration.zero();
	m_bStart = false;
}

double FTimer::Seconds()
{
	return m_duration.count();
}

double FTimer::Milliseconds()
{
	return m_duration.count() * 1000;
}

}