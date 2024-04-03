#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Mesh.h"
#include "Camera.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CGameObject;

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType;

	int								m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	ID3D12Resource** m_ppd3dTextures = NULL;
	ID3D12Resource** m_ppd3dTextureUploadBuffers;

	UINT* m_pnResourceTypes = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;

	int								m_nRootParameters = 0;
	int* m_pnRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);

	//int LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex);

	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);
	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }

	int GetRootParameters() { return(m_nRootParameters); }
	int GetTextures() { return(m_nTextures); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	int GetRootParameter(int nIndex) { return(m_pnRootParameterIndices[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40


class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	int m_nShader = NULL;
	CTexture* m_pTexture = NULL;

	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(const int& nShader) { m_nShader = nShader; }
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(CTexture* pTexture);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();


public:
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

};

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||| <CGameObject> ||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class CAnimationController;

class CGameObject
{
public:
	CGameObject();
	~CGameObject();

	bool CheckShader(const int& nShader) { return nShader == m_nShader; }

	virtual void SetMesh(CMesh* pMesh);
	void SetShader(const int& nShader) { m_nShader = nShader; }
	void SetMaterial(int nMaterial, CMaterial* pMaterial);

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ResetAnimatedSRT();

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	XMFLOAT4X4& GetWorldMT()  { return m_xmf4x4World; }

	virtual void SetPosition(const float& x, const float& y, const float& z);
	virtual void SetPosition(XMFLOAT3 xmf3Position);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	virtual void Rotate(const float& fPitch, const float& fYaw, const float& fRoll);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetChild(CGameObject* pChild);

	CGameObject* FindFrame(const std::string& strFrameName);
	CGameObject* FindSkinMeshFrame();

public:
	static CGameObject* LoadHeirarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		std::ifstream& InFile, int& nFrames);
	

public:
	int	m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	//Heirarchy 구조 관련 변수
	std::string m_strFrameName;

	XMFLOAT4X4 m_xmf4x4ToParent;

	XMFLOAT3 m_xmf3Scale;
	XMFLOAT3 m_xmf3Rotation;
	XMFLOAT3 m_xmf3Translation;

	XMFLOAT3 m_xmf3BlendedScale;
	XMFLOAT3 m_xmf3BlendedRotation;
	XMFLOAT3 m_xmf3BlendedTranslation;

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;

	CMesh* m_pMesh = NULL;

protected:
	XMFLOAT4X4 m_xmf4x4World;


	// 이 오브젝트가 사용하는 쉐이더 넘버
	int m_nShader = -1;

	CAnimationController* m_pAnimationController = NULL;
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||| <CRotatingObject> ||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

private:
	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||| <CHeightMapTerrain> |||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	~CHeightMapTerrain();
private:
	CHeightMapImage* m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad)); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetRawImageWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetRawImageLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

};
