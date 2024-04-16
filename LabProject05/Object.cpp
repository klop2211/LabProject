#include "stdafx.h"
#include "Object.h"
#include "Scene.h"
#include "Shader.h"
#include "Animation.h"
#include "Camera.h"
#include "Mesh.h"
#include "DescriptorManager.h"


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||| <CTexture> ||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CTexture::CTexture()
{

}

CTexture::CTexture(std::ifstream& InFile)
{
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken);

	if (strToken == "<Texture>:")
		LoadTextureFromFile(InFile);
}

CTexture::~CTexture()
{
	if (m_pd3dTexture)
		m_pd3dTexture->Release();
}

void CTexture::SetRootParameterIndex(UINT nRootParameterIndex)
{
	m_RootParameterIndex = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_SrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_SamplerGpuDescriptorHandle = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_RootParameterIndex, m_SrvGpuDescriptorHandle);
}

void CTexture::ReleaseShaderVariables()
{

}

void CTexture::LoadTextureFromFile(std::ifstream& InFile)
{
	FBXLoad::ReadFromFile<int>(InFile); //index

	std::string strToken;

	FBXLoad::ReadStringFromFile(InFile, strToken);
	m_strTextureName = strToken;

	m_LoadType = (TextureLoadType)FBXLoad::ReadFromFile<int>(InFile);

	FBXLoad::ReadStringFromFile(InFile, strToken);
	m_strTextureFileName = strToken;

	m_UVScale.x = FBXLoad::ReadFromFile<float>(InFile);
	m_UVScale.y = FBXLoad::ReadFromFile<float>(InFile);

	m_UVTranslation.x = FBXLoad::ReadFromFile<float>(InFile);
	m_UVTranslation.y = FBXLoad::ReadFromFile<float>(InFile);

	m_UVSwap = (bool)FBXLoad::ReadFromFile<int>(InFile);

	m_UVWRotation.x = FBXLoad::ReadFromFile<float>(InFile);
	m_UVWRotation.y = FBXLoad::ReadFromFile<float>(InFile);
	m_UVWRotation.z = FBXLoad::ReadFromFile<float>(InFile);

	m_AlphaSource = FBXLoad::ReadFromFile<int>(InFile);

	m_CroppingBox.x = FBXLoad::ReadFromFile<int>(InFile); // left
	m_CroppingBox.y = FBXLoad::ReadFromFile<int>(InFile); // top
	m_CroppingBox.z = FBXLoad::ReadFromFile<int>(InFile); // right
	m_CroppingBox.w = FBXLoad::ReadFromFile<int>(InFile); // bottom

	m_MappingType = (TextureMappingType)FBXLoad::ReadFromFile<int>(InFile);

	m_BlendMode = (TextureBlendMode)FBXLoad::ReadFromFile<int>(InFile);

	m_DefaultAlpha = FBXLoad::ReadFromFile<float>(InFile);

	m_MaterialUseType = (TextureMaterialUseType)FBXLoad::ReadFromFile<int>(InFile);

	m_TextureUseType = (TextureUseType)FBXLoad::ReadFromFile<int>(InFile);
}

void CTexture::ReleaseUploadBuffer()
{
	m_pd3dTextureUploadBuffer->Release();
}

