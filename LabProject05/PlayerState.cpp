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
	case WeaponType::None:
		player->set_animation_state(PlayerAnimationState::Idle);
		break;
	case WeaponType::Sword:
		player->set_animation_state(PlayerAnimationState::SwordIdle);
		break;
	case WeaponType::Sphere:
		player->set_animation_state(PlayerAnimationState::SphereIdle);
		break;

	default:
		break;
	}

	player->OffWeaponObb();
}

void PIdle::Execute(CPlayer* player, float elapsed)
{
	if (!player->animation_controller()->is_animation_chainging())
	{
		player->animation_controller()->set_is_blend_change(true);
		player->animation_controller()->set_is_end_time_blending(false);
		player->set_is_move_allow(true);
	}
	
	idle_time_ += elapsed;

	if (player->animation_state() != PlayerAnimationState::Idle)
	{
		player->EnableWeapon();
	}

	if (idle_time_ > release_weapon_time_)
	{
		player->set_current_weapon(WeaponType::None);
		player->DisableWeapon();
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
	player->set_current_weapon(WeaponType::None);
	player->DisableWeapon();
	player->set_animation_state(PlayerAnimationState::Run);
	player->movement_component()->set_max_speed(player->speed());
	player->OffWeaponObb();
}

void PMove::Execute(CPlayer* player, float elapsed_time)
{
	if (!player->animation_controller()->is_animation_chainging())
	{
		player->animation_controller()->set_is_blend_change(true);
		player->set_is_move_allow(true);
	}
	if(player->orient_rotation_to_movement())
		player->OrientRotationToMove(elapsed_time);
	if (IsZero(Vector3::Length(player->movement_component()->direction_vector())))
		player->state_machine()->ChangeState(PIdle::Instance());

}

void PMove::Exit(CPlayer* player)
{
	player->movement_component()->set_max_speed(0.f);
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
	player->OffWeaponObb();
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
	//player->movement_component()->set_max_speed(player->speed());
}

void PAttack::Enter(CPlayer* player)
{
}

PAttack1* PAttack1::Instance(const WeaponType& type)
{
	switch (type)
	{
		case WeaponType::Sword:
			return PSwordAttack1::Instance();
		case WeaponType::Sphere:
			return PSphereAttack1::Instance();
	default:
		break;
	}
}

PAttack2* PAttack2::Instance(const WeaponType& type)
{
	switch (type)
	{
	case WeaponType::Sword:
		return PSwordAttack2::Instance();
	case WeaponType::Sphere:
		return PSphereAttack2::Instance();
	default:
		break;
	}
}

PAttack3* PAttack3::Instance(const WeaponType& type)
{
	switch (type)
	{
	case WeaponType::Sword:
		return PSwordAttack3::Instance();
	case WeaponType::Sphere:
		return PSphereAttack3::Instance();
	default:
		break;
	}
}

PAttack4* PAttack4::Instance(const WeaponType& type)
{
	switch (type)
	{
	case WeaponType::Sword:
		return PSwordAttack4::Instance();
	case WeaponType::Sphere:
		return PSphereAttack4::Instance();
	default:
		break;
	}
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||| < SwordAttack > ||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

PSwordAttack1* PSwordAttack1::Instance()
{
	static PSwordAttack1 instance;
	return &instance;
}

void PSwordAttack1::Enter(CPlayer* player)
{
	player->set_animation_state(PlayerAnimationState::SwordAttack11);
	player->set_is_move_allow(false);
	player->movement_component()->set_max_speed(0.f);
	charge_step_ = 0;
	slash_time_ = 0.f;
	is_charging_ = false;
}

void PSwordAttack1::Execute(CPlayer* player, float elapsed_time)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		is_charging_ = true;
	else
		is_charging_ = false;

	if (charge_step_ > max_charge_step_)
		is_charging_ = false;

	if(!is_charging_ && player->animation_state() == PlayerAnimationState::SwordAttack13)
	{
		slash_time_ += elapsed_time;
		if (slash_time_ > slash_move_start_time_)
		{
			player->movement_component()->set_max_speed(slash_move_speed_);
			player->movement_component()->set_direction_vector(player->look_vector());
		}
		if (slash_time_ > slash_move_end_time_)
		{
			player->movement_component()->set_max_speed(0.f);
			player->movement_component()->set_direction_vector(XMFLOAT3(0,0,0));
		}
	}

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		//사전동작 끝
		if (player->animation_state() == PlayerAnimationState::SwordAttack11)
		{
			player->animation_controller()->set_is_blend_change(false);
			if (!is_charging_)
			{
				player->set_animation_state(PlayerAnimationState::SwordAttack13);
				player->ClearDamagedObjectList();
				player->OnWeaponObb();
				player->SetWeaponDamageScale(charge_damage_scale_[charge_step_]);
			}
			else
			{
				player->set_animation_state(PlayerAnimationState::SwordAttack12);
			}
			return;
		}
		//차지동작 끝
		if (player->animation_state() == PlayerAnimationState::SwordAttack12)
		{
			if (charge_step_ > max_charge_step_)
			{
				player->set_animation_state(PlayerAnimationState::SwordAttack13);
				player->ClearDamagedObjectList();
				player->OnWeaponObb();
				player->SetWeaponDamageScale(charge_damage_scale_[max_charge_step_]);

			}
			else if (is_charging_)
			{
				player->animation_controller()->EnableTrack((int)PlayerAnimationState::SwordAttack12);
				charge_step_++;
			}
			else
			{
				player->set_animation_state(PlayerAnimationState::SwordAttack13);
				player->ClearDamagedObjectList();
				player->OnWeaponObb();
				player->SetWeaponDamageScale(charge_damage_scale_[charge_step_]);

			}
			return;
		}
		//베기동작 끝
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
	player->set_animation_state(PlayerAnimationState::SwordAttack21);
	hold_time_ = 0.f;
	is_holding_ = false;
	player->set_is_move_allow(false);
	player->movement_component()->set_max_speed(0.f);
}

void PSwordAttack2::Execute(CPlayer* player, float elapsed_time)
{
	//if (player->orient_rotation_to_movement())
	//	player->OrientRotationToMove(elapsed_time);

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		is_holding_ = true;
	else
		is_holding_ = false;

	if (is_holding_ && player->animation_state() == PlayerAnimationState::SwordAttack22)
	{
		hold_time_ += elapsed_time;
		player->movement_component()->set_max_speed(slash_move_speed_);
	}
	
	if (hold_time_ > max_hold_time_)
		is_holding_ = false;

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		//사전동작 끝
		if (player->animation_state() == PlayerAnimationState::SwordAttack21)
		{
			player->animation_controller()->set_is_blend_change(false);
			player->ClearDamagedObjectList();
			player->OnWeaponObb();
			if (!is_holding_)
			{
				player->set_animation_state(PlayerAnimationState::SwordAttack23);
				player->SetWeaponDamageScale(damage_scale_last_attack_);
			}
			else
			{
				player->set_is_move_allow(true);
				player->set_animation_state(PlayerAnimationState::SwordAttack22);
				player->SetWeaponDamageScale(damage_scale_holding_attack_);

			}
			return;
		}
		//회전동작 끝
		if (player->animation_state() == PlayerAnimationState::SwordAttack22)
		{
			if (is_holding_)
			{
				player->animation_controller()->EnableTrack((int)PlayerAnimationState::SwordAttack22);
				player->ClearDamagedObjectList();
			}
			else
			{
				player->set_is_move_allow(false);
				player->movement_component()->set_max_speed(0.f);
				player->set_animation_state(PlayerAnimationState::SwordAttack23);
				player->ClearDamagedObjectList();
				player->SetWeaponDamageScale(damage_scale_last_attack_);
			}
			return;
		}
		//베기동작 끝
		if (player->animation_state() == PlayerAnimationState::SwordAttack23)
		{
			player->state_machine()->ChangeState(PIdle::Instance());
		}
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
	player->set_is_move_allow(false);
	attack_time_ = 0.f;
	obb_triger_ = false;
}

void PSwordAttack3::Execute(CPlayer* player, float elapsed_time)
{
	attack_time_ += elapsed_time;

	for (int i = 0; i < move_end_time_.size(); ++i)
	{
		if (move_start_time_[i] < attack_time_ && attack_time_ < move_end_time_[i])
		{
			player->movement_component()->set_direction_vector(player->look_vector());
			player->movement_component()->set_max_speed(move_speed_[i]);
			//처음 움직이기 시작하는 시점부터 obb on
			if(!obb_triger_)
			{
				player->ClearDamagedObjectList();
				player->OnWeaponObb();
				player->SetWeaponDamageScale(damage_scale_);
				obb_triger_ = true;
			}
			break;
		}
		else
		{
			player->movement_component()->set_direction_vector(0, 0, 0);
			player->movement_component()->set_max_speed(0);
		}
	}
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
	player->set_is_move_allow(false);
	attack_time_ = 0.f;
	obb_triger_ = false;
}

void PSwordAttack4::Execute(CPlayer* player, float elapsed_time)
{
	attack_time_ += elapsed_time;

	if(attack_time_ > weapon_invisible_time_)
		player->DisableWeapon();


	for (int i = 0; i < move_end_time_.size(); ++i)
	{
		if (move_start_time_[i] < attack_time_ && attack_time_ < move_end_time_[i])
		{
			player->movement_component()->set_direction_vector(player->look_vector());
			player->movement_component()->set_max_speed(move_speed_[i]);
			//처음 움직이기 시작하는 시점부터 obb on
			if (!obb_triger_)
			{
				player->ClearDamagedObjectList();
				player->OnWeaponObb();
				player->SetWeaponDamageScale(damage_scale_);
				obb_triger_ = true;
			}
			break;
		}
		else
		{
			player->movement_component()->set_direction_vector(0,0,0);
			player->movement_component()->set_max_speed(0);
		}
	}

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->animation_controller()->set_is_blend_change(false);
		player->set_current_weapon(WeaponType::None);
		player->state_machine()->ChangeState(PIdle::Instance());
	}
}

