#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateBait.h"
#include "FisherStateMachine.h"
#include "Fisher.h"


FisherStateBait::FisherStateBait(FisherStateType state_type, Fisher* fisher)
    : FisherStateBase(state_type, fisher)
    , m_has_press_bite1(false)
    , m_has_press_bite2(false)
    , m_has_press_bite3(false)
{
}

FisherStateBait::~FisherStateBait()
{
}

void FisherStateBait::OnEnter(void* param)
{
    m_fisher->PrintStatus(L"上饵...\n");

    m_has_press_bite1 = false;
    m_has_press_bite2 = false;
    m_has_press_bite3 = false;
    m_timer = 0;
}

void FisherStateBait::OnLeave(void* param)
{
}

void FisherStateBait::Update(int dt)
{
    FisherStateBase::Update(dt);

    if (m_timer > 0)
    {
        return;
    }

    if (!m_has_press_bite1)
    {
        m_has_press_bite1 = true;
        if (m_fisher->m_hotkeyBite1 != 0)
        {
            m_fisher->PressKeyboard(m_fisher->m_hotkeyBite1);
            SetRandomTimer(2100);
            return;
        }
    }

    if (!m_has_press_bite2)
    {
        m_has_press_bite2 = true;
        if (m_fisher->m_hotkeyBite2 != 0)
        {
            m_fisher->PressKeyboard(m_fisher->m_hotkeyBite2);
            SetRandomTimer(2100);
            return;
        }
    }

    if (!m_has_press_bite3)
    {
        m_has_press_bite3 = true;
        if (m_fisher->m_hotkeyBite3 != 0)
        {
            m_fisher->PressKeyboard(m_fisher->m_hotkeyBite3);
            SetRandomTimer(2100);
            return;
        }
    }

    if (m_timer <= 0)
    {
        m_fisher->SetBaitTime(time(NULL));
        m_fisher->GetStateMachine().GotoState(FisherStateType::FisherState_ThrowPole);
    }
}
