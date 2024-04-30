#include "stdafx.h"
#include "MovementComponent.h"
#include "Object.h"

CMovementComponent::CMovementComponent(CGameObject* owner, const XMFLOAT3& direction, const float& velocity) 
	: CComponent(owner), direction_vector_(direction), velocity_(velocity)
{

}

void CMovementComponent::Update(const float& elapsed_time)
{
	direction_vector_ = Vector3::Normalize(direction_vector_);
	XMFLOAT3 xmf3NewPosition = Vector3::Add(owner_->position_vector(), direction_vector_, elapsed_time * velocity_);

	if (owner_->is_fall())
	{
		gravity_velocity_ += gravity_acceleration_ * elapsed_time;

		if (gravity_velocity_ > 5300.f) // 종단 속도
			gravity_velocity_ = 5300.f;
		xmf3NewPosition = Vector3::Add(xmf3NewPosition, m_cxmf3GravityDirection, gravity_velocity_);
	}
	else
		gravity_velocity_ = 0.f;

	owner_->set_position_vector(xmf3NewPosition);
}
