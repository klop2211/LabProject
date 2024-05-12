#pragma once

class CShader;
class CPlayer;
class CGameObject;
class CTexture;
class CCamera;
class CHeightMapTerrain;
class CDescriptorManager;
class CAudioManager;
class CSkyBox;

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

	// Srv 생성
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer, CAudioManager* audio_manager);
	void ReleaseObjects();

	void BuildLightsAndMaterials();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(d3d12_root_signature_); }

	bool ProcessInput(UCHAR* pKeysBuffer) { return false; }
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	void ReleaseUploadBuffers();

	// 충돌 처리
	void UpdateCollisionList();
	void CollisionCheck();

protected:
	ID3D12RootSignature* d3d12_root_signature_ = NULL;

	// Shader 관리
	std::vector<CShader*> shaders_;

	// Terrain
	CHeightMapTerrain* terrain_ = NULL;

	// Player
	CPlayer* player_ = NULL;

	// SkyBox
	CSkyBox* skybox_ = NULL;

	// GameObject 관리 
	// 05.09 수정: 생성과 삭제가 빈번이 일어날 확률이 높은 씬의 오브젝트들은 벡터보다 리스트를 쓰는 것이 좋다고 판단 + 정렬도 필요없음
	// 05.13 수정: 현재는 생성과 삭제를 하지 않아 다시 vector로 변경
	std::vector<CGameObject*> objects_;

	// 충돌 관리
	std::list<CGameObject*> collision_list_;

	//TODO: 조명관련 처리 필요
	LIGHTS* m_pLights = NULL;

	ID3D12Resource* d3d12_lights_ = NULL;
	LIGHTS* mapped_lights_ = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE d3d12_lights_gpu_descriptor_start_handle_;


	// Descriptor
	CDescriptorManager* descriptor_manager_ = NULL;

	// AudioManager
	CAudioManager* audio_manager_ = NULL;

};

