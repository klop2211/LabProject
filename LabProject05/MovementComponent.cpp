#include "stdafx.h"
#include "MovementComponent.h"
#include "Object.h"

CMovementComponent::CMovementComponent(CGameObject* owner, const XMFLOAT3& direction, const float& velocity) 
	: CComponent(owner), direction_vector_(direction), velocity_(velocity)
{

}

void CMovementComponent::Update(const float& fElapsedTime)
{
	direction_vector_ = Vector3::Normalize(direction_vector_);
	XMFLOAT3 xmf3NewPosition = Vector3::Add(owner_->position_vector(), direction_vector_, fElapsedTime * velocity_);

	if (m_bGravity)
	{
		//TODO: 중력 구현하기
		//float tt = fElapsedTime * fElapsedTime;
		//float temp = fElapsedTime * m_fGravityVelocity + (float)0.5 * m_fGravityAcceleration * tt;
		//xmf3NewPosition = Vector3::Add(xmf3NewPosition, m_cxmf3GravityDirection, temp);
		//m_fGravityVelocity += fElapsedTime * m_fGravityAcceleration;
	}

	owner_->set_position_vector(xmf3NewPosition);
}
