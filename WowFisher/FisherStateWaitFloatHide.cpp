#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateWaitFloatHide.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateWaitFloatHide::FisherStateWaitFloatHide(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
{
}

FisherStateWaitFloatHide::~FisherStateWaitFloatHide()
{
}

void FisherStateWaitFloatHide::OnEnter(void* param)
{
    SetRandomTimer(2500);
}

void FisherStateWaitFloatHide::OnLeave(void* param)
{
}

void FisherStateWaitFloatHide::Update(int dt)
{
    FisherStateBase::Update(dt);

    if (m_timer <= 0)
    {
        if (m_fisher->NeedJump())
        {
            m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_Jump);
        }
        else
        {
            m_fisher->TurnEnd();
            m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_Start);
        }
    }
}
