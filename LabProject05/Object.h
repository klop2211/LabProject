#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

class CCamera;
class CMesh;
class CGameObject;
class CDescriptorManager;
class CMaterial;
class CAnimationController;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||| <CGameObject> ||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class CGameObject
{	
protected:
	std::vector<CMaterial*> m_Materials;

	XMFLOAT4X4 m_xmf4x4World; // 최종적으로 셰이더에 입력되는 행렬(ToParent 행렬과 부모의 World 행렬의 곱의 결과이다)

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

	// 이 오브젝트가 사용하는 쉐이더 넘버
	int m_nShader = -1;

	CAnimationController* m_pAnimationController = NULL;

public:
	CGameObject();
	~CGameObject();

	bool CheckShader(const int& nShader) { return nShader == m_nShader; }

	virtual void SetMesh(CMesh* pMesh);
	void SetShader(const int& nShader) { m_nShader = nShader; }
	void SetMaterial(const int& index, CMaterial* pMaterial);
	void SetPosition(const float& x, const float& y, const float& z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetChild(CGameObject* pChild);
	void SetBlendedScale(const XMFLOAT3& xmf3Value) { m_xmf3BlendedScale = xmf3Value; }
	void SetBlendedRotation(const XMFLOAT3& xmf3Value) { m_xmf3BlendedRotation = xmf3Value; }
	void SetBlendedTranslation(const XMFLOAT3& xmf3Value) { m_xmf3BlendedTranslation = xmf3Value; }
	void SetScale(const XMFLOAT3& xmf3Value) { m_xmf3Scale = xmf3Value; }
	void SetRotation(const XMFLOAT3& xmf3Value) { m_xmf3Rotation = xmf3Value; }
	void SetTranslation(const XMFLOAT3& xmf3Value) { m_xmf3Translation = xmf3Value; }

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	XMFLOAT4X4& GetWorldMatrix() { return m_xmf4x4World; }
	XMFLOAT3 GetScale() const { return m_xmf3Scale; }
	XMFLOAT3 GetRotation() const { return m_xmf3Rotation; }
	XMFLOAT3 GetTranslation() const { return m_xmf3Translation; }
	XMFLOAT3 GetBlendedScale() const { return m_xmf3BlendedScale; }
	XMFLOAT3 GetBlendedRotation() const { return m_xmf3BlendedRotation; }
	XMFLOAT3 GetBlendedTranslation() const { return m_xmf3BlendedTranslation; }

	void UpdateMatrixByBlendedSRT();

	//TODO: Rotation Component를 제작해서 사용하자
	virtual void Rotate(const float& fPitch, const float& fYaw, const float& fRoll);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CDescriptorManager* pDescriptorManager);

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ResetAnimatedSRT();

	virtual void ReleaseUploadBuffers();

	CGameObject* FindFrame(const std::string& strFrameName);

	void PrepareSkinning(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pRootObject);

	//모델 파일 로드 관련 함수
	void LoadMaterialFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::ifstream& InFile);

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

};
