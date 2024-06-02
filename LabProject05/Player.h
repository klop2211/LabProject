#pragma once
#include "StateMachine.h"
#include "Object.h"

class CMovementComponent;
class CRotationComponent;
class CObbComponent;
class CAnimationCallbackFunc;
class CWeapon;
class CShader;

enum class PlayerAnimationState { Idle = 0, Roll, Run, Walk, 
	SwordIdle, SwordAttack11, SwordAttack12, SwordAttack13, SwordAttack21, SwordAttack22, SwordAttack23, SwordAttack30, SwordAttack40,
	SphereIdle, SphereAttack11, SphereAttack12, SphereAttack20, SphereAttack31, SphereAttack32, SphereAttack33, SphereAttack40
};

//플레이어 공격 4가지 None 타입은 공격 상태가 아님을 나타냄
enum class PlayerAttackType { None = 0, LeftAttack, RightAttack, BothAttack, ControlAttack };

class CPlayer : public CRootObject
{
protected:
	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	float						speed_; // 단위: m/s

	bool is_move_allow_ = true;

	XMFLOAT3 direction_vector_{ 0.f,0.f,0.f };

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	CCamera* camera_ = NULL;

	//Components
	CMovementComponent* movement_component_ = NULL;
	CRotationComponent* rotation_component_ = NULL;

	bool orient_rotation_to_movement_ = true;

	PlayerAnimationState animation_state_ = PlayerAnimationState::Idle;

	// 현재 플레이어의 무기상태를 나타냄 None은 납도
	WeaponType current_weapon_type_ = WeaponType::None;

	//플레이어의 공격속성 무, 불, 물, 땅, 번개
	DamageType damage_type_ = DamageType::None;

	// 플레이어 무기 소켓(현재 플레이어가 공격할 때 사용하는 무기)
	CGameObject* weapon_socket_ = NULL;

	// 플레이어가 장착 가능한 무기 슬롯
	std::array<CWeapon*, 2> weapon_slot_ = { NULL, NULL };

	// 사용중인 무기슬롯 인덱스
	int current_weapon_slot_index_ = 0;

	//에테르 해방 공격 관련
	bool is_ether_ = false;
	std::array<CGameObject*, 4> ether_weapon_sockets_;

	//무기 데이터 (차후 인벤토리 형식으로 바뀔 예정)
	std::vector<CWeapon*> weapons_;

	PlayerAttackType attack_type_ = PlayerAttackType::None;
	StateMachine<CPlayer>* state_machine_;

public:
	CPlayer();
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual ~CPlayer();

	//setter
	void set_is_ether(const bool& value) { is_ether_ = value; }
	void set_is_move_allow(const bool& value) { is_move_allow_ = value; }
	void set_attack_type(const PlayerAttackType& value) { attack_type_ = value; }
	void set_current_weapon(const WeaponType& value) { current_weapon_type_ = value; }
	void set_animation_state(const PlayerAnimationState& value) { animation_state_ = value; }
	void set_weapon_socket(CGameObject* value) { weapon_socket_ = value; }

	//getter
	DamageType damage_type() const { return damage_type_; }
	bool is_ether() const { return is_ether_; }
	WeaponType current_weapon() const { return current_weapon_type_; }
	CGameObject* weapon_socket() const { return weapon_socket_; }
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

	// 무기 관리
	void AddWeapon(CWeapon* ptr) { weapons_.push_back(ptr); }
	void ChangeWeapon();
	void EquipWeapon(int index);
	void EnableWeapon();
	void DisableWeapon();
	void SetWeaponSlot(CWeapon* weapon, int index) { weapon_slot_[index] = weapon; }
	void OnWeaponObb() { if (weapon_socket_->child()) static_cast<CRootObject*>(weapon_socket_->child())->OnAllObb(); }
	void OffWeaponObb() { if (weapon_socket_->child()) static_cast<CRootObject*>(weapon_socket_->child())->OffAllObb(); }
	void ClearDamagedObjectList();
	void SetWeaponDamageScale(const float& value);

	CCamera *GetCamera() { return(camera_); }
	void SetCamera(CCamera *pCamera) { camera_ = pCamera; }

	virtual void Update(float fTimeElapsed);

	// 시간에 따라 회전
	void OrientRotationToMove(float fTimeElapsed);
	// 남은 회전 각도만큼 즉시 회전
	void OrientRotationToMove();

	virtual void HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb) override;

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

	// 에테르 무기 관련 함수
	void SetEtherWeaponSocketByShader(CShader* shader);
	void SpawnEtherWeapon();
	void DespawnEtherWeapon();
	void UpdateEtherWeapon(float elapsed_time);
};

