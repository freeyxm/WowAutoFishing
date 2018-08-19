#pragma once
#include <CommUtil/StateMachine.h>
#include "FisherStateDefine.h"

class FisherStateBase;
class Fisher;

class FisherStateMachine : public comm_util::StateMachine
{
public:
    FisherStateMachine(Fisher* fisher);
    virtual ~FisherStateMachine();

    virtual void Init() override;

private:
    FisherStateBase * CreateState(FisherStateType state_type);

private:
    Fisher * m_fisher;
};

