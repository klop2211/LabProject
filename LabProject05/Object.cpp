#include "stdafx.h"
#include "Object.h"
#include "Scene.h"
#include "Shader.h"
#include "Animation.h"
#include "Camera.h"
#include "Mesh.h"
#include "DescriptorManager.h"
#include "Material.h"

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||| <CGameObject> |||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CGameObject::CGameObject()
{
	to_parent_matrix_ = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	for (auto& p : m_Materials) p->Release();
	if (axis_transform_matrix_) delete axis_transform_matrix_;

	if (child_) child_->Release();
	if (sibling_) sibling_->Release();
}

void CGameObject::set_look_vector(const float& x, const float& y, const float& z)
{
	to_parent_matrix_._31 = x, to_parent_matrix_._32 = y, to_parent_matrix_._33 = z;
}

void CGameObject::set_right_vector(const float& x, const float& y, const float& z)
{
	to_parent_matrix_._11 = x, to_parent_matrix_._12 = y, to_parent_matrix_._13 = z;
}

void CGameObject::set_up_vector(const float& x, const float& y, const float& z)
{
	to_parent_matrix_._21 = x, to_parent_matrix_._22 = y, to_parent_matrix_._23 = z;
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh)
	{
		m_pMesh->Release();
	}
	m_pMesh = pMesh;
	if (pMesh) pMesh->AddRef();
}

void CGameObject::SetMaterial(const int& index, CMaterial* pMaterial)
{
	if (m_Materials.size() == index)
	{
		m_Materials.emplace_back(pMaterial);
	}
	else 
		m_Materials[index] = pMaterial;
	m_Materials[index]->AddRef();
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	if (axis_transform_matrix_)
	{
		XMFLOAT4X4 mat = Matrix4x4::Multiply(*axis_transform_matrix_, to_parent_matrix_);
		m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(mat, *pxmf4x4Parent) : mat;
	}
	else
		m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(to_parent_matrix_, *pxmf4x4Parent) : to_parent_matrix_;

	if (sibling_) sibling_->UpdateTransform(pxmf4x4Parent);
	if (child_) child_->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager)
{
	for (auto& Material : m_Materials)
		Material->CreateShaderResourceViews(pd3dDevice, pDescriptorManager);

	if (child_) child_->CreateShaderResourceViews(pd3dDevice, pDescriptorManager);
	if (sibling_) sibling_->CreateShaderResourceViews(pd3dDevice, pDescriptorManager);
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (animation_controller_)
	{
		ResetAnimatedSRT();
		animation_controller_->Animate(fTimeElapsed, this);
	}

	if (sibling_) sibling_->Animate(fTimeElapsed);
	if (child_) child_->Animate(fTimeElapsed);
}

void CGameObject::ResetAnimatedSRT()
{
	m_xmf3BlendedScale = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3BlendedRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3BlendedTranslation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	if (sibling_) sibling_->ResetAnimatedSRT();
	if (child_) child_->ResetAnimatedSRT();

}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!parent_ && !animation_controller_)	// 이 오브젝트가 루트 노드이고 애니메이션이 없을 때만 시행
		UpdateTransform(NULL);

	for(auto& p : m_Materials)
		p->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh) 
	{
		UpdateShaderVariables(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList);
	}

	if (child_) child_->Render(pd3dCommandList);
	if (sibling_) sibling_->Render(pd3dCommandList);
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (auto& p : m_Materials)
		p->ReleaseUploadBuffers();
}

void CGameObject::set_position_vector(const float& x, const float& y, const float& z)
{
	to_parent_matrix_._41 = x;
	to_parent_matrix_._42 = y;
	to_parent_matrix_._43 = z;
}


XMFLOAT3 CGameObject::position_vector() const
{
	return(XMFLOAT3(to_parent_matrix_._41, to_parent_matrix_._42, to_parent_matrix_._43));
}

XMFLOAT3 CGameObject::look_vector() const
{
	return(Vector3::Normalize(XMFLOAT3(to_parent_matrix_._31, to_parent_matrix_._32, to_parent_matrix_._33)));
}

XMFLOAT3 CGameObject::up_vector() const
{
	return(Vector3::Normalize(XMFLOAT3(to_parent_matrix_._21, to_parent_matrix_._22, to_parent_matrix_._23)));
}

XMFLOAT3 CGameObject::right_vector() const
{
	return(Vector3::Normalize(XMFLOAT3(to_parent_matrix_._11, to_parent_matrix_._12, to_parent_matrix_._13)));
}

