#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

enum class TextureType 
{
	NONE = 0,
	RESOURCE_TEXTURE2D = 0x01, 
	RESOURCE_TEXTURE2D_ARRAY = 0x02,
	RESOURCE_TEXTURE2DARRAY = 0x03,
	RESOURCE_TEXTURE_CUBE = 0x04,
	RESOURCE_BUFFER = 0x05
};

enum class TextureLoadType
{
	File = 0,
	Procedural = 1
};

enum class TextureMappingType
{
	Null = 0,
	Planar = 1,
	Spherical = 2, 
	Cylindrical = 3,
	Box = 4,
	Face = 5,
	UV = 6,
	Environment = 7
};

enum class TextureBlendMode
{
	None = -1, 
	Translucent = 0,
	Additive = 1, 
	Modulate = 2,
	Modulate2, 
	Over, 
	Normal,
	Dissolve, 
	Darken, 
	ColorBurn,
	LinearBurn,
	DarkerColor, 
	Lighten, 
	Screen, 
	ColorDodge, 
	LinearDodge, 
	LighterColor, 
	SoftLight, 
	HardLight, 
	VividLight, 
	LinearLight, 
	PinLight, 
	HardMix,
	Difference, 
	Exclusion, 
	Substract, 
	Divide, 
	Hue, 
	Saturation, 
	Color, 
	Luminosity, 
	Overlay
};

enum class TextureMaterialUseType
{
	None = -1,
	Model = 0,
	Default
};

enum class TextureUseType
{
	Standard = 0, 
	ShadowMap, 
	LightMap, 
	SphericalReflexionMap, 
	SphereReflexionMap, 
	BumpNormalMap
};

class CCamera;
class CMesh;
class CGameObject;
class CDescriptorManager;

class CTexture
{
public:
	CTexture();
	CTexture(std::ifstream& InFile);
	virtual ~CTexture();

private:
	//Fbx Texture Info
	std::string m_strTextureName;

	TextureLoadType m_LoadType;

	std::string m_strTextureFileName;

	XMFLOAT2 m_UVScale;
	XMFLOAT2 m_UVTranslation;
	bool m_UVSwap;
	XMFLOAT3 m_UVWRotation;

	int m_AlphaSource;

	XMINT4 m_CroppingBox; //Left, Top, Right, Bottom

	TextureMappingType m_MappingType;
	TextureBlendMode m_BlendMode;

	float m_DefaultAlpha;

	TextureMaterialUseType m_MaterialUseType;
	TextureUseType m_TextureUseType;

	// Render 관련
	TextureType m_ResourceType;

	ID3D12Resource* m_pd3dTexture;
	ID3D12Resource* m_pd3dTextureUploadBuffer;

	UINT m_RootParameterIndex;

	DXGI_FORMAT m_BufferFormat;
	UINT m_BufferElements;

	D3D12_GPU_DESCRIPTOR_HANDLE m_SrvGpuDescriptorHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE m_SamplerGpuDescriptorHandle;

public:
	void SetSampler(D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(std::ifstream& InFile);

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& strFileName, TextureType nResourceType, UINT nRootParameterIndex);
	
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, TextureType nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);

	void SetRootParameterIndex(UINT nRootParameterIndex);
	void SetGpuDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle() { return(m_SrvGpuDescriptorHandle); }

	std::string& GetTextureName() { return(m_strTextureName); }
	ID3D12Resource* GetResource() { return(m_pd3dTexture); }
	int GetRootParameter() { return(m_RootParameterIndex); }

	TextureType GetTextureType() { return(m_ResourceType); }
	DXGI_FORMAT GetBufferFormat() { return(m_BufferFormat); }
	int GetBufferElements() { return(m_BufferElements); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc();

	void ReleaseUploadBuffer();

	void CreateShaderResourceView(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager);
};


// 재질이 가지는 특성
class CTextureProperty
{
private:
	std::string m_PropertyName;
	std::vector<CTexture> m_Textures; // 각 특성을 표현하기위한 텍스처들

public:
	CTextureProperty() {}
	// dds 파일로 텍스처 직접 추가
	CTextureProperty(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& strFileName, TextureType nResourceType, UINT nRootParameterIndex);
	// fbx 파일을 익스포트한 bin 파일 로드
	CTextureProperty(std::ifstream& InFile, const std::string& strName);
	~CTextureProperty() {}

	void LoadTexturePropertyFromFile(std::ifstream& InFile);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseUploadBuffers();

	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager);
};

//TODO: ShadingModel이 어떤의미인지 어떻게 사용되는 지 공부
class CShadingModel
{
private:
	std::string m_strName;

public:
	virtual void LoadShadingModelFromFile(std::ifstream& InFile) {}
};

class CPhongModel : public CShadingModel
{
private:
	XMFLOAT3 m_Ambient;
	XMFLOAT3 m_Diffuse;
	XMFLOAT3 m_Specular;
	XMFLOAT3 m_Emissive;

	float m_TransparencyFactor;
	float m_Shininess;
	float m_ReflectionFactor;

public:
	virtual void LoadShadingModelFromFile(std::ifstream& InFile);


};

class CLambertModel : public CShadingModel
{
private:
	XMFLOAT3 m_Ambient;
	XMFLOAT3 m_Diffuse;
	XMFLOAT3 m_Emissive;

	float m_TransparencyFactor;

public:
	virtual void LoadShadingModelFromFile(std::ifstream& InFile);


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
	CMaterial(const int& nProperty);
	virtual ~CMaterial();

private:
	int	m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	int m_nShader = NULL;
	std::vector<CTextureProperty> m_TextureProperties;

	CShadingModel* m_pShadingModel = NULL;

	std::string m_strName;

	XMFLOAT4 m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(const int& nShader) { m_nShader = nShader; }
	void SetMaterialType(UINT nType) { m_nType |= nType; }

	void SetName(const std::string& strName) { m_strName = strName; }

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();

	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager);

	void LoadTexturePropertiesFromFile(std::ifstream& InFile);

	void AddTexturePropertyFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& strFileName, TextureType nResourceType, UINT nRootParameterIndex);

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
	void SetMaterial(const int& index, CMaterial* pMaterial);

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager);

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ResetAnimatedSRT();

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	XMFLOAT4X4& GetWorldMT()  { return m_xmf4x4World; }

	void SetPosition(const float& x, const float& y, const float& z);
	void SetPosition(XMFLOAT3 xmf3Position);

	virtual void Rotate(const float& fPitch, const float& fYaw, const float& fRoll);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetChild(CGameObject* pChild);

	CGameObject* FindFrame(const std::string& strFrameName);

	void PrepareSkinning(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pRootObject);

	void LoadMaterialFromFile(std::ifstream& InFile);

public:
	static CGameObject* LoadHeirarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		std::ifstream& InFile, int& nFrames);
	
public:
	std::vector<CMaterial*> m_Materials;

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
//|||||||||||||||||||||||||||||||||||||||||||||||||| <CHeightMapTerrain> |||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class CHeightMapImage;

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	~CHeightMapTerrain();
private:
	CHeightMapImage* m_pHeightMapImage;

	int	m_nWidth;
	int	m_nLength;

	XMFLOAT3 m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false);
	XMFLOAT3 GetNormal(float x, float z);

	int GetRawImageWidth();
	int GetRawImageLength(); 

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

};
