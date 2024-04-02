#pragma once

class CGameObject;

class CAnimationCurve
{
public:
	CAnimationCurve();
	CAnimationCurve(std::ifstream& InFile);
	~CAnimationCurve() {}

	void GetAnimatedSRT(const float& fPosition, const float& fLayerWeight,
		XMFLOAT3* pScale, XMFLOAT3* pRotate, XMFLOAT3* pTranslation);

public:
	std::string m_strFrameName;

private:
	void LoadKeys(std::ifstream& InFile, std::vector<XMFLOAT3>& vecKeyValues);

	std::vector<float> m_KeyTimes;
	std::vector<XMFLOAT3> m_Scales;
	std::vector<XMFLOAT3> m_Rotates;
	std::vector<XMFLOAT3> m_Translations;
};

class CAnimationLayer
{
public:
	CAnimationLayer();
	CAnimationLayer(std::ifstream& InFile);
	~CAnimationLayer() {}

	void SetFrameCaches(CGameObject* pRootObject);
	std::vector<CGameObject*>& GetFrameCaches() { return m_FrameCaches; }

	void UpdateFrameCachesSRT(const float& fPosition);

private:
	float m_fWeight;

	std::vector<CAnimationCurve> m_Curves;
	std::vector<CGameObject*> m_FrameCaches;
};

class CAnimationSet
{
public:
	CAnimationSet();
	CAnimationSet(const float& fStartTime, const float& fEndTime, const std::string& strAnimationName);
	~CAnimationSet(){}

	void LoadLayersFromFile(std::ifstream& InFile);

	float GetEndTime() const { return m_fEndTime; }
	void SetFrameCaches(CGameObject* pRootObject);

	void Animate(const float& fPosition, const float& fTrackWeight);
	void UpdateMatrix();

private:
	float m_fStartTime;
	float m_fEndTime;

	std::string m_strAnimationName;

	std::vector<CAnimationLayer> m_Layers;
};

enum class AnimationLoopType{ Once, Repeat };

class CAnimationTrack
{
public:
	CAnimationTrack();
	CAnimationTrack(std::ifstream& InFile);
	~CAnimationTrack();

	bool IsEnable() { return m_bEnable; }

	void Animate(const float& fElapsedTime);

	void SetFrameCaches(CGameObject* pRootObject);
	void UpdateMatrix();

private:
	void UpdatePosition(const float& fElapsedTime);

	float m_fPosition;
	float m_fSpeed;
	float m_fWeight;

	bool m_bEnable;

	AnimationLoopType m_LoopType;

	CAnimationSet* m_pAnimationSet;
};

class CAnimationController
{
public:
	CAnimationController();
	~CAnimationController() {}

	void LoadAnimationFromFile(std::ifstream& InFile);

	void Animate(const float& fElapsedTime, CGameObject* pRootObject);

	void SetFrameCaches(CGameObject* pRootObject);

private:

	float m_fTime;

	std::vector<CAnimationTrack> m_Tracks;
	
};
