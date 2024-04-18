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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer()
{
	camera_ = NULL;

	m_fSpeed = 0.f;

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

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (camera_) delete camera_;
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
	switch (camera_->GetMode())
	{
	case CameraMode::GHOST:
		((CGhostCamera*)camera_)->Move(dwDirection, fElapsedTime);
		break;
	case CameraMode::THIRD_PERSON:
	case CameraMode::FIRST_PERSON:
	{
		XMFLOAT3 direction_vector = XMFLOAT3(0.f, 0.f, 0.f);
		if (dwDirection)
		{
			if (orient_rotation_to_movement_)
			{
				float yaw = camera_->GetYaw();

				if (dwDirection & DIR_BACKWARD) yaw += 180;
				if (dwDirection & DIR_LEFT) yaw -= 90;
				if (dwDirection & DIR_RIGHT) yaw += 90;

				rotation_component_->set_yaw(yaw);
				rotation_component_->Update(0.f);
				direction_vector = look_vector();
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
		break;
	default:
		break;
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

void CPlayer::Update(float elapsed_time)
{
	if (movement_component_)
		movement_component_->Update(elapsed_time);

	if (rotation_component_)
		rotation_component_->Update(elapsed_time);

	if (camera_->GetMode() == CameraMode::THIRD_PERSON)
	{
		camera_->Update(elapsed_time);
	}
	camera_->RegenerateViewMatrix();
}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (pCamera->GetMode() != CameraMode::FIRST_PERSON)
	{
		CGameObject::Render(pd3dCommandList);
	}
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||< CEllenPlayer >|||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CEllenPlayer::CEllenPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) 
{
	m_xmf4x4World = Matrix4x4::Identity();
	to_parent_matrix_ = Matrix4x4::Identity();
	CGameObject* pGameObject = NULL;
	
	std::ifstream InFile("../Resource/Model/Mawang_Zup.bin", std::ios::binary);

	std::string strToken;

	FBXLoad::ReadStringFromFile(InFile, strToken);

	int nFrames = 0;
	while (strToken != "</Hierarchy>") 
	{
		FBXLoad::ReadStringFromFile(InFile, strToken);
		if (strToken == "<Frame>:") 
		{
			pGameObject = CGameObject::LoadHeirarchyFromFile(pd3dDevice, pd3dCommandList, InFile, nFrames);
			SetChild(pGameObject);
		}
	}

	FBXLoad::ReadStringFromFile(InFile, strToken);

	if (strToken == "<Animation>")
	{
		m_pAnimationController = new CAnimationController;
		m_pAnimationController->LoadAnimationFromFile(InFile);

		FBXLoad::ReadStringFromFile(InFile, strToken);

	}

	m_pAnimationController->SetFrameCaches(this);

	m_pChild->PrepareSkinning(pd3dDevice, pd3dCommandList, m_pChild);

	m_fSpeed = 1000.f;

	movement_component_ = new CMovementComponent((CGameObject*)this, XMFLOAT3(0.f,0.f,0.f), m_fSpeed);
	rotation_component_ = new CRotationComponent((CGameObject*)this);

	rotation_component_->set_use_pitch(false);

	axis_transform_matrix_ = new XMFLOAT4X4
	(	1,0,0,0,
		0,0,-1,0,
		0,1,0,0,
		0,0,0,1 );

	SetCamera(pCamera);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetShader((int)ShaderNum::Standard);
}