void CTexture::CreateShaderResourceView(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc();
	pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &d3dShaderResourceViewDesc, pDescriptorManager->GetSrvNextCPUHandle());
	
	m_SrvGpuDescriptorHandle = pDescriptorManager->GetSrvNextGPUHandle();
	
	pDescriptorManager->AddSrvIndex();
}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& strFileName, TextureType nResourceType, UINT nRootParameterIndex)
{
	m_ResourceType = nResourceType;
	m_RootParameterIndex = nRootParameterIndex;
	std::wstring wstrFileName;
	wstrFileName.assign(strFileName.begin(), strFileName.end());
	m_pd3dTexture = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, wstrFileName.c_str(), &m_pd3dTextureUploadBuffer, D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_ResourceType = TextureType::RESOURCE_BUFFER;
	m_BufferFormat = ndxgiFormat;
	m_BufferElements = nElements;
	m_pd3dTexture = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_pd3dTextureUploadBuffer);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, TextureType nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue)
{
	m_ResourceType = nResourceType;
	m_pd3dTexture = ::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_pd3dTexture);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc()
{
	D3D12_RESOURCE_DESC d3dResourceDesc = m_pd3dTexture->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (m_ResourceType)
	{
	case TextureType::RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case TextureType::RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case TextureType::RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case TextureType::RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case TextureType::RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_BufferFormat;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_BufferElements;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||| <CMaterial> |||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CMaterial::CMaterial()
{
}

CMaterial::CMaterial(const int& nProperty)
{
	m_TextureProperties.reserve(nProperty);
}

CMaterial::~CMaterial()
{
	if (m_pShadingModel)
		delete m_pShadingModel;
}

void CMaterial::ReleaseUploadBuffers()
{
	for (auto& Property : m_TextureProperties)
		Property.ReleaseUploadBuffers();
}

void CMaterial::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager)
{
	for (auto& Property : m_TextureProperties)
		Property.CreateShaderResourceViews(pd3dDevice, pDescriptorManager);
}

void CMaterial::LoadTexturePropertiesFromFile(std::ifstream& InFile)
{
	std::string strToken;

	int nProperties = FBXLoad::ReadFromFile<int>(InFile);

	m_TextureProperties.reserve(nProperties);

	for (int j = 0; j < nProperties; ++j)
	{
		FBXLoad::ReadStringFromFile(InFile, strToken); // <Property>:
		FBXLoad::ReadFromFile<int>(InFile); // index
		FBXLoad::ReadStringFromFile(InFile, strToken); //TextureChannelName
		FBXLoad::ReadStringFromFile(InFile, strToken); //strPropertyName
		if (strToken != "Null")
		{
			m_TextureProperties.emplace_back(InFile, strToken);
		}
	}
}

void CMaterial::AddTexturePropertyFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& strFileName, TextureType nResourceType, UINT nRootParameterIndex)
{
	m_TextureProperties.emplace_back(pd3dDevice, pd3dCommandList, strFileName, nResourceType, nRootParameterIndex);
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (auto& Property : m_TextureProperties)
		Property.UpdateShaderVariables(pd3dCommandList);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||| <CGameObject> |||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CGameObject::CGameObject()
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	for (auto& p : m_Materials) p->Release();
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
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
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

	if (m_pChild) m_pChild->CreateShaderResourceViews(pd3dDevice, pDescriptorManager);
	if (m_pSibling) m_pSibling->CreateShaderResourceViews(pd3dDevice, pDescriptorManager);
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (m_pAnimationController)
	{
		ResetAnimatedSRT();
		m_pAnimationController->Animate(fTimeElapsed, this);
	}

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}

void CGameObject::ResetAnimatedSRT()
{
	m_xmf3BlendedScale = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3BlendedRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3BlendedTranslation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	if (m_pSibling) m_pSibling->ResetAnimatedSRT();
	if (m_pChild) m_pChild->ResetAnimatedSRT();

}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_pParent && !m_pAnimationController)	// 이 오브젝트가 루트 노드이고 애니메이션이 없을 때만 시행
		UpdateTransform(NULL);

	for(auto& p : m_Materials)
		p->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh) 
	{
		UpdateShaderVariables(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList);
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList);
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (auto& p : m_Materials)
		p->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(const float& x, const float& y, const float& z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4ToParent._41, m_xmf4x4ToParent._42, m_xmf4x4ToParent._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4ToParent._31, m_xmf4x4ToParent._32, m_xmf4x4ToParent._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4ToParent._21, m_xmf4x4ToParent._22, m_xmf4x4ToParent._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4ToParent._11, m_xmf4x4ToParent._12, m_xmf4x4ToParent._13)));
}

void CGameObject::Rotate(const float& fPitch, const float& fYaw, const float& fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);
}

