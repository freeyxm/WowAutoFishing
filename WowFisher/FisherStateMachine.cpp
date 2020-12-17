#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include "FisherStateMachine.h"
#include "FisherStateBase.h"
#include "FisherStateBait.h"
#include "FisherStateCheckBait.h"
#include "FisherStateFindFloat.h"
#include "FisherStateShaduf.h"
#include "FisherStateThrowPole.h"
#include "FisherStateWaitBite.h"
#include "FisherStateWaitFloatHide.h"
#include "FisherStateJump.h"
#include "Fisher.h"


FisherStateMachine::FisherStateMachine(Fisher* fisher)
    : m_fisher(fisher)
{
}

FisherStateMachine::~FisherStateMachine()
{
}

void FisherStateMachine::Init()
{
    AddState(CreateState(FisherStateType::FisherState_Start));
    AddState(CreateState(FisherStateType::FisherState_CheckBait));
    AddState(CreateState(FisherStateType::FisherState_Bait));
    AddState(CreateState(FisherStateType::FisherState_ThrowPole));
    AddState(CreateState(FisherStateType::FisherState_FindFloat));
    AddState(CreateState(FisherStateType::FisherState_WaitBite));
    AddState(CreateState(FisherStateType::FisherState_Shaduf));
    AddState(CreateState(FisherStateType::FisherState_WaitFloatHide));
    AddState(CreateState(FisherStateType::FisherState_Jump));
}

FisherStateBase* FisherStateMachine::CreateState(FisherStateType state_type)
{
    switch (state_type)
    {
    case FisherState_Start:
    case FisherState_CheckBait:
        return new FisherStateCheckBait(state_type, m_fisher);
    case FisherState_Bait:
        return new FisherStateBait(state_type, m_fisher);
    case FisherState_ThrowPole:
        return new FisherStateThrowPole(state_type, m_fisher);
    case FisherState_FindFloat:
        return new FisherStateFindFloat(state_type, m_fisher);
    case FisherState_WaitBite:
        return new FisherStateWaitBite(state_type, m_fisher);
    case FisherState_Shaduf:
        return new FisherStateShaduf(state_type, m_fisher);
    case FisherState_WaitFloatHide:
        return new FisherStateWaitFloatHide(state_type, m_fisher);
    case FisherState_Jump:
        return new FisherStateJump(state_type, m_fisher);
    default:
        return new FisherStateBase(state_type, m_fisher);
        break;
    }
}
