#pragma once
#include "FisherStateBase.h"
class FisherStateWaitBite :
    public FisherStateBase
{
public:
    FisherStateWaitBite(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateWaitBite();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;
};

