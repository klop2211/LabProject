#pragma once

#include "Object.h"


struct CB_PLAYER_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
};

class CPlayer : public CGameObject
{
protected:
	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	bool						m_bRotate = true;
	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	float						m_fSpeed; // ¥‹¿ß: m/s

	XMFLOAT3					m_xmf3Velocity;
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	CCamera* m_pCamera = NULL;

public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	virtual void SetPosition(const float& x, const float& y, const float& z) { SetPosition(XMFLOAT3(x, y, z)); }
	virtual void SetPosition(const XMFLOAT3& xmf3Position) { m_xmf3Position = xmf3Position; }

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	void AddVelocity(const DWORD& dwDirection, const float& fElapsedTime);

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	bool IsRotate() const { return m_bRotate; }
	void OnRotate() { m_bRotate = true; }
	void OffRotate() { m_bRotate = false; }
	virtual void Rotate(const float& fPitch, const float& fYaw, const float& fRoll);

	void Move(float fTimeElapsed);

	void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

private:
	void UpdateVelocity();

};

class CEllenPlayer : public CPlayer
{
public:
	CEllenPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void OnPrepareRender();
};