void PSwordAttack4::Exit(CPlayer* player)
{
	player->ChangeWeapon();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||| < PSphereAttack > |||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

PSphereAttack1* PSphereAttack1::Instance()
{
	static PSphereAttack1 instance;
	return &instance;
}

void PSphereAttack1::Enter(CPlayer* player)
{
	player->set_is_move_allow(false);
	player->set_animation_state(PlayerAnimationState::SphereAttack11);
	attack_time_ = 0.f;
	is_click_ = false;
	obb_triger_ = false;
}

void PSphereAttack1::Execute(CPlayer* player, float elapsed_time)
{
	attack_time_ += elapsed_time;

	for (int i = 0; i < move_end_time_.size(); ++i)
	{
		if (move_start_time_[i] < attack_time_ && attack_time_ < move_end_time_[i])
		{
			player->movement_component()->set_direction_vector(player->look_vector());
			player->movement_component()->set_max_speed(move_speed_[i]);
			if (i == 0)
			{
				if (!obb_triger_)
				{
					player->OnWeaponObb();
					player->ClearDamagedObjectList();
					player->SetWeaponDamageScale(damage_scale_first_attack_);
					obb_triger_ = true;
				}
			}
			// i == 1
			else
			{
				player->ClearDamagedObjectList();
				player->SetWeaponDamageScale(damage_scale_second_attack_);

			}
			break;
		}
		else
		{
			player->movement_component()->set_direction_vector(0, 0, 0);
			player->movement_component()->set_max_speed(0);
		}
	}

	if (attack_time_ > 0.1 && GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		is_click_ = true;

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		//1타 끝
		if (player->animation_state() == PlayerAnimationState::SphereAttack11)
		{
			player->ClearDamagedObjectList();
			if (is_click_) 
			{
				player->animation_controller()->set_is_blend_change(false);
				player->set_animation_state(PlayerAnimationState::SphereAttack12);

			}
			else
			{
				player->animation_controller()->set_is_end_time_blending(true);
				player->state_machine()->ChangeState(PIdle::Instance());
			}
			return;
		}
		//2타 끝
		if (player->animation_state() == PlayerAnimationState::SphereAttack12)
		{
			player->state_machine()->ChangeState(PIdle::Instance());
			return;
		}
	}
}

void PSphereAttack1::Exit(CPlayer* player)
{
}

PSphereAttack2* PSphereAttack2::Instance()
{
	static PSphereAttack2 instance;
	return &instance;
}

void PSphereAttack2::Enter(CPlayer* player)
{
	player->set_is_move_allow(false);
	player->set_animation_state(PlayerAnimationState::SphereAttack20);
	attack_time_ = 0.f;
}

void PSphereAttack2::Execute(CPlayer* player, float elapsed_time)
{
	attack_time_ += elapsed_time;

	for (int i = 0; i < move_end_time_.size(); ++i)
	{
		if (move_start_time_[i] < attack_time_ && attack_time_ < move_end_time_[i])
		{
			player->movement_component()->set_direction_vector(player->look_vector());
			player->movement_component()->set_max_speed(move_speed_[i]);
			break;
		}
		else
		{
			player->movement_component()->set_direction_vector(0, 0, 0);
			player->movement_component()->set_max_speed(0);
		}
	}

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}
}

