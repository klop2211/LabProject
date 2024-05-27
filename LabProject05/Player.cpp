//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "Animation.h"
#include "MovementComponent.h"
#include "RotationComponent.h"
#include "Camera.h"
#include "Mesh.h"
#include "AnimationCallbackFunc.h"
#include "Material.h"
#include "PlayerState.h"
#include "Weapon.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer()
{
	camera_ = NULL;

	speed_ = 0.f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

	state_machine_ = new StateMachine<CPlayer>(this);
	state_machine_->SetCurrentState(PIdle::Instance());
	state_machine_->SetPreviousState(PIdle::Instance());

	//TODO: 씬에서 사용하는 모든 무기의 개수만큼 잡아야함
	weapons_.reserve(10);
}

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) : CPlayer()
{
	CGameObject* pGameObject = NULL;

	CModelInfo model = CGameObject::LoadModelInfoFromFile(pd3dDevice, pd3dCommandList, "../Resource/Model/Player_Model.bin");

	set_child(model.heirarchy_root);

	animation_controller_ = new CAnimationController(*model.animation_controller);

	animation_controller_->SetFrameCaches(this);

	animation_controller_->EnableTrack(0);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Idle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::SwordIdle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::SphereIdle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Walk, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Run, AnimationLoopType::Repeat);

	CMaterial* Material = new CMaterial(1);
	Material->AddTexturePropertyFromDDSFile(pd3dDevice, pd3dCommandList, "../Resource/Model/Texture/uv.png", TextureType::RESOURCE_TEXTURE2D, 7);

	SetMaterial(0, Material);

	speed_ = 550.f;

	movement_component_ = new CMovementComponent((CGameObject*)this);
	rotation_component_ = new CRotationComponent((CGameObject*)this);

	movement_component_->set_max_speed(speed_);

	rotation_component_->set_use_pitch(false);

	axis_transform_matrix_ = new XMFLOAT4X4
	(	1, 0, 0, 0,
		0, 0, -1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1);

	SetCamera(pCamera);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetShader((int)ShaderNum::Standard);
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	//if (camera_) delete camera_;
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (camera_) camera_->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (camera_) camera_->UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	if (camera_) camera_->ReleaseShaderVariables();
}

void CPlayer::InputActionMove(const DWORD& dwDirection, const float& fElapsedTime)
{
	if (camera_->GetMode() == CameraMode::GHOST)
		return;

	// MoveInput 예외처리
	if (!is_move_allow_)
		return;

	XMFLOAT3 direction_vector = XMFLOAT3(0.f, 0.f, 0.f);
	if (dwDirection)
	{


		//CS_MOVE_PACKET p;
		//p.size = sizeof(p);
		//p.type = CS_MOVE;
		//// TODO: 현재 yaw를 float으로 보냄. 해당 yaw를 줄일 방법 찾기
		//p.camera_yaw = (camera_->GetYaw());

		//p.player_yaw = (rotation_component_->yaw());

		//// TODO: 현재 direction를 float으로 보냄. 해당 direction를 줄일 방법 찾기
		////p.yaw = (camera_->GetYaw());
		////p.direction = static_cast<unsigned char>(dwDirection);
		//p.direction = (dwDirection);
		//DoSend(&p);
	}
	//if (movement_component_)
	//{
	//	movement_component_->set_direction_vector(direction_vector);
	//}
}

void CPlayer::InputActionRotate(const XMFLOAT2& delta_xy, const float& elapsed_time)
{
	if (camera_) camera_->Rotate(delta_xy.y, delta_xy.x, 0.f);

	if (camera_->GetMode() == CameraMode::GHOST) return;

	if (orient_rotation_to_movement_) return;

	// 플레이어의 default 회전은 roll 회전을 하지 않음
	if (rotation_component_)
		rotation_component_->Rotate(delta_xy.y, delta_xy.x, 0.f);
}

void CPlayer::InputActionRoll(const DWORD& direction)
{
	if(state_machine_->isInState(*PIdle::Instance()) || state_machine_->isInState(*PMove::Instance()))
		state_machine_->ChangeState(PEvade::Instance());	
}

void CPlayer::InputActionAttack(const PlayerAttackType& attack_type)
{
	// 플레이어가 공격 중이고 새로운 입력이 교체공격이 아니라면 공격을 변경하면 안됨(교체공격은 캔슬 공격임)
	if (attack_type_ != PlayerAttackType::None && attack_type != PlayerAttackType::ControlAttack)
		return;
	// 이미 해당 공격을 실행중
	if (attack_type_ == attack_type)
		return;
	attack_type_ = attack_type;


	if (current_weapon_type_ == WeaponType::None)
	{
		current_weapon_type_ = ((CWeapon*)weapon_socket_->child())->type();
		weapon_socket_->set_is_visible(true);
	}

	State<CPlayer>* attack_state = NULL;
	switch (attack_type_)
	{
	case PlayerAttackType::None:
		break;
	case PlayerAttackType::LeftAttack:
		attack_state = PAttack1::Instance(current_weapon_type_);
		break;
	case PlayerAttackType::RightAttack:
		attack_state = PAttack2::Instance(current_weapon_type_);
		break;
	case PlayerAttackType::BothAttack:
		attack_state = PAttack3::Instance(current_weapon_type_);
		break;
	case PlayerAttackType::ControlAttack:
		attack_state = PAttack4::Instance(current_weapon_type_);
		break;
	default:
		break;
	}
	state_machine_->ChangeState(attack_state);
}

void CPlayer::Update(float elapsed_time)
{
	animation_controller_->ChangeAnimation((int)animation_state_);

	state_machine_->Update(elapsed_time);

	if (movement_component_)
	{
		movement_component_->Update(elapsed_time); 
	}
	

	if (rotation_component_)
		rotation_component_->Update(elapsed_time);
}

void CPlayer::OrientRotationToMove(float elapsed_time)
{

	if (!IsZero(g_objects[g_myid].yaw))
	{
		rotation_component_->Rotate(0.f, g_objects[g_myid].yaw, 0.f);
	}
}

void CPlayer::EquipWeapon(const std::string& name)
{
	CGameObject* weapon = NULL;
	for (const auto& p : weapons_)
		if (p->name() == name)
		{
			weapon = p;
			break;
		}
	if (!weapon) 
		return;
	weapon_socket_->ResetChild(weapon);
	current_weapon_type_ = ((CWeapon*)weapon)->type();
	weapon_socket_->SetShader(weapon->shader_num());
}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (pCamera->GetMode() != CameraMode::FIRST_PERSON)
	{
		CGameObject::Render(pd3dCommandList, pCamera, shader_num_);
	}
}

void CPlayer::SetAnimationCallbackKey(const float& index, const float& time, CAnimationCallbackFunc* func)
{
	animation_controller_->SetCallbackKey(index, time, func);
}

void CPlayer::SendInput(uint8_t& input)
{
	CS_MOVE_PACKET p;
	p.size = sizeof(p);
	p.type = CS_MOVE;
	p.camera_yaw = static_cast<short>(camera_->GetYaw());
	p.player_yaw = static_cast<short>(rotation_component_->yaw());
	p.keyinput = input;
	DoSend(&p);
}

