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
}

void PEvade::Exit(CPlayer* player)
{
	player->movement_component()->set_max_speed(player->speed());
}
