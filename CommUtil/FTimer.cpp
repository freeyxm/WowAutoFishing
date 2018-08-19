#include "FTimer.h"

namespace comm_util
{

using namespace std::chrono;

FTimer::FTimer()
    : m_duration(0), m_bRunning(false)
{
}

FTimer::~FTimer()
{
}

void FTimer::Start()
{
    m_begin = steady_clock::now();
    m_bRunning = true;
}

void FTimer::Stop()
{
    if (m_bRunning)
    {
        auto end = steady_clock::now();
        m_duration += duration_cast<duration<double>>(end - m_begin);
        m_bRunning = false;
    }
}

void FTimer::Reset()
{
    m_duration = m_duration.zero();
    m_bRunning = false;
}

int64_t FTimer::Seconds()
{
    if (m_bRunning)
    {
        auto end = steady_clock::now();
        auto duration = m_duration + (end - m_begin);
        return duration_cast<seconds>(duration).count();
    }
    else
    {
        return duration_cast<seconds>(m_duration).count();
    }
}

int64_t FTimer::Milliseconds()
{
    if (m_bRunning)
    {
        auto end = steady_clock::now();
        auto duration = m_duration + (end - m_begin);
        return duration_cast<milliseconds>(duration).count();
    }
    else
    {
        return duration_cast<milliseconds>(m_duration).count();
    }
}

}