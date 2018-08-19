﻿#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateFindFloat.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateFindFloat::FisherStateFindFloat(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
    , m_has_find_float(false)
{
}

FisherStateFindFloat::~FisherStateFindFloat()
{
}

void FisherStateFindFloat::OnEnter(void* param)
{
    m_fisher->PrintStatus(L"寻找鱼漂...\n");

    m_has_find_float = m_fisher->FindFloat();
    if (!m_has_find_float)
    {
        m_timer = 1500; // 寻找鱼漂失败，等现鱼漂消失再重新抛竿。
    }
    else
    {
        m_timer = 0;
    }
}

void FisherStateFindFloat::OnLeave(void* param)
{
}

void FisherStateFindFloat::Update(int dt)
{
    FisherStateBase::Update(dt);

    if (m_timer <= 0)
    {
        if (m_has_find_float)
        {
            m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_WaitBite);
        }
        else
        {
            m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_ThrowPole);
        }
    }
}
