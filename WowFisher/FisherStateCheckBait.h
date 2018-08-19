#pragma once
#include "FisherStateBase.h"
class FisherStateCheckBait :
    public FisherStateBase
{
public:
    FisherStateCheckBait(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateCheckBait();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;
};

