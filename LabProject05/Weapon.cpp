#include "stdafx.h"
#include "Weapon.h"
#include "Player.h"
#include "ObbComponent.h"

CWeapon::CWeapon(const CModelInfo& model) : CRootObject(model)
{
	is_visible_ = false;
}

CWeapon::CWeapon(const CWeapon& other) : CRootObject(other), type_(other.type_), name_(other.name_)
{

}

void CWeapon::HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb)
{
	CDamageEvent damage_event{ DamageType::None, other_obb.name()};
	other->TakeDamage(atk_, damage_event, owner_, this);
}
