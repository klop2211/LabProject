#pragma once

class CAnimationCurve
{
public:
	CAnimationCurve();
	CAnimationCurve(std::ifstream& InFile);
	~CAnimationCurve() {}



private:
	void LoadKeys(std::ifstream& InFile, std::vector<XMFLOAT3>& vecKeyValues);


	std::string m_strFrameName;
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

private:
	float m_fWeight;

	std::vector<CAnimationCurve> m_Curves;
};

class CAnimationSet
{
public:
	CAnimationSet();
	CAnimationSet(const float& fStartTime, const float& fEndTime, const std::string& strAnimationName);
	~CAnimationSet(){}

	void LoadLayersFromFile(std::ifstream& InFile);

private:
	float m_fStartTime;
	float m_fEndTime;

	std::string m_strAnimationName;

	std::vector<CAnimationLayer> m_Layers;
};

class CAnimationTrack
{
public:
	CAnimationTrack();
	CAnimationTrack(std::ifstream& InFile);
	~CAnimationTrack();

private:
	float m_fPosition;
	float m_fSpeed;
	float m_fWeight;

	bool m_bEnable;

	CAnimationSet* m_pAnimationSet;
};

class CAnimationController
{
public:
	CAnimationController();
	~CAnimationController() {}

	void LoadAnimationFromFile(std::ifstream& InFile);

private:
	float m_fTime;
	std::vector<CAnimationTrack> m_Tracks;
	
};

