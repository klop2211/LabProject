#include "stdafx.h"
#include "Weapon.h"
#include "Player.h"
#include "ObbComponent.h"

CWeapon::CWeapon(const CModelInfo& model) : CRootObject(model)
{
	is_visible_ = false;

	collision_type_ = CollisionType::Dynamic;
}

CWeapon::CWeapon(const CWeapon& other) : CRootObject(other), type_(other.type_), name_(other.name_), owner_(other.owner_)
{

}

void CWeapon::HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb)
{
	for (auto& p : damaged_object_list_)
	{
		if (p == other) return;
	}

	CDamageEvent damage_event{ DamageType::None, other_obb.name()};
	other->TakeDamage(atk_ * damage_scale_, damage_event, owner_, this);
	AddDamagedObject(other);

	if (owner_->is_ether())
	{
		owner_->FireEtherWeapon(other, other_obb.animated_obb().Center);
	}
}

void CWeapon::ChangeObbParent(CGameObject* parent)
{
	for (auto& p : obb_list_)
	{
		p->set_parent_socket(parent);
	}
}

CEtherWeapon::CEtherWeapon(const CWeapon& other) : CWeapon(other)
{
	damage_scale_ = 0.1;
}

void CEtherWeapon::Update(float elapsed_time)
{
	if (!parent_)
	{
		//TODO: 무기를 타겟 방향으로 회전시키고 시간에 따라 움직이도록 변경 필요
		set_position_vector(target_position_);
	}
}

void CEtherWeapon::HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb)
{

}
