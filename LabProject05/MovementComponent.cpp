#include "stdafx.h"
#include "MovementComponent.h"
#include "Object.h"

CMovementComponent::CMovementComponent(CGameObject* owner, const XMFLOAT3& direction, const XMFLOAT3& velocity)
	: CComponent(owner), direction_vector_(direction), velocity_vector_(velocity_vector_)
{

}

void CMovementComponent::Update(const float& elapsed_time)
{
	if (Vector3::Length(direction_vector_))
		is_friction_ = false;
	else
		is_friction_ = true;
		
	direction_vector_ = Vector3::Normalize(direction_vector_);

	// v = v0 + a * t - f * t
	velocity_vector_ = velocity_vector_ + (direction_vector_ * (acceleration_ * elapsed_time)); 
	if (is_friction_)
	{
		if (friction_ * elapsed_time > Vector3::Length(velocity_vector_))
			velocity_vector_ = XMFLOAT3(0, 0, 0);
		else
		{
			XMFLOAT3 friction_vector = Vector3::Normalize(velocity_vector_ * -1) * (friction_ * elapsed_time);
			velocity_vector_ = velocity_vector_ + friction_vector;
		}
	}

	float speed = Vector3::Length(velocity_vector_);
	if (speed > max_speed_)
		velocity_vector_ = Vector3::Normalize(velocity_vector_) * max_speed_;
	XMFLOAT3 xmf3NewPosition = Vector3::Add(owner_->position_vector(), velocity_vector_ * elapsed_time);

	if (owner_->is_fall())
	{
		gravity_velocity_ += gravity_acceleration_ * elapsed_time;

		if (gravity_velocity_ > 5300.f) // ���� �ӵ�
			gravity_velocity_ = 5300.f;
		xmf3NewPosition = Vector3::Add(xmf3NewPosition, m_cxmf3GravityDirection, gravity_velocity_);
	}
	else
		gravity_velocity_ = 0.f;

	owner_->set_position_vector(xmf3NewPosition);
}
