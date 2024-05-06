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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer()
{
	camera_ = NULL;

	speed_ = 0.f;

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) : CPlayer()
{
	CGameObject* pGameObject = NULL;

	CModelInfo model = CGameObject::LoadModelInfoFromFile(pd3dDevice, pd3dCommandList, "../Resource/Model/Player_Model.bin");

	set_child(model.heirarchy_root);

	animation_controller_ = model.animation_controller;

	animation_controller_->SetFrameCaches(this);

	animation_controller_->EnableTrack(0);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Roll, AnimationLoopType::Once);

	CMaterial* Material = new CMaterial(1);
	Material->AddTexturePropertyFromDDSFile(pd3dDevice, pd3dCommandList, "../Resource/Model/Texture/uv.png", TextureType::RESOURCE_TEXTURE2D, 7);

	SetMaterial(0, Material);

	speed_ = 550.f;

	movement_component_ = new CMovementComponent((CGameObject*)this);
	rotation_component_ = new CRotationComponent((CGameObject*)this);

	movement_component_->set_max_speed(speed_);

	rotation_component_->set_use_pitch(false);

	axis_transform_matrix_ = new XMFLOAT4X4
	(1, 0, 0, 0,
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
	animation_state_ = PlayerAnimationState::Roll;
}

void CPlayer::Update(float elapsed_time)
{
	if (movement_component_)
		movement_component_->Update(elapsed_time);

	if (orient_rotation_to_movement_ && !IsZero(movement_component_->speed()))
	{
		// 벡터의 삼중적을 활용한 최단 방향 회전 
		XMFLOAT3 v = look_vector(), d = Vector3::Normalize(movement_component_->velocity_vector()), u = XMFLOAT3(0.f,1.f,0.f);
		float result = Vector3::DotProduct(u, Vector3::CrossProduct(d, v));

		float yaw = Vector3::Angle(v, d);
		if (result > 0)
			yaw *= -1;
		if(!IsZero(yaw))
			rotation_component_->Rotate(0.f, yaw * 6.f * elapsed_time, 0.f);
	}

	if (rotation_component_)
		rotation_component_->Update(elapsed_time);

	animation_controller_->ChangeAnimation((int)animation_state_);

	UpdateAnimationState();


}

void CPlayer::UpdateAnimationState()
{
	if (animation_state_ == PlayerAnimationState::Roll)
	{
		if(!animation_controller_->IsEnableTrack((int)PlayerAnimationState::Roll))
			animation_state_ = PlayerAnimationState::Idle;
	}


	if (animation_state_ != PlayerAnimationState::Roll)
	{
		if (IsZero(Vector3::Length(movement_component_->direction_vector())))
			animation_state_ = PlayerAnimationState::Idle;
		else
			animation_state_ = PlayerAnimationState::Run;
	}

}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (pCamera->GetMode() != CameraMode::FIRST_PERSON)
	{
		CGameObject::Render(pd3dCommandList);
	}
}

void CPlayer::SetAnimationCallbackKey(const float& index, const float& time, CAnimationCallbackFunc* func)
{
	animation_controller_->SetCallbackKey(index, time, func);
}

