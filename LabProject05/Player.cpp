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

	//TODO: ������ ����ϴ� ��� ������ ������ŭ ��ƾ���
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

	// ������� ������ ������..
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

	// MoveInput ����ó��
	if (!is_move_allow_)
		return;

	XMFLOAT3 direction_vector = XMFLOAT3(0.f, 0.f, 0.f);
	if (dwDirection)
	{


		//CS_MOVE_PACKET p;
		//p.size = sizeof(p);
		//p.type = CS_MOVE;
		//// TODO: ���� yaw�� float���� ����. �ش� yaw�� ���� ��� ã��
		//p.camera_yaw = (camera_->GetYaw());

		//p.player_yaw = (rotation_component_->yaw());

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

	// �÷��̾��� default ȸ���� roll ȸ���� ���� ����
	if (rotation_component_)
		rotation_component_->Rotate(delta_xy.y, delta_xy.x, 0.f);
}

void CPlayer::InputActionRoll(const DWORD& direction)
{
	if (state_machine_->isInState(*PIdle::Instance()) || state_machine_->isInState(*PMove::Instance()))
	{
		OrientRotationToMove();
		state_machine_->ChangeState(PEvade::Instance());	
	}
}

void CPlayer::InputActionAttack(const PlayerAttackType& attack_type)
{
	// �÷��̾ ���� ���̰� ���ο� �Է��� ��ü������ �ƴ϶�� ������ �����ϸ� �ȵ�(��ü������ ĵ�� ������)
	if (attack_type_ != PlayerAttackType::None && attack_type != PlayerAttackType::ControlAttack)
		return;
	// �̹� �ش� ������ ������
	if (attack_type_ == attack_type)
		return;
	if (state_machine_->isInState(*PEvade::Instance()))
		return;

	OrientRotationToMove();

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

	UpdateEtherWeapon(elapsed_time);
}

void CPlayer::HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb)
{
	//�����̵� ���� S = P - n(P*n), P: �Ի纤��(���⼭�� this�� �̵��� ����), n: ��麤��(���⼭�� other���� this�� �ٶ󺸴� ���͸� ����Ͽ� ���� �ε��� ����� ����)
	XMFLOAT3 P = position_vector() - movement_component_->prev_position_vector();
	XMFLOAT3 other_to_this = Vector3::Normalize(position_vector() - other->position_vector());
	XMFLOAT3 other_look = Vector3::Normalize(other->look_vector());
	XMFLOAT3 other_right = Vector3::Normalize(other->right_vector());
	XMFLOAT3 rect_extants(other_obb.animated_obb().Extents.x, 0, other_obb.animated_obb().Extents.y);
	XMFLOAT3 x_axis = XMFLOAT3(1, 0, 0);
	float angle = abs(Vector3::Angle(Vector3::Normalize(rect_extants), x_axis));
	XMFLOAT3 n;
	if (abs(Vector3::Angle(other_to_this, other_look)) < 90.f - angle) n = Vector3::Normalize(other_look);
	if (abs(Vector3::Angle(other_to_this, other_look * -1)) < 90.f - angle) n = Vector3::Normalize(other_look * -1);
	if (abs(Vector3::Angle(other_to_this, other_right)) < angle) n = Vector3::Normalize(other_right);
	if (abs(Vector3::Angle(other_to_this, other_right * -1)) < angle) n = Vector3::Normalize(other_right * -1);

	XMFLOAT3 S = P - (n * (Vector3::DotProduct(P, n)));

	//�浹�� ��ġ���� �̵��� ��ġ�� �����̵� ���͸�ŭ �̵�
	set_position_vector(movement_component_->prev_position_vector() + S);
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
		p->ResetChild(new CWeapon(*(CWeapon*)weapon));
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

// ���� ���� YAW�� �̿��ؼ� ���� �ڵ� ���� �ʿ�
void CPlayer::OrientRotationToMove(float elapsed_time)
{
	if (!IsZero(g_objects[g_myid].yaw))
	{
		rotation_component_->Rotate(0.f, g_objects[g_myid].yaw, 0.f);
	}
}

void CPlayer::OrientRotationToMove()
{
	// TODO : �ڷ� ���鼭 ��ų ���� �������� �߰� �۾� �ʿ�
	if (!IsZero(g_objects[g_myid].yaw))
	{
		rotation_component_->Rotate(0.f, g_objects[g_myid].yaw, 0.f);
	}
}

void CPlayer::OrientRotationAnothers(int another_num)
{
	// TODO : �ڷ� ���鼭 ��ų ���� �������� �߰� �۾� �ʿ�
	if (!IsZero(g_objects[another_num].yaw))
	{
		Rotate(0.f, g_objects[another_num].yaw, 0.f);
	}
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

void CPlayer::SendSkill(bool skillend)
{
	CS_SKILL_PACKET p;
	p.size = sizeof(p);
	p.type = CS_SKILL;
	p.Weapon_N_Attack = (static_cast<uint8_t>(current_weapon_type_) << 4)
						| static_cast<uint8_t>(attack_type_);
	p.Animation = (static_cast<uint8_t>(animation_state_) << 1) | skillend;
	DoSend(&p);
}