void CGameObject::UpdateMatrixByBlendedSRT()
{
	XMMATRIX S = XMMatrixScaling(m_xmf3BlendedScale.x, m_xmf3BlendedScale.y, m_xmf3BlendedScale.z);
	XMMATRIX R = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(m_xmf3BlendedRotation.x), XMMatrixRotationY(m_xmf3BlendedRotation.y)), XMMatrixRotationZ(m_xmf3BlendedRotation.z));
	XMMATRIX T = XMMatrixTranslation(m_xmf3BlendedTranslation.x, m_xmf3BlendedTranslation.y, m_xmf3BlendedTranslation.z);
	XMStoreFloat4x4(&to_parent_matrix_, XMMatrixMultiply(XMMatrixMultiply(S, R), T));
}

void CGameObject::set_child(CGameObject* pChild)
{
	if (pChild)
	{
		pChild->set_parent(this);
	}
	if (child_)
	{
		if (pChild) pChild->sibling_ = child_->sibling_;
		child_->sibling_ = pChild;
	}
	else
	{
		child_ = pChild;
	}
}

void CGameObject::set_sibling(CGameObject* ptr)
{
	if (ptr)
	{
		if(parent_)
			ptr->parent_ = parent_;
		if (sibling_)
		{
			sibling_->set_sibling(ptr);
		}
		else
		{
			sibling_ = ptr;
		}
	}
}

void CGameObject::set_parent(CGameObject* ptr)
{
	parent_ = ptr;
	if (sibling_) sibling_->set_parent(ptr);
}

CGameObject* CGameObject::FindFrame(const std::string& strFrameName)
{
	if (m_strFrameName == strFrameName) return this;

	CGameObject* pFrameObject = NULL;
	if (sibling_) if (pFrameObject = sibling_->FindFrame(strFrameName)) return(pFrameObject);
	if (child_) if (pFrameObject = child_->FindFrame(strFrameName)) return(pFrameObject);

	return NULL;
}

void CGameObject::PrepareSkinning(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pRootObject)
{
	CSkinMesh* pSkinMesh = dynamic_cast<CSkinMesh*>(m_pMesh);
	if(pSkinMesh)
		pSkinMesh->SetBoneFrameCaches(pd3dDevice, pd3dCommandList, pRootObject);

	if (child_) child_->PrepareSkinning(pd3dDevice, pd3dCommandList, pRootObject);
	if (sibling_) sibling_->PrepareSkinning(pd3dDevice, pd3dCommandList, pRootObject);
}

void CGameObject::LoadMaterialFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::ifstream& InFile)
{
	int nMaterials = FBXLoad::ReadFromFile<int>(InFile);
	m_Materials.reserve(nMaterials);

	std::string strToken;

	for (int i = 0; i < nMaterials; ++i)
	{

		FBXLoad::ReadStringFromFile(InFile, strToken);

		FBXLoad::ReadFromFile<int>(InFile);

		FBXLoad::ReadStringFromFile(InFile, strToken);

		CMaterial* pMaterial = new CMaterial; //TODO: Material의 이름으로 중복이 있는지 체크해야함

		pMaterial->SetName(strToken);

		FBXLoad::ReadStringFromFile(InFile, strToken); // <TextureProperties>: 

		pMaterial->LoadTexturePropertiesFromFile(pd3dDevice, pd3dCommandList, InFile);

		m_Materials.emplace_back(pMaterial);

		FBXLoad::ReadStringFromFile(InFile, strToken); // </TextureProperties>

		FBXLoad::ReadStringFromFile(InFile, strToken); // <ShadingModel>:

		FBXLoad::ReadStringFromFile(InFile, strToken); // name
		std::string strName = strToken;

		FBXLoad::ReadStringFromFile(InFile, strToken); // <Phong>: or <Lambert>:, <Unknown>
		if (strToken == "<Phong>:")
		{
			CShadingModel* pShadingModel = new CPhongModel;
			pShadingModel->LoadShadingModelFromFile(InFile);
		}
		else if (strToken == "<Lambert>:")
		{
			CShadingModel* pShadingModel = new CLambertModel;
			pShadingModel->LoadShadingModelFromFile(InFile);

		}
		FBXLoad::ReadStringFromFile(InFile, strToken); // </Materials>
	}
}

CModelInfo CGameObject::LoadModelInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& model_file_name)
{
	std::string token;

	CGameObject* frame_root = NULL;

	//Heirarchy
	std::ifstream file(model_file_name, std::ios::binary);

	FBXLoad::ReadStringFromFile(file, token);

	int frames = 0;
	while (token != "</Hierarchy>")
	{
		FBXLoad::ReadStringFromFile(file, token);
		if (token == "<Frame>:")
		{
			CGameObject* frame;
			frame = CGameObject::LoadHeirarchyFromFile(pd3dDevice, pd3dCommandList, file, frames);
			if(!frame_root) 
				frame_root = frame;
			else
				frame_root->set_sibling(frame);
		}
	}

	//Animations
	FBXLoad::ReadStringFromFile(file, token);

	CAnimationController* animation_controller = NULL;

	if (token == "<Animation>")
	{
		animation_controller = CGameObject::LoadAnimationFromFile(file, frame_root);

		FBXLoad::ReadStringFromFile(file, token);
	}

	frame_root->PrepareSkinning(pd3dDevice, pd3dCommandList, frame_root);

	CModelInfo rvalue;

	rvalue.heirarchy_root = frame_root;
	rvalue.animation_controller = animation_controller;

	return rvalue;
}

