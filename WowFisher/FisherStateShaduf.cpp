#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateShaduf.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateShaduf::FisherStateShaduf(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
{
}

FisherStateShaduf::~FisherStateShaduf()
{
}

void FisherStateShaduf::OnEnter(void* param)
{
    m_fisher->Shaduf();
    m_timer = 0;
}

void FisherStateShaduf::OnLeave(void* param)
{
}

void FisherStateShaduf::Update(int dt)
{
    FisherStateBase::Update(dt);

    if (m_timer <= 0)
    {
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_WaitFloatHide);
    }
}
