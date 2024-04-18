#pragma once

#include "Object.h"

class CMovementComponent;
class CRotationComponent;
class CCharacterMovementComponent;

class CPlayer : public CGameObject
{
protected:
	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	float						m_fSpeed; // ����: m/s

	XMFLOAT3					m_xmf3Velocity;
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	CCamera* camera_ = NULL;

	//Components
	CCharacterMovementComponent* character_movement_component_ = NULL;
	CMovementComponent* movement_component_ = NULL;
	CRotationComponent* rotation_component_ = NULL;

public:
	CPlayer();
	virtual ~CPlayer();

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	void InputActionMove(const DWORD& dwDirection, const float& elapsed_time);
	void InputActionRotate(const XMFLOAT2& delta_xy, const float& elapsed_time);

	CCamera *GetCamera() { return(camera_); }
	void SetCamera(CCamera *pCamera) { camera_ = pCamera; }

	void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender() {}
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

};

class CEllenPlayer : public CPlayer
{
public:
	CEllenPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};


