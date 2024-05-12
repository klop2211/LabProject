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
	idle_time_ = 0.f;
	player->set_attack_type(PlayerAttackType::None);
	switch (player->current_weapon())
	{
	case PlayerWeaponType::None:
		player->set_animation_state(PlayerAnimationState::Idle);
		break;
	case PlayerWeaponType::Sword:
		player->set_animation_state(PlayerAnimationState::SwordIdle);
		break;

	default:
		break;
	}
}

void PIdle::Execute(CPlayer* player, float elapsed)
{
	idle_time_ += elapsed;
	if (!player->animation_controller()->is_animation_chainging())
		player->animation_controller()->set_is_blend_change(true);
	if (idle_time_ > release_weapon_time_)
	{
		player->set_current_weapon(PlayerWeaponType::None);
		player->weapon_socket()->set_is_visible(false);
		player->set_animation_state(PlayerAnimationState::Idle);
	}
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
	player->set_current_weapon(PlayerWeaponType::None);
	player->weapon_socket()->set_is_visible(false);
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
	//TODO: �������ݿ� ���缭 ������ �� �ܰ躰 �ִϸ��̼� ����
	player->set_animation_state(PlayerAnimationState::SwordAttack11);
	charge_time_ = 0.f;
	is_charging_ = false;
}

void PSwordAttack1::Execute(CPlayer* player, float elapsed_time)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		is_charging_ = true;
	else
		is_charging_ = false;

	if (is_charging_)
		charge_time_ += elapsed_time;

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		//�������� ��
		if (player->animation_state() == PlayerAnimationState::SwordAttack11)
		{
			player->animation_controller()->set_is_blend_change(false);
			if (!is_charging_)
				player->set_animation_state(PlayerAnimationState::SwordAttack13);
			else
			{
				player->set_animation_state(PlayerAnimationState::SwordAttack12);
			}
			return;
		}
		//�������� ��
		if (player->animation_state() == PlayerAnimationState::SwordAttack12)
		{
			if (charge_time_ > max_charge_time_)
			{
				player->set_animation_state(PlayerAnimationState::SwordAttack13);
			}
			else if (is_charging_)
				player->animation_controller()->EnableTrack((int)PlayerAnimationState::SwordAttack12);
			else
				player->set_animation_state(PlayerAnimationState::SwordAttack13);
			return;
		}
		//���⵿�� ��
		if (player->animation_state() == PlayerAnimationState::SwordAttack13)
		{
			player->state_machine()->ChangeState(PIdle::Instance());
		}
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
	//TODO: ������� ���� �� �����̰� ����
	player->set_animation_state(PlayerAnimationState::SwordAttack21);
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

PSwordAttack3* PSwordAttack3::Instance()
{
	static PSwordAttack3 instance;
	return &instance;
}

void PSwordAttack3::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::SwordAttack30);

}

void PSwordAttack3::Execute(CPlayer* player, float elapsed_time)
{
	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}
}

void PSwordAttack3::Exit(CPlayer* player)
{
}

PSwordAttack4* PSwordAttack4::Instance()
{
	static PSwordAttack4 instance;
	return &instance;
}

void PSwordAttack4::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::SwordAttack40);

}

void PSwordAttack4::Execute(CPlayer* player, float elapsed_time)
{
	//TODO: �ִϸ��̼��� ������ �κп� ���߾� ���⸦ invisible�� ��ȯ
	//player->weapon_socket()->set_is_visible(false);

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}
}

void PSwordAttack4::Exit(CPlayer* player)
{
	//TODO: ���� ��ü ����
}
