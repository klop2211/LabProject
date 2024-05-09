#pragma once
#include "StateMachine.h"
#include "Object.h"

class CMovementComponent;
class CRotationComponent;
class CAnimationCallbackFunc;

enum class PlayerAnimationState { Idle = 0, Roll, Run, Walk, SwordIdle, SwordAttack1, SwordAttack2, SwordAttack3, SwordAttack4 };
enum class PlayerWeaponType { Sword = 0, Sphere };
enum class PlayerAttackType { None = 0, LeftAttack, RightAttack, BothAttack, ControlAttack };

class CPlayer : public CGameObject
{
protected:
	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	float						speed_; // 단위: m/s

	XMFLOAT3 direction_vector_{ 0.f,0.f,0.f };

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	CCamera* camera_ = NULL;

	//Components
	CMovementComponent* movement_component_ = NULL;
	CRotationComponent* rotation_component_ = NULL;

	bool orient_rotation_to_movement_ = true;

	PlayerAnimationState animation_state_ = PlayerAnimationState::Idle;

	PlayerWeaponType current_weapon_ = PlayerWeaponType::Sword;
	PlayerWeaponType other_weapon_ = PlayerWeaponType::Sphere;
	PlayerAttackType attack_type_ = PlayerAttackType::None;

	StateMachine<CPlayer>* state_machine_;

public:
	CPlayer();
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual ~CPlayer();

	//setter
	void set_animation_state(const PlayerAnimationState& value) { animation_state_ = value; }

	//getter
	StateMachine<CPlayer>* state_machine()const { return state_machine_; }
	CAnimationController* animation_controller() const { return animation_controller_; }
	float speed() const { return speed_; }
	CMovementComponent* movement_component() const { return movement_component_; }
	bool orient_rotation_to_movement() const { return orient_rotation_to_movement_; }
	PlayerAnimationState animation_state() const { return animation_state_; }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	// 입력 처리
	void InputActionMove(const DWORD& dwDirection, const float& elapsed_time);
	void InputActionRotate(const XMFLOAT2& delta_xy, const float& elapsed_time);
	void InputActionRoll(const DWORD& direction);
	void InputActionAttack(const PlayerAttackType& attack_type);

	CCamera *GetCamera() { return(camera_); }
	void SetCamera(CCamera *pCamera) { camera_ = pCamera; }

	virtual void Update(float fTimeElapsed);

	void OrientRotationToMove(float fTimeElapsed);

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

	void SetAnimationCallbackKey(const float& index, const float& time, CAnimationCallbackFunc* func);
};

