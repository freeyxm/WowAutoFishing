#pragma once
#include "FisherStateBase.h"
class FisherStateThrowPole :
    public FisherStateBase
{
public:
    FisherStateThrowPole(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateThrowPole();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;
};

