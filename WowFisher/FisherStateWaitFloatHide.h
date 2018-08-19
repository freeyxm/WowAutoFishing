#pragma once
#include "FisherStateBase.h"
class FisherStateWaitFloatHide :
    public FisherStateBase
{
public:
    FisherStateWaitFloatHide(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateWaitFloatHide();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;
};