void PSphereAttack2::Exit(CPlayer* player)
{
}

PSphereAttack3* PSphereAttack3::Instance()
{
	static PSphereAttack3 instance;
	return &instance;
}

void PSphereAttack3::Enter(CPlayer* player)
{
	player->set_is_move_allow(false);
	player->set_animation_state(PlayerAnimationState::SphereAttack31);
	is_charging_ = false;
}

void PSphereAttack3::Execute(CPlayer* player, float elapsed_time)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		is_charging_ = true;
	else
		is_charging_ = false;

	if (is_charging_)
		charge_time_ += elapsed_time;
	
	if (player->animation_state() == PlayerAnimationState::SphereAttack33)
	{
		attack_time_ += elapsed_time;
	}

	if (move_start_time_ < attack_time_ && attack_time_ < move_end_time_)
	{
		player->movement_component()->set_direction_vector(player->look_vector());
		player->movement_component()->set_max_speed(move_speed_);
	}
	else
	{
		player->movement_component()->set_direction_vector(0, 0, 0);
		player->movement_component()->set_max_speed(0);
	}

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		//1타 끝
		if (player->animation_state() == PlayerAnimationState::SphereAttack31)
		{
			player->animation_controller()->set_is_blend_change(false);
			if (is_charging_)
				player->set_animation_state(PlayerAnimationState::SphereAttack32);
			else
				player->set_animation_state(PlayerAnimationState::SphereAttack33);

			return;
		}
		//2타 끝
		if (player->animation_state() == PlayerAnimationState::SphereAttack32)
		{
			if (charge_time_ > max_charge_time_)
			{
				player->set_animation_state(PlayerAnimationState::SphereAttack33);

			}
			else if (is_charging_)
				player->animation_controller()->EnableTrack((int)PlayerAnimationState::SphereAttack32);
			else
			{
				player->set_animation_state(PlayerAnimationState::SphereAttack33);
			}
			return;
		}
		//3타 끝
		if (player->animation_state() == PlayerAnimationState::SphereAttack33)
		{
			player->state_machine()->ChangeState(PIdle::Instance());
			return;
		}
	}

}

