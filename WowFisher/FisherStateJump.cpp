#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateJump.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateJump::FisherStateJump(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
{
}

FisherStateJump::~FisherStateJump()
{
}

void FisherStateJump::OnEnter(void* param)
{
    m_fisher->Jump();
    SetRandomTimer(1500);
}

void FisherStateJump::OnLeave(void* param)
{
}

void FisherStateJump::Update(int dt)
{
    FisherStateBase::Update(dt);

    if (m_timer <= 0)
    {
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_Start);
    }
}
