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
	float atk_ = 5.f;

	// 공격의 데미지 배율 atk * damage_scale == damage_amount
	// 각 공격이 실행될 때 set하여 사용
	float damage_scale_ = 1.f;

	// 현재 공격에서 데미지를 준 객체
	std::list<CRootObject*> damaged_object_list_;

public:
	CWeapon(const CModelInfo& model);
	CWeapon(const CWeapon& other);

	//setter
	void set_damage_scale(const float& value) { damage_scale_ = value; }
	void set_owner(CPlayer* value) { owner_ = value; }
	void set_atk(const float& value) { atk_ = value; }
	void set_name(const std::string& value) { name_ = value; }

	//getter
	std::string name() const { return name_; }
	WeaponType type() const { return type_; }

	virtual void HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb);

	void AddDamagedObject(CRootObject* object) { damaged_object_list_.push_back(object); }
	void ClearDamagedObjectList() { damaged_object_list_.clear(); }
};