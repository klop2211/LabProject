#pragma once

#include "Object.h"

class CPlayer;

class CWeapon : public CRootObject
{
protected:
	CPlayer* owner_ = NULL;

	WeaponType type_ = WeaponType::None;

	std::string name_ = "None";

	// ������ ���ݷ�
	float atk_ = 5.f;

	// ������ ������ ���� atk * damage_scale == damage_amount
	// �� ������ ����� �� set�Ͽ� ���
	float damage_scale_ = 1.f;

	// ���� ���ݿ��� �������� �� ��ü
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