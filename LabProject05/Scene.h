#pragma once

class CShader;
class CPlayer;
class CGameObject;
class CTexture;
class CCamera;
class CHeightMapTerrain;
class CDescriptorManager;

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

class CScene
{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return true; }
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return true; }

	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	// Srv 积己
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer);
	void ReleaseObjects();

	void BuildLightsAndMaterials();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput(UCHAR* pKeysBuffer) { return false; }
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	void ReleaseUploadBuffers();
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);

	CPlayer* m_pPlayer = NULL;

protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	// Shader 包府
	CShader** m_ppShaders = NULL;
	int	m_nShaders = 0;

	// GameObject 包府
	std::vector<CGameObject*> m_Objects;
	int	m_nObjects = 0;

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	UINT8* m_pcbMappedGameObjects = NULL;

	// Light 包府
	LIGHTS* m_pLights = NULL;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dLightsGPUDescriptorStartHandle;

	// Terrain
	CHeightMapTerrain* m_pTerrain = NULL;

	// Descriptor
	CDescriptorManager* m_pDescriptorManager = NULL;

};

