#pragma once
#include "FisherStateBase.h"
class FisherStateJump :
    public FisherStateBase
{
public:
    FisherStateJump(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateJump();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;
};

