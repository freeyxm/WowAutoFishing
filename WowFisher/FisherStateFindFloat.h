#pragma once
#include "FisherStateBase.h"
class FisherStateFindFloat :
    public FisherStateBase
{
public:
    FisherStateFindFloat(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateFindFloat();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;

private:
    bool m_has_find_float;
};

