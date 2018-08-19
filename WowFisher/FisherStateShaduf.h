#pragma once
#include "FisherStateBase.h"
class FisherStateShaduf :
    public FisherStateBase
{
public:
    FisherStateShaduf(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateShaduf();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;
};