void CGameObject::SetChild(CGameObject* pChild)
{
	if (pChild)
	{
		pChild->m_pParent = this;
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

CGameObject* CGameObject::FindFrame(const std::string& strFrameName)
{
	if (m_strFrameName == strFrameName) return this;

	CGameObject* pFrameObject = NULL;
	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(strFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(strFrameName)) return(pFrameObject);

	return NULL;
}

void CGameObject::PrepareSkinning(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pRootObject)
{
	CSkinMesh* pSkinMesh = dynamic_cast<CSkinMesh*>(m_pMesh);
	if(pSkinMesh)
		pSkinMesh->SetBoneFrameCaches(pd3dDevice, pd3dCommandList, pRootObject);

	if (m_pChild) m_pChild->PrepareSkinning(pd3dDevice, pd3dCommandList, pRootObject);
	if (m_pSibling) m_pSibling->PrepareSkinning(pd3dDevice, pd3dCommandList, pRootObject);
}

void CGameObject::LoadMaterialFromFile(std::ifstream& InFile)
{
	int nMaterials = FBXLoad::ReadFromFile<int>(InFile);
	m_Materials.reserve(nMaterials);

	std::string strToken;

	for (int i = 0; i < nMaterials; ++i)
	{
		FBXLoad::ReadFromFile<int>(InFile);

		FBXLoad::ReadStringFromFile(InFile, strToken);

		CMaterial* pMaterial = new CMaterial; //TODO: Material의 이름으로 중복이 있는지 체크해야함

		pMaterial->SetName(strToken);

		FBXLoad::ReadStringFromFile(InFile, strToken); // <TextureProperties>: 

		pMaterial->LoadTexturePropertiesFromFile(InFile);

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
			rvalue->m_xmf4x4ToParent = FBXLoad::ReadFromFile<XMFLOAT4X4>(InFile);

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
			rvalue->LoadMaterialFromFile(InFile);
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
					if (pChild) rvalue->SetChild(pChild);
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



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||| <CTextureProperty> ||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CTextureProperty::CTextureProperty(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& strFileName, TextureType nResourceType, UINT nRootParameterIndex)
{
	m_Textures.emplace_back();
	m_Textures.back().LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, strFileName, nResourceType, nRootParameterIndex);
}

CTextureProperty::CTextureProperty(std::ifstream& InFile, const std::string& strName) : m_PropertyName(strName)
{
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken);
	if(strToken == "<Textures>:")
		LoadTexturePropertyFromFile(InFile);
}

void CTextureProperty::LoadTexturePropertyFromFile(std::ifstream& InFile)
{
	int nTextures = FBXLoad::ReadFromFile<int>(InFile);
	m_Textures.reserve(nTextures);
	for (int i = 0; i < nTextures; ++i)
	{
		m_Textures.emplace_back(InFile);
	}

}

void CTextureProperty::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (auto& Texture : m_Textures)
		Texture.UpdateShaderVariable(pd3dCommandList);
}

void CTextureProperty::ReleaseUploadBuffers()
{
	for (auto& Texture : m_Textures)
		Texture.ReleaseUploadBuffer();
}

void CTextureProperty::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager)
{
	m_Textures[0].CreateShaderResourceView(pd3dDevice, pDescriptorManager);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||| <CPhongModel> ||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CPhongModel::LoadShadingModelFromFile(std::ifstream& InFile)
{
	m_Ambient.x = FBXLoad::ReadFromFile<float>(InFile);
	m_Ambient.y = FBXLoad::ReadFromFile<float>(InFile);
	m_Ambient.z = FBXLoad::ReadFromFile<float>(InFile);

	m_Diffuse.x = FBXLoad::ReadFromFile<float>(InFile);
	m_Diffuse.y = FBXLoad::ReadFromFile<float>(InFile);
	m_Diffuse.z = FBXLoad::ReadFromFile<float>(InFile);

	m_Specular.x = FBXLoad::ReadFromFile<float>(InFile);
	m_Specular.y = FBXLoad::ReadFromFile<float>(InFile);
	m_Specular.z = FBXLoad::ReadFromFile<float>(InFile);

	m_Emissive.x = FBXLoad::ReadFromFile<float>(InFile);
	m_Emissive.y = FBXLoad::ReadFromFile<float>(InFile);
	m_Emissive.z = FBXLoad::ReadFromFile<float>(InFile);

	m_TransparencyFactor = FBXLoad::ReadFromFile<float>(InFile);
	m_Shininess = FBXLoad::ReadFromFile<float>(InFile);
	m_ReflectionFactor = FBXLoad::ReadFromFile<float>(InFile);

}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||| <CLambertModel> |||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CLambertModel::LoadShadingModelFromFile(std::ifstream& InFile)
{
	m_Ambient.x = FBXLoad::ReadFromFile<float>(InFile);
	m_Ambient.y = FBXLoad::ReadFromFile<float>(InFile);
	m_Ambient.z = FBXLoad::ReadFromFile<float>(InFile);

	m_Diffuse.x = FBXLoad::ReadFromFile<float>(InFile);
	m_Diffuse.y = FBXLoad::ReadFromFile<float>(InFile);
	m_Diffuse.z = FBXLoad::ReadFromFile<float>(InFile);

	m_Emissive.x = FBXLoad::ReadFromFile<float>(InFile);
	m_Emissive.y = FBXLoad::ReadFromFile<float>(InFile);
	m_Emissive.z = FBXLoad::ReadFromFile<float>(InFile);

	m_TransparencyFactor = FBXLoad::ReadFromFile<float>(InFile);
}
