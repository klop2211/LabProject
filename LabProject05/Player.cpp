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
#include "ObbComponent.h"

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

CPlayer::CPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, CCamera* pCamera) : CPlayer()
{
	CGameObject* pGameObject = NULL;

	CModelInfo model = CGameObject::LoadModelInfoFromFile(device, command_list, "../Resource/Model/Player_Model.bin");

	set_child(model.heirarchy_root);
	for (auto& p : ether_weapon_sockets_)
	{
		p = AddSocket("Bip001");
	} 
	ether_weapon_sockets_[0]->set_position_vector(0, -150.f, 0);
	ether_weapon_sockets_[1]->set_position_vector(0, -100.f, 100);
	ether_weapon_sockets_[2]->set_position_vector(0, 150.f, 0);
	ether_weapon_sockets_[3]->set_position_vector(0, 100.f, 100.f);

	CreateBoneTransformMatrix(device, command_list);

	animation_controller_ = new CAnimationController(*model.animation_controller);

	animation_controller_->SetFrameCaches(this);

	animation_controller_->EnableTrack(0);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Idle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::SwordIdle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::SphereIdle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Walk, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Run, AnimationLoopType::Repeat);

	CMaterial* Material = new CMaterial(1);
	Material->AddTexturePropertyFromDDSFile(device, command_list, "../Resource/Model/Texture/uv.png", TextureType::RESOURCE_TEXTURE2D, 7);

	model.heirarchy_root->SetMaterial(0, Material);

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

	CreateShaderVariables(device, command_list);

	SetShader((int)ShaderNum::Standard);
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
	weapon_socket_->ResetChild(NULL);

	// 무기들의 삭제는 씬에서..
	//for (auto& p : weapons_)
	//	delete p;

	//if (camera_) delete camera_;
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (camera_) camera_->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	CRootObject::UpdateShaderVariables(pd3dCommandList);
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
		if (orient_rotation_to_movement_)
		{
			// 카메라의 yaw 회전만 가져와서 사용
			float yaw = camera_->GetYaw();
			XMMATRIX R = XMMatrixRotationRollPitchYaw(0.f, XMConvertToRadians(yaw), 0.f);
			XMFLOAT3 look = XMFLOAT3(0.f, 0.f, 1.f), up = XMFLOAT3(0.f, 1.f, 0.f);
			XMStoreFloat3(&look, XMVector3TransformCoord(XMLoadFloat3(&look), R));
			XMFLOAT3 right = Vector3::CrossProduct(up, look);

			if (dwDirection & DIR_FORWARD) direction_vector = Vector3::Add(direction_vector, look);
			if (dwDirection & DIR_BACKWARD) direction_vector = Vector3::Add(direction_vector, look, -1.f);
			if (dwDirection & DIR_LEFT) direction_vector = Vector3::Add(direction_vector, right, -1.f);
			if (dwDirection & DIR_RIGHT) direction_vector = Vector3::Add(direction_vector, right);
			direction_vector_ = direction_vector;
		}
		else
		{
			XMFLOAT3 look = look_vector(), right = right_vector();
			if (dwDirection & DIR_FORWARD) direction_vector = Vector3::Add(direction_vector, look);
			if (dwDirection & DIR_BACKWARD) direction_vector = Vector3::Add(direction_vector, look, -1.f);
			if (dwDirection & DIR_LEFT) direction_vector = Vector3::Add(direction_vector, right, -1.f);
			if (dwDirection & DIR_RIGHT) direction_vector = Vector3::Add(direction_vector, right);
		}
	}
	if (movement_component_)
	{
		movement_component_->set_direction_vector(direction_vector);
	}
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
		movement_component_->Update(elapsed_time);

	if (rotation_component_)
		rotation_component_->Update(elapsed_time);

	UpdateEtherWeapon(elapsed_time);
}