void PSphereAttack3::Exit(CPlayer* player)
{
}

PSphereAttack4* PSphereAttack4::Instance()
{
	static PSphereAttack4 instance;
	return &instance;
}

void PSphereAttack4::Enter(CPlayer* player)
{
	player->set_is_move_allow(false);
	player->set_animation_state(PlayerAnimationState::SphereAttack40);
	player->animation_controller()->SetTrackSpeed((int)PlayerAnimationState::SphereAttack40, 0.1);
	attack_time_ = 0.f;
	obb_triger_ = false;
}

void PSphereAttack4::Execute(CPlayer* player, float elapsed_time)
{
	attack_time_ += elapsed_time;

	for (int i = 0; i < move_end_time_.size(); ++i)
	{
		if (move_start_time_[i] < attack_time_ && attack_time_ < move_end_time_[i])
		{
			player->movement_component()->set_direction_vector(player->look_vector());
			player->movement_component()->set_max_speed(move_speed_[i]);
			if (!obb_triger_)
			{
				player->OnWeaponObb();
				player->ClearDamagedObjectList();
				player->SetWeaponDamageScale(damage_scale_);
				obb_triger_ = true;

			}
			break;
		}
		else
		{
			player->movement_component()->set_direction_vector(0, 0, 0);
			player->movement_component()->set_max_speed(0);
		}
	}

	if (!player->animation_controller()->IsEnableTrack((int)player->animation_state()))
	{
		player->state_machine()->ChangeState(PIdle::Instance());
	}
}

void PSphereAttack4::Exit(CPlayer* player)
{
	player->ChangeWeapon();
}
