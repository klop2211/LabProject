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

	// Srv ����
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer, CAudioManager* audio_manager);
	void ReleaseObjects();

	void BuildLightsAndMaterials();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(d3d12_root_signature_); }

	bool ProcessInput(UCHAR* pKeysBuffer) { return false; }
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, float elapsed_time = 0);

	void ReleaseUploadBuffers();

	// �浹 ó��
	void UpdateCollisionList();
	void CollisionCheck();

protected:
	ID3D12RootSignature* d3d12_root_signature_ = NULL;

	// Shader ����
	std::vector<CShader*> shaders_;

	// Terrain
	CHeightMapTerrain* terrain_ = NULL;

	// Player
	CPlayer* player_ = NULL;

	// SkyBox
	CSkyBox* skybox_ = NULL;

	// GameObject ���� 
	// 05.09 ����: ������ ������ ����� �Ͼ Ȯ���� ���� ���� ������Ʈ���� ���ͺ��� ����Ʈ�� ���� ���� ���ٰ� �Ǵ� + ���ĵ� �ʿ����
	std::list<CGameObject*> objects_;

	// �浹 ����
	std::list<CGameObject*> collision_list_;

	//TODO: ������� ó�� �ʿ�
	LIGHTS* m_pLights = NULL;

	ID3D12Resource* d3d12_lights_ = NULL;
	LIGHTS* mapped_lights_ = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE d3d12_lights_gpu_descriptor_start_handle_;


	// Descriptor
	CDescriptorManager* descriptor_manager_ = NULL;

	// AudioManager
	CAudioManager* audio_manager_ = NULL;

};

