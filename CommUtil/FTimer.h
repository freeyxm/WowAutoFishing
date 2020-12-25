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

    int64_t Seconds();
    int64_t MilliSeconds();

private:
    std::chrono::steady_clock::time_point m_begin;
    std::chrono::duration<double> m_duration;
    bool m_running;
};

}

