#pragma once

#include "Object.h"

class CPlayer;

class CWeapon : public CRootObject
{
protected:
	CPlayer* owner_ = NULL;

	WeaponType type_ = WeaponType::None;

	std::string name_ = "None";

	// 무기의 공격력
	float atk_ = 0.f;

public:
	CWeapon(const CModelInfo& model);
	CWeapon(const CWeapon& other);

	//setter
	void set_owner(CPlayer* value) { owner_ = value; }
	void set_atk(const float& value) { atk_ = value; }
	void set_name(const std::string& value) { name_ = value; }

	//getter
	std::string name() const { return name_; }
	WeaponType type() const { return type_; }

	virtual void HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb);

};