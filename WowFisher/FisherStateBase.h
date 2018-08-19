#pragma once
#include <CommUtil/StateBase.h>
#include <random>
#include "FisherStateDefine.h"

class Fisher;

class FisherStateBase : public comm_util::StateBase
{
public:
    FisherStateBase(FisherStateType state_type, Fisher* fisher);
    virtual ~FisherStateBase();

    virtual void OnEnter(void* param) override;
    virtual void OnLeave(void* param) override;
    virtual void Update(int dt) override;

protected:
    virtual void SetRandomTimer(int time);
    virtual int GetRandomDelay();

protected:
    Fisher * m_fisher;
    int m_timer;
    static std::default_random_engine m_random_engine;
};

