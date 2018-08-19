#include "StateBase.h"

namespace comm_util
{

StateBase::StateBase(int state_type)
    : m_state_type(state_type)
{
}

StateBase::~StateBase()
{
}

void StateBase::OnEnter(void* param)
{
}

void StateBase::OnLeave(void* param)
{
}

void StateBase::Update(int dt)
{
}

int StateBase::GetStateType() const
{
    return m_state_type;
}

} // namespace comm_util
