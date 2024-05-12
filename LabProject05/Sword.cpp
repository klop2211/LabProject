#include "stdafx.h"
#include "Sword.h"

CSword::CSword(const CModelInfo& model) : CWeapon(model)
{
	type_ = WeaponType::Sword;
}
