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

// 모델의 계층구조를 표현하는 class
class CFrame
{
protected:
	// 최종적으로 셰이더에 입력되는 행렬(ToParent 행렬과 부모의 World 행렬의 곱의 결과이다)
	XMFLOAT4X4 world_matrix_;

	XMFLOAT4X4 to_parent_matrix_;

	//Heirarchy 구조 관련 변수
	std::string frame_name_;

	XMFLOAT3 scale_;
	XMFLOAT3 rotation_;
	XMFLOAT3 translation_;

	//애니메이션 블렌딩을 위한 SRT
	XMFLOAT3 blended_scale_;
	XMFLOAT3 blended_rotation_;
	XMFLOAT3 blended_translation_;

	std::unique_ptr<CFrame> parent_ = NULL;
	std::unique_ptr<CFrame> child_ = NULL;
	std::unique_ptr<CFrame> sibling_ = NULL;

public:
	CFrame();
	~CFrame();

	//setter
	void set_frame_name(const std::string& value) { frame_name_ = value; }
	void set_to_parent_matrix(const XMFLOAT4X4& value) { to_parent_matrix_ = value; }
	void set_blended_scale(const XMFLOAT3& xmf3Value) { blended_scale_ = xmf3Value; }
	void set_blended_rotation(const XMFLOAT3& xmf3Value) { blended_rotation_ = xmf3Value; }
	void set_blended_translation(const XMFLOAT3& xmf3Value) { blended_translation_ = xmf3Value; }
	void set_scale(const XMFLOAT3& xmf3Value) { scale_ = xmf3Value; }
	void set_rotation(const XMFLOAT3& xmf3Value) { rotation_ = xmf3Value; }
	void set_translation(const XMFLOAT3& xmf3Value) { translation_ = xmf3Value; }
	void set_child(CFrame* pChild);
	void set_sibling(CFrame* ptr);
	void set_parent(CFrame* ptr);
	void set_look_vector(const float& x, const float& y, const float& z);
	void set_look_vector(const XMFLOAT3& look) { set_look_vector(look.x, look.y, look.z); }
	void set_right_vector(const float& x, const float& y, const float& z);
	void set_right_vector(const XMFLOAT3& right) { set_right_vector(right.x, right.y, right.z); }
	void set_up_vector(const float& x, const float& y, const float& z);
	void set_up_vector(const XMFLOAT3& up) { set_up_vector(up.x, up.y, up.z); }
	
	//getter
	CFrame* child() const { return child_.get(); }
	CFrame* sibling() const { return sibling_.get(); }
	XMFLOAT4X4& world_matrix() { return world_matrix_; }
	XMFLOAT4X4 to_parent_matrix() const { return to_parent_matrix_; }
	XMFLOAT3 scale() const { return scale_; }
	XMFLOAT3 rotation() const { return rotation_; }
	XMFLOAT3 translation() const { return translation_; }
	XMFLOAT3 blended_scale() const { return blended_scale_; }
	XMFLOAT3 blended_rotation() const { return blended_rotation_; }
	XMFLOAT3 blended_translation() const { return blended_translation_; }
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 up_vector() const;
	XMFLOAT3 right_vector() const;


	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);


};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||| <CGameObject> ||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class CGameObject : public CFrame
{	
private:
	int reference_count_ = 0;

protected:
	std::vector<CMaterial*> m_Materials;
	
	CMesh* m_pMesh = NULL;

	// 모델의 좌표축이 다이렉트 환경과 다를 경우 사용되는 행렬로 루트 오브젝트의 to_parent 행렬 앞에 이 행렬을 곱해준다(UpdateTransform 함수 참조)
	XMFLOAT4X4* axis_transform_matrix_;

	// 이 오브젝트가 사용하는 쉐이더 넘버
	int shader_num_ = -1;

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

	bool CheckShader(const int& nShader) { return nShader == shader_num_; }

	//setter
	void set_is_visible(const bool& value) { is_visible_ = value; }
	void set_is_fall(const bool& value) { is_fall_ = value; }
	void set_position_vector(const float& x, const float& y, const float& z);
	void set_position_vector(const XMFLOAT3& position){ set_position_vector(position.x, position.y, position.z); }
	void set_animation_controller(CAnimationController* value) { animation_controller_ = value; }
	virtual void SetMesh(CMesh* pMesh);
	void SetShader(const int& nShader); 
	void SetMaterial(const int& index, CMaterial* pMaterial);

	//getter
	int shader_num() const { return shader_num_; }
	bool is_visible() const { return is_visible_; }
	bool is_fall() const { return is_fall_; }

	void UpdateMatrixByBlendedSRT();

	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);
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

