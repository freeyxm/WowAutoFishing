#ifndef _COMM_UTIL_STATE_BASE_H__
#define _COMM_UTIL_STATE_BASE_H__
#include "stddef.h"

namespace comm_util
{

class StateBase
{
public:
    StateBase(int state_type);
    virtual ~StateBase();

    virtual void OnEnter(void* param);
    virtual void OnLeave(void* param);
    virtual void Update(int dt);

    int GetStateType() const;

protected:
    int m_state_type;
};

} // namespace comm_util
#endif // !_COMM_UTIL_STATE_BASE_H__
