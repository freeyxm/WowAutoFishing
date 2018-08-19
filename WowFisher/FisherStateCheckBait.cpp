#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateCheckBait.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateCheckBait::FisherStateCheckBait(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
{
}

FisherStateCheckBait::~FisherStateCheckBait()
{
}

void FisherStateCheckBait::OnEnter(void* param)
{
    m_timer = 0;
}

void FisherStateCheckBait::OnLeave(void* param)
{
}

void FisherStateCheckBait::Update(int dt)
{
    bool is_bait_timeout = false;
    time_t bait_time = m_fisher->GetBaitTime();
    if (bait_time == 0)
    {
        is_bait_timeout = true;
    }
    else
    {
        time_t now = time(NULL);
        is_bait_timeout = (now - bait_time) > (MAX_BAIT_TIME - MAX_BITE_TIME);
    }

    if (is_bait_timeout)
    {
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_Bait);
    }
    else
    {
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_ThrowPole);
    }
}
