#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateThrowPole.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateThrowPole::FisherStateThrowPole(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
{
}

FisherStateThrowPole::~FisherStateThrowPole()
{
}

void FisherStateThrowPole::OnEnter(void* param)
{
    m_fisher->PrintStatus(L"甩竿...\n");
    m_fisher->ThrowPole();

    SetRandomTimer(1500);
}

void FisherStateThrowPole::OnLeave(void* param)
{
}

void FisherStateThrowPole::Update(int dt)
{
    FisherStateBase::Update(dt);

    if (m_timer <= 0)
    {
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_FindFloat);
    }
}
