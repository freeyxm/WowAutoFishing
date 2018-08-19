#ifndef _COMM_UTIL_STATE_MACHINE_H__
#define _COMM_UTIL_STATE_MACHINE_H__
#include "StateBase.h"
#include <map>

namespace comm_util
{

class StateMachine
{
public:
    StateMachine();
    virtual ~StateMachine();

    virtual void Init();
    virtual void Update(int dt);

    virtual bool GotoState(int state_type, void* param = NULL);

    virtual StateBase* GetCurState() const;
    virtual int GetCurStateType() const;
    virtual int GetPreStateType() const;

protected:
    virtual bool AddState(StateBase* state);

protected:
    std::map<int, StateBase*> m_state_map;
    StateBase * m_cur_state;
    int m_pre_state;
};

} // namespace comm_util
#endif // !_COMM_UTIL_STATE_MACHINE_H__