void CPlayer::HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb)
{
	//슬라이딩 벡터 S = P - n(P*n), P: 입사벡터(여기서는 this가 이동한 벡터), n: 평면벡터(여기서는 other에서 this를 바라보는 벡터(normalize된))
	XMFLOAT3 P = position_vector() - movement_component_->prev_position_vector();
	XMFLOAT3 other_to_this = Vector3::Normalize(position_vector() - other->position_vector());
	//XMFLOAT3 this_rect_extants = my_obb.animated_obb().Extents;
	//XMFLOAT3 other_to_LT = position_vector() + XMFLOAT3(-this_rect_extants.x, 0, this_rect_extants.y) - other->position_vector();
	//XMFLOAT3 other_to_RT = position_vector() + XMFLOAT3(this_rect_extants.x, 0, this_rect_extants.y)  - other->position_vector();
	//std::array<XMFLOAT3, 2> other_to_points{ other_to_LT, other_to_RT };
	//float min_length = INFINITE;
	//XMFLOAT3 min_vector;
	//for (auto& length : other_to_points)
	//{
	//	if (min_length > Vector3::Length(length))
	//	{
	//		min_length = Vector3::Length(length);
	//		min_vector = length;
	//	}
	//}
	//min_vector = Vector3::Normalize(min_vector);
	XMFLOAT3 other_look = other->look_vector();
	XMFLOAT3 other_right = other->right_vector();
	XMFLOAT3 rect_extants(other_obb.animated_obb().Extents.x, 0, other_obb.animated_obb().Extents.y);
	XMFLOAT3 x_axis = XMFLOAT3(1, 0, 0);
	float angle = abs(Vector3::Angle(Vector3::Normalize(rect_extants), x_axis));
	XMFLOAT3 n;
	if (abs(Vector3::Angle(other_to_this, other_look)) < 90.f - angle) n = Vector3::Normalize(other_look);
	if (abs(Vector3::Angle(other_to_this, other_look * -1)) < 90.f - angle) n = Vector3::Normalize(other_look * -1);
	if (abs(Vector3::Angle(other_to_this, other_right)) < angle) n = Vector3::Normalize(other_right);
	if (abs(Vector3::Angle(other_to_this, other_right * -1)) < angle) n = Vector3::Normalize(other_right * -1);

	XMFLOAT3 S = P - (n * (Vector3::DotProduct(P, n)));

	//충돌전 위치에서 이동한 위치의 슬라이딩 벡터만큼 이동
	set_position_vector(movement_component_->prev_position_vector() + S);
}

void CPlayer::OrientRotationToMove(float elapsed_time)
{
	// 벡터의 삼중적을 활용한 최단 방향 회전 d = Vector3::Normalize(movement_component_->velocity_vector())
	XMFLOAT3 v = look_vector(), d = Vector3::Normalize(direction_vector_), u = XMFLOAT3(0.f, 1.f, 0.f);
	float result = Vector3::DotProduct(u, Vector3::CrossProduct(d, v));

	float yaw = Vector3::Angle(v, d);
	if (result > 0)
		yaw *= -1;
	if (!IsZero(yaw))
		rotation_component_->Rotate(0.f, yaw * 12.f * elapsed_time, 0.f);
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
	if (!weapon) return;
	weapon_socket_->set_is_visible(false);
	weapon->set_is_visible(true);
	weapon_socket_->ResetChild(weapon);
	weapon_socket_->set_is_visible(true);
	for (auto& p : ether_weapon_sockets_)
	{
		p->ResetChild(weapon);
		p->SetShader(weapon->shader_num());
		p->set_is_visible(false);
	}
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

void CPlayer::UpdateEtherWeapon(float elapsed_time)
{
	if (is_ether_)
	{
	}
}

void CPlayer::SpawnEtherWeapon()
{
	for (auto& p : ether_weapon_sockets_)
	{
		p->set_is_visible(true);
	}
}

void CPlayer::DespawnEtherWeapon()
{
	for (auto& p : ether_weapon_sockets_)
	{
		p->set_is_visible(false);
	}
}

void CPlayer::SetEtherWeaponSocketByShader(CShader* shader)
{
	for (auto& p : ether_weapon_sockets_)
		shader->AddObject(p);
}

