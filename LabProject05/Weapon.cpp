#include "stdafx.h"
#include "Weapon.h"

CWeapon::CWeapon(const CModelInfo& model)
{
	set_child(model.heirarchy_root);
}
