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

}

void CWeapon::ChangeObbParent(CGameObject* parent)
{
	for (auto& p : obb_list_)
	{
		p->set_parent_socket(parent);
	}
}