CAnimationController* CGameObject::LoadAnimationFromFile(std::ifstream& file, CGameObject* root_object)
{
	CAnimationController* rvalue = new CAnimationController;

	rvalue->LoadAnimationFromFile(file);

	//rvalue->SetFrameCaches(root_object);

	return rvalue;
}

CGameObject* CGameObject::LoadHeirarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	std::ifstream& InFile, int& nFrames)
{
	CGameObject* rvalue = new CGameObject;

	std::string strToken;

	nFrames++;
	FBXLoad::ReadFromFile<int>(InFile);

	FBXLoad::ReadStringFromFile(InFile, rvalue->m_strFrameName);

	while (strToken != "</Frame>")
	{
		FBXLoad::ReadStringFromFile(InFile, strToken);
		if (strToken == "<Transform>:")
		{
			rvalue->to_parent_matrix_ = FBXLoad::ReadFromFile<XMFLOAT4X4>(InFile);

			rvalue->m_xmf3Scale = FBXLoad::ReadFromFile<XMFLOAT3>(InFile);
			rvalue->m_xmf3Rotation = FBXLoad::ReadFromFile<XMFLOAT3>(InFile);
			rvalue->m_xmf3Translation = FBXLoad::ReadFromFile<XMFLOAT3>(InFile);
		}
		else if (strToken == "<SkinDeformations>:")
		{
			CSkinMesh* pSkinMesh = new CSkinMesh();
			pSkinMesh->LoadSkinMeshFromFile(pd3dDevice, pd3dCommandList, InFile);

			FBXLoad::ReadStringFromFile(InFile, strToken);

			pSkinMesh->SetType(1);

			if (strToken == "<Mesh>:") pSkinMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, InFile);

			rvalue->SetMesh(pSkinMesh);
		}
		else if (strToken == "<Mesh>:")
		{
			CMesh* pMesh = new CMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, InFile);
			rvalue->SetMesh(pMesh);
		}
		else if (strToken == "<Materials>:")
		{
			rvalue->LoadMaterialFromFile(pd3dDevice, pd3dCommandList, InFile);
		}
		else if (strToken == "<Children>:")
		{
			int nChildren = FBXLoad::ReadFromFile<int>(InFile);
			for (int i = 0; i < nChildren; ++i)
			{
				FBXLoad::ReadStringFromFile(InFile, strToken);
				if (strToken == "<Frame>:")
				{
					CGameObject* pChild = CGameObject::LoadHeirarchyFromFile(pd3dDevice, pd3dCommandList, InFile, nFrames);
					if (pChild) rvalue->set_child(pChild);
				}
			}
		}
	}

	return rvalue;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||| <CHeightMapTerrain> |||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	CHeightMapGridMesh* pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pHeightMapGridMesh);
	
	CMaterial* pTerrainMaterial = new CMaterial(2);

	pTerrainMaterial->AddTexturePropertyFromDDSFile(pd3dDevice, pd3dCommandList, "../Resource/Terrain/Base_Texture.dds", TextureType::RESOURCE_TEXTURE2D, 3);
	pTerrainMaterial->AddTexturePropertyFromDDSFile(pd3dDevice, pd3dCommandList, "../Resource/Terrain/Detail_Texture_7.dds", TextureType::RESOURCE_TEXTURE2D, 6);
	//pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"../Resource/Terrain/HeightMap(Alpha).dds", TextureType::RESOURCE_TEXTURE2D, 2);

	pTerrainMaterial->SetShader((int)ShaderNum::Terrain);

	SetMaterial(0, pTerrainMaterial);

}

CHeightMapTerrain::~CHeightMapTerrain()
{
}

float CHeightMapTerrain::GetHeight(float x, float z, bool bReverseQuad)
{ 
	return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad));
} 

XMFLOAT3 CHeightMapTerrain::GetNormal(float x, float z)
{ 
	return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
}

int CHeightMapTerrain::GetRawImageWidth()
{ 
	return(m_pHeightMapImage->GetHeightMapWidth());
}

int CHeightMapTerrain::GetRawImageLength()
{ 
	return(m_pHeightMapImage->GetHeightMapLength());
}

