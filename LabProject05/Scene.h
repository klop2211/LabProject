#pragma once
#include "Player.h"

class CShader;

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

	// Gameobject의 월드 변환 행렬 및 재질, Light, Material 정보 등 DescriptorTable을 통해 업데이트 해주기 위한 버퍼를 생성하고 관리
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	// srv 생성 관리
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nRootParameterStartIndex);


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


	ID3D12Resource* m_pd3dcbPlayer = NULL;
	CB_PLAYER_INFO* m_pcbMappedPlayer = NULL;

	// Shader 관리
	CShader** m_ppShaders = NULL;
	int	m_nShaders = 0;

	// GameObject 관리
	CGameObject** m_ppObjects = NULL;
	int	m_nObjects = 0;

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	UINT8* m_pcbMappedGameObjects = NULL;

	// Light 관리
	LIGHTS* m_pLights = NULL;

	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dLightsGPUDescriptorStartHandle;

	// Terrain
	CHeightMapTerrain* m_pTerrain = NULL;

	// Descriptor
	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCbvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dCbvGPUDescriptorNextHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dSrvGPUDescriptorNextHandle;


};

