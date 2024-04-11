#pragma once

class CGameObject;

class CComponent
{
protected:
	CGameObject* m_pOwner = nullptr;

public:
	CComponent() {}
	CComponent(CGameObject* pOwner) { m_pOwner = pOwner; }
	~CComponent() {}
	
	virtual void Update(const float& fElapsedTime) {}
};

