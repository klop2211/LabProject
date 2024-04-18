#pragma once
#include "Component.h"

//TODO: 구글 명명법으로 작업 시작
class CRotationComponent : public CComponent
{
	bool orient_rotation_to_movement_ = false;

	bool use_pitch_ = true;
	bool use_yaw_ = true;
	bool use_roll_ = true;

	float pitch_ = 0.f;
	float yaw_ = 0.f;
	float roll_ = 0.f;

public:
	CRotationComponent() {}
	CRotationComponent(CGameObject* owner) : CComponent(owner) {}

	void set_use_pitch(const bool& value) { use_pitch_ = value; }
	void set_use_yaw(const bool& value) { use_yaw_ = value; }
	void set_use_roll(const bool& value) { use_roll_ = value; }

	void Rotate(const float& pitch, const float& yaw, const float& roll);

	virtual void Update(const float& elapsed_time);
};

