#pragma once
#include "Animation.h"
#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

class CCamera;
class CMesh;
class CDescriptorManager;
class CMaterial;
//class CAnimationController;
class CGameObject;

struct CModelInfo
{
	CGameObject* heirarchy_root;
	std::unique_ptr<CAnimationController> animation_controller;
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||| <CGameObject> ||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class CGameObject
{	
private:
	int reference_count_ = 0;

protected:
	std::vector<CMaterial*> m_Materials;
	
	// 모델의 좌표축이 다이렉트 환경과 다를 경우 사용되는 행렬로 루트 오브젝트의 to_parent 행렬 앞에 이 행렬을 곱해준다(UpdateTransform 함수 참조)
	XMFLOAT4X4* axis_transform_matrix_;

	XMFLOAT4X4 m_xmf4x4World; // 최종적으로 셰이더에 입력되는 행렬(ToParent 행렬과 부모의 World 행렬의 곱의 결과이다)

	//Heirarchy 구조 관련 변수
	std::string m_strFrameName;

	XMFLOAT4X4 to_parent_matrix_;

	XMFLOAT3 m_xmf3Scale;
	XMFLOAT3 m_xmf3Rotation;
	XMFLOAT3 m_xmf3Translation;

	XMFLOAT3 m_xmf3BlendedScale;
	XMFLOAT3 m_xmf3BlendedRotation;
	XMFLOAT3 m_xmf3BlendedTranslation;

	CGameObject* parent_ = NULL;
	CGameObject* child_ = NULL;
	CGameObject* sibling_ = NULL;

	CMesh* m_pMesh = NULL;

	// 이 오브젝트가 사용하는 쉐이더 넘버
	int m_nShader = -1;

	// animation 관련
	CAnimationController* animation_controller_ = NULL;
	
	// 물리 옵션 적용관련 변수
	bool is_fall_ = false; //중력의 적용을 받는지

	// 객체의 렌더 여부
	bool is_visible_ = true;

public:
	CGameObject();
	~CGameObject();

	void AddRef() { reference_count_++; }
	void Release() { reference_count_--; if (reference_count_ <= 0) delete this; }

	bool CheckShader(const int& nShader) { return nShader == m_nShader; }

	//setter
	void set_is_visible(const bool& value) { is_visible_ = value; }
	void set_to_parent_matrix(const XMFLOAT4X4& value) { to_parent_matrix_ = value; }
	void set_is_fall(const bool& value) { is_fall_ = value; }
	void set_look_vector(const float& x, const float& y, const float& z);
	void set_look_vector(const XMFLOAT3& look) { set_look_vector(look.x, look.y, look.z); }
	void set_right_vector(const float& x, const float& y, const float& z);
	void set_right_vector(const XMFLOAT3& right) { set_right_vector(right.x, right.y, right.z); }
	void set_up_vector(const float& x, const float& y, const float& z);
	void set_up_vector(const XMFLOAT3& up) { set_up_vector(up.x, up.y, up.z); }
	void set_position_vector(const float& x, const float& y, const float& z);
	void set_position_vector(const XMFLOAT3& position){ set_position_vector(position.x, position.y, position.z); }
	void set_animation_controller(CAnimationController* value) { animation_controller_ = value; }
	void set_child(CGameObject* pChild);
	void set_sibling(CGameObject* ptr);
	void set_parent(CGameObject* ptr);
	virtual void SetMesh(CMesh* pMesh);
	void SetShader(const int& nShader); 
	void SetMaterial(const int& index, CMaterial* pMaterial);
	void SetBlendedScale(const XMFLOAT3& xmf3Value) { m_xmf3BlendedScale = xmf3Value; }
	void SetBlendedRotation(const XMFLOAT3& xmf3Value) { m_xmf3BlendedRotation = xmf3Value; }
	void SetBlendedTranslation(const XMFLOAT3& xmf3Value) { m_xmf3BlendedTranslation = xmf3Value; }
	void SetScale(const XMFLOAT3& xmf3Value) { m_xmf3Scale = xmf3Value; }
	void SetRotation(const XMFLOAT3& xmf3Value) { m_xmf3Rotation = xmf3Value; }
	void SetTranslation(const XMFLOAT3& xmf3Value) { m_xmf3Translation = xmf3Value; }

	//getter
	bool is_visible() const { return is_visible_; }
	XMFLOAT4X4 to_parent_matrix() const { return to_parent_matrix_; }
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 up_vector() const;
	XMFLOAT3 right_vector() const;
	bool is_fall() const { return is_fall_; }
	CGameObject* child() const { return child_; }
	CGameObject* sibling() const { return sibling_; }
	XMFLOAT4X4& GetWorldMatrix() { return m_xmf4x4World; }
	XMFLOAT3 GetScale() const { return m_xmf3Scale; }
	XMFLOAT3 GetRotation() const { return m_xmf3Rotation; }
	XMFLOAT3 GetTranslation() const { return m_xmf3Translation; }
	XMFLOAT3 GetBlendedScale() const { return m_xmf3BlendedScale; }
	XMFLOAT3 GetBlendedRotation() const { return m_xmf3BlendedRotation; }
	XMFLOAT3 GetBlendedTranslation() const { return m_xmf3BlendedTranslation; }

	void UpdateMatrixByBlendedSRT();

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager);

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, int shader_num = -1);
	void ResetAnimatedSRT();

	virtual void ReleaseUploadBuffers();

	virtual void HandleCollision(CGameObject* other) {}

	CGameObject* FindFrame(const std::string& strFrameName);
	void PrepareSkinning(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pRootObject);

	CGameObject* AddSocket(const std::string& frame_name);

	//모델 파일 로드 관련 함수
	void LoadMaterialFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::ifstream& InFile);
	static CModelInfo LoadModelInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		const std::string& heirarchy_file_name);
	static CAnimationController* LoadAnimationFromFile(std::ifstream& file, CGameObject* root_object);
	static CGameObject* LoadHeirarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		std::ifstream& InFile, int& nFrames);

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

	virtual void HandleCollision(CGameObject* other) override {}

};

