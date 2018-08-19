#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateWaitBite.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateWaitBite::FisherStateWaitBite(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
{
}

FisherStateWaitBite::~FisherStateWaitBite()
{
}

void FisherStateWaitBite::OnEnter(void* param)
{
    m_fisher->PrintStatus(L"等待上钩...\n");
    m_fisher->StartListenBite();

    SetRandomTimer(MAX_BITE_TIME * 1000);
}

void FisherStateWaitBite::OnLeave(void* param)
{
    m_fisher->StopListenBite();
}

void FisherStateWaitBite::Update(int dt)
{
    FisherStateBase::Update(dt);

    bool has_bite = m_fisher->HasBite();
    if (has_bite)
    {
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_Shaduf);
    }
    else if (m_timer <= 0)
    {
        m_fisher->PrintStatus(L"没有鱼儿上钩！\n");
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_WaitFloatHide);
    }
}
