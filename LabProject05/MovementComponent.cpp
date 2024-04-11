#include "stdafx.h"
#include "MovementComponent.h"
#include "Object.h"

CMovementComponent::CMovementComponent(CGameObject* pOwner, const XMFLOAT3& xmf3Direction, const float& fVelocity) 
	: CComponent(pOwner), m_xmf3Direction(xmf3Direction), m_fVelocity(fVelocity)
{

}

void CMovementComponent::Update(const float& fElapsedTime)
{

	Vector3::Normalize(m_xmf3Direction);
	XMFLOAT3 xmf3NewPosition = Vector3::Add(m_pOwner->GetPosition(), m_xmf3Direction, fElapsedTime * m_fVelocity);

	if (m_bGravity)
	{
		//float tt = fElapsedTime * fElapsedTime;
		//float temp = fElapsedTime * m_fGravityVelocity + (float)0.5 * m_fGravityAcceleration * tt;
		//xmf3NewPosition = Vector3::Add(xmf3NewPosition, m_cxmf3GravityDirection, temp);
		//m_fGravityVelocity += fElapsedTime * m_fGravityAcceleration;
	}

	m_pOwner->SetPosition(xmf3NewPosition);
}
