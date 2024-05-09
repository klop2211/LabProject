#pragma once
#include "State.h"

class CPlayer;

class PIdle : public State<CPlayer>
{
    const float release_weapon_time_ = 2.f;
    float idle_time_ = 0.f;
public:
    static PIdle* Instance();

public:
    virtual void Enter(CPlayer* player);

    virtual void Execute(CPlayer*, float elapsed);

    virtual void Exit(CPlayer* player);

};

class PMove : public State<CPlayer>
{
public:
    static PMove* Instance();

public:
    virtual void Enter(CPlayer* player);

    virtual void Execute(CPlayer* player, float elapsed_time);

    virtual void Exit(CPlayer* player);

};

class PEvade : public State<CPlayer>
{
private:
    //5프레임 부터 이동 시작
    const float move_start_time_ = 1.f / 60.f * 5;
    float evade_time_ = 0.f;

public:
    static PEvade* Instance();

public:
    virtual void Enter(CPlayer* player);

    virtual void Execute(CPlayer* player, float elapsed_time);

    virtual void Exit(CPlayer* player);

};

class PAttack : public State<CPlayer>
{
public:
    static PAttack* Instance();

public:
    virtual void Enter(CPlayer* player);

    virtual void Execute(CPlayer* player, float elapsed_time);

    virtual void Exit(CPlayer* player);
};

class PSwordAttack1 : public State<CPlayer>
{
public:
    static PSwordAttack1* Instance();

public:
    virtual void Enter(CPlayer* player);

    virtual void Execute(CPlayer* player, float elapsed_time);

    virtual void Exit(CPlayer* player);

};

class PSwordAttack2 : public State<CPlayer>
{
public:
    static PSwordAttack2* Instance();

public:
    virtual void Enter(CPlayer* player);

    virtual void Execute(CPlayer* player, float elapsed_time);

    virtual void Exit(CPlayer* player);

};