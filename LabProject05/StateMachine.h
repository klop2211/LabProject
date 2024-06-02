#pragma once
#include "State.h"

template<class entity_type>
class StateMachine
{
    entity_type* owner_;

    State<entity_type>* current_state_;

    State<entity_type>* previous_state_;

public:

    StateMachine(entity_type* owner) :
        owner_(owner),
        current_state_(NULL),
        previous_state_(NULL)
    {}

    virtual ~StateMachine() {}

    void SetCurrentState(State<entity_type>* s) { current_state_ = s; }
    void SetPreviousState(State<entity_type>* s) { previous_state_ = s; }

    //���� ������ Execute ����
    void  Update(float elapsed)const
    {
        if (current_state_)
        {
            current_state_->Execute(owner_, elapsed);
        }
    }

    //���� ����: ���� ������ exit ȣ�� �� ������ �� ���ο� ������ enter�� ȣ���Ѵ�.
    void  ChangeState(State<entity_type>* pNewState)
    {
        previous_state_ = current_state_;

        current_state_->Exit(owner_);

        current_state_ = pNewState;

        current_state_->Enter(owner_);
    }

    //���� ���·� �ǵ�����.
    void  RevertToPreviousState()
    {
        ChangeState(previous_state_);
    }

    // ���� ���°� ���ڷ� ���� ���¿� �������� üũ
    bool  isInState(const State<entity_type>& st)const
    {
        return typeid(*current_state_) == typeid(st);
    }

    State<entity_type>* CurrentState()  const { return current_state_; }
    State<entity_type>* PreviousState() const { return previous_state_; }


};
