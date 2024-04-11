//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "Animation.h"
//#include "MovementComponent.h"
#include "Camera.h"
#include "Mesh.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer()
{
	m_pCamera = NULL;

	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

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

	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::AddVelocity(const DWORD& dwDirection, const float& fElapsedTime)
{
	if (m_pCamera->GetMode() == CameraMode::GHOST)
	{
		((CGhostCamera*)m_pCamera)->Move(dwDirection, fElapsedTime);
		return;
	}
	if (dwDirection)
	{
		if (dwDirection & DIR_FORWARD) m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Look, m_fSpeed * fElapsedTime);
		if (dwDirection & DIR_BACKWARD) m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Look, -m_fSpeed * fElapsedTime);
		if (dwDirection & DIR_LEFT) m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Right, -m_fSpeed * fElapsedTime);
		if (dwDirection & DIR_RIGHT) m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Right, +m_fSpeed * fElapsedTime);
		if (dwDirection & DIR_UP) m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Up, +m_fSpeed * fElapsedTime);
		if (dwDirection & DIR_DOWN) m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Up, -m_fSpeed * fElapsedTime);
	}
	else
		m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	UpdateVelocity();
}

void CPlayer::Move(float fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
}

void CPlayer::OnRotate()
{
	m_bRotate = true;
	if (m_pCamera->GetMode() == CameraMode::THIRD_PERSON)
	{
		CThirdPersonCamera* pCamera = (CThirdPersonCamera*)m_pCamera;
		pCamera->ResetFromPlayer();
	}
}

void CPlayer::Rotate(const float& fPitch, const float& fYaw, const float& fRoll)
{
	if (m_pCamera) m_pCamera->Rotate(fPitch, fYaw, fRoll);

	if (!m_bRotate) return;

	m_fPitch += fPitch;
	m_fYaw += fYaw;
	m_fRoll += fRoll;

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.f), XMConvertToRadians(fYaw), XMConvertToRadians(0.f));
	m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

}

void CPlayer::Update(float fTimeElapsed)
{
	Move(fTimeElapsed);

	if (m_pCamera->GetMode() == CameraMode::THIRD_PERSON)
	{
		m_pCamera->Update(fTimeElapsed);
	}
	m_pCamera->RegenerateViewMatrix();
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (pCamera->GetMode() != CameraMode::FIRST_PERSON)
	{
		CGameObject::Render(pd3dCommandList);
	}
}

void CPlayer::UpdateVelocity()
{
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (m_fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (m_fMaxVelocityXZ / fLength);
	}
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (m_fMaxVelocityY / fLength);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||< CEllenPlayer >|||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CEllenPlayer::CEllenPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) 
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4ToParent = Matrix4x4::Identity();
	CGameObject* pGameObject = NULL;
	
	std::ifstream InFile("../Resource/Model/test.bin", std::ios::binary);

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

	CSkinMesh* pSkinMesh = (CSkinMesh*)FindSkinMeshFrame()->m_pMesh;
	if(pSkinMesh) 
		pSkinMesh->SetBoneFrameCaches(pd3dDevice, pd3dCommandList, m_pChild);

	//m_xmf3Gravity.y = -9.8f;
	m_fSpeed = 1000.f;
	m_fMaxVelocityXZ = 100.f;

	SetCamera(pCamera);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetShader((int)ShaderNum::AnimationWireframe);
}

void CEllenPlayer::OnPrepareRender()
{

	CPlayer::OnPrepareRender();
	XMFLOAT4X4 xmf4x4AxisTransform = XMFLOAT4X4
	{	1,0,0,0,
		0,0,-1,0,
		0,1,0,0,
		0,0,0,1 };
	m_xmf4x4ToParent = Matrix4x4::Multiply(xmf4x4AxisTransform, m_xmf4x4ToParent);

}