#pragma once

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

#include "Texture.h"

// ������ ������ Ư��
class CTextureProperty
{
private:
	std::string m_PropertyName;
	std::vector<CTexture> m_Textures; // �� Ư���� ǥ���ϱ����� �ؽ�ó��

public:
	CTextureProperty() {}
	// dds ���Ϸ� �ؽ�ó ���� �߰�
	CTextureProperty(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& strFileName, TextureType nResourceType, UINT nRootParameterIndex);
	// fbx ������ �ͽ���Ʈ�� bin ���� �ε�
	CTextureProperty(std::ifstream& InFile, const std::string& strName);
	~CTextureProperty() {}

	void LoadTexturePropertyFromFile(std::ifstream& InFile);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseUploadBuffers();

	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager);
};

//TODO: ShadingModel�� ��ǹ����� ��� ���Ǵ� �� ����
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