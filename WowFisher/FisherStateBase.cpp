#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateBase.h"
#include "Fisher.h"
#include <ctime>

std::default_random_engine FisherStateBase::m_random_engine((unsigned int)time(NULL));

using comm_util::StateBase;

FisherStateBase::FisherStateBase(FisherStateType state_type, Fisher* fisher)
    : StateBase((int)state_type)
    , m_fisher(fisher)
{
}


FisherStateBase::~FisherStateBase()
{
}


void FisherStateBase::OnEnter(void* param)
{
    m_timer = 0;
}

void FisherStateBase::OnLeave(void* param)
{
}

void FisherStateBase::Update(int dt)
{
    if (m_timer > 0)
    {
        m_timer -= dt;
        if (m_timer < 0)
        {
            m_timer = 0;
        }
    }
}

void FisherStateBase::SetRandomTimer(int time)
{
    m_timer = time + GetRandomDelay();
}

int FisherStateBase::GetRandomDelay()
{
    static std::uniform_int_distribution<int> dis(0, 30);
    return dis(m_random_engine) + 20;
}
