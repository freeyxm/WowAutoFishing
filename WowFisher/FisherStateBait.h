#pragma once
#include "FisherStateBase.h"

class FisherStateBait : public FisherStateBase
{
public:
    FisherStateBait(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateBait();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;

private:
    bool m_has_press_bite1;
    bool m_has_press_bite2;
    bool m_has_press_bite3;
};

