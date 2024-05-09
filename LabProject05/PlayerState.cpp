#include "stdafx.h"
#include "Player.h"
#include "PlayerState.h"
#include "MovementComponent.h"

PIdle* PIdle::Instance()
{
	static PIdle instance;
	return &instance;
}

void PIdle::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::SwordIdle);
}

void PIdle::Execute(CPlayer* player, float elapsed)
{
	if (!IsZero(Vector3::Length(player->movement_component()->direction_vector())))
		player->state_machine()->ChangeState(PMove::Instance());

}

void PIdle::Exit(CPlayer* player)
{
}

PMove* PMove::Instance()
{
	static PMove instance;
	return &instance;
}

void PMove::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::Run);
}

void PMove::Execute(CPlayer* player, float elapsed_time)
{
	if(player->orient_rotation_to_movement())
		player->OrientRotationToMove(elapsed_time);
	if (IsZero(Vector3::Length(player->movement_component()->direction_vector())))
		player->state_machine()->ChangeState(PIdle::Instance());

}

void PMove::Exit(CPlayer* player)
{
}

PEvade* PEvade::Instance()
{
	static PEvade instance;
	return &instance;
}

void PEvade::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::Roll);
	player->movement_component()->set_max_speed(700);
	evade_time_ = 0.f;
}

void PEvade::Execute(CPlayer* player, float elapsed_time)
{
	evade_time_ += elapsed_time;
	if (move_start_time_ < evade_time_)
		player->movement_component()->set_direction_vector(player->look_vector());
	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}

}

void PEvade::Exit(CPlayer* player)
{
	player->movement_component()->set_max_speed(player->speed());
}

PAttack* PAttack::Instance()
{
	static PAttack instance;
	return &instance;
}

void PAttack::Enter(CPlayer* player)
{

	player->set_animation_state(PlayerAnimationState::SwordAttack2);
}

void PAttack::Execute(CPlayer* player, float elapsed_time)
{
	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}

}

void PAttack::Exit(CPlayer* player)
{
}

PSwordAttack1* PSwordAttack1::Instance()
{
	static PSwordAttack1 instance;
	return &instance;
}

void PSwordAttack1::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::SwordAttack1);
}

void PSwordAttack1::Execute(CPlayer* player, float elapsed_time)
{
	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}
}

void PSwordAttack1::Exit(CPlayer* player)
{
}

PSwordAttack2* PSwordAttack2::Instance()
{
	static PSwordAttack2 instance;
	return &instance;
}

void PSwordAttack2::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::SwordAttack2);
}

void PSwordAttack2::Execute(CPlayer* player, float elapsed_time)
{
	if (player->orient_rotation_to_movement())
		player->OrientRotationToMove(elapsed_time);

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}
}

void PSwordAttack2::Exit(CPlayer* player)
{
}
