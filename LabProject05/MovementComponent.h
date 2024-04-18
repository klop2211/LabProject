#pragma once
#include "Component.h"

class CGameObject;

// Owner의 전후좌우 이동을 담당
class CMovementComponent : public CComponent
{
	const XMFLOAT3 m_cxmf3GravityDirection = XMFLOAT3(0.f, -1.0f, 0.f);

	XMFLOAT3 direction_vector_ = XMFLOAT3(0.f, 0.f, 0.f);
	float velocity_ = 100.f;

	bool m_bGravity = false;
	float m_fGravityAcceleration = 980.f;
	float m_fGravityVelocity = 0.f;

public:
	CMovementComponent() {}
	CMovementComponent(CGameObject* pOwner, const XMFLOAT3& xmf3Direction, const float& fVelocity);
	~CMovementComponent() {}

	void set_direction_vector(const float& x, const float& y, const float& z) { set_direction_vector(XMFLOAT3(x, y, z)); }
	void set_direction_vector(const XMFLOAT3& xmf3Value) { direction_vector_ = xmf3Value; }

	void AddVelocity(const float& fValue) { velocity_ += fValue; }
	
	virtual void Update(const float& fElapsedTime);
};

