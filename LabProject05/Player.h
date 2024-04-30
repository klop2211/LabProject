#pragma once

#include "Object.h"

class CMovementComponent;
class CRotationComponent;
class CAnimationCallbackFunc;

class CPlayer : public CGameObject
{
protected:
	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	float						speed_; // ¥‹¿ß: m/s

	XMFLOAT3					m_xmf3Velocity;
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	CCamera* camera_ = NULL;

	//Components
	CMovementComponent* movement_component_ = NULL;
	CRotationComponent* rotation_component_ = NULL;

	bool orient_rotation_to_movement_ = true;


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

	virtual void Update(float fTimeElapsed);

	virtual void HandleCollision(CGameObject* other) override {}

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

enum class EllenAnimationState { Idle = 0, Roll, Run, Walk };

class CEllenPlayer : public CPlayer
{
private:
	EllenAnimationState animation_state_ = EllenAnimationState::Idle;

public:
	CEllenPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void Update(float fTimeElapsed);

	void SetAnimationCallbackKey(const float& index, const float& time, CAnimationCallbackFunc* func);
};


