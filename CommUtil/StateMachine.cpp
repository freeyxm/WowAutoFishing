#include "StateMachine.h"

namespace comm_util
{

StateMachine::StateMachine()
    : m_cur_state(NULL)
    , m_pre_state(0)
{
}


StateMachine::~StateMachine()
{
    for (auto iter = m_state_map.begin(); iter != m_state_map.end(); ++iter)
    {
        delete iter->second;
    }
    m_state_map.clear();
    m_cur_state = NULL;
}

void StateMachine::Init()
{
    // init m_state_map
}

void StateMachine::Update(int dt)
{
    if (m_cur_state != NULL)
    {
        m_cur_state->Update(dt);
    }
}

bool StateMachine::GotoState(int state_type, void* param)
{
    auto iter = m_state_map.find(state_type);
    if (iter == m_state_map.end())
    {
        _ASSERT(false);
        return false;
    }

    if (m_cur_state != NULL)
    {
        m_cur_state->OnLeave(param);
        m_pre_state = m_cur_state->GetStateType();
    }

    m_cur_state = iter->second;
    m_cur_state->OnEnter(param);

    return true;
}

StateBase* StateMachine::GetCurState() const
{
    return m_cur_state;
}

int StateMachine::GetCurStateType() const
{
    if (m_cur_state != NULL)
        return m_cur_state->GetStateType();
    else
        return 0;
}

int StateMachine::GetPreStateType() const
{
    return m_pre_state;
}

bool StateMachine::AddState(StateBase* state)
{
    if (state == NULL)
    {
        return false;
    }

    if (m_state_map.find(state->GetStateType()) != m_state_map.end())
    {
        return false;
    }

    m_state_map.insert(std::make_pair(state->GetStateType(), state));
    return true;
}

} // namespace comm_util
