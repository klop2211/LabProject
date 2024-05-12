#pragma once

#include "Object.h"


class CWeapon : public CGameObject
{
protected:
	WeaponType type_ = WeaponType::None;

	std::string name_ = "None";

public:
	CWeapon(const CModelInfo& model);

	//setter
	void set_name(const std::string& value) { name_ = value; }

	//getter
	std::string name() const { return name_; }
	WeaponType type() const { return type_; }

};