#pragma once

#include "AnimationCallbackFunc.h"

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
	float weight_;

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

struct CallbackKey
{
	float time = 0.f;
	CAnimationCallbackFunc callback_func;
};

class CAnimationTrack
{
public:
	CAnimationTrack();
	CAnimationTrack(std::ifstream& InFile);
	~CAnimationTrack();

	bool IsEnable() { return enable_; }

	void set_enable(const bool& value) { enable_ = value; }
	void set_weight(const float& value) { weight_ = value; }

	float weight() const { return weight_; }

	void AddWeight(const float& value);
	void AddCallbackKey(const float& time, const CAnimationCallbackFunc& callback_func);

	void Animate(const float& fElapsedTime);

	void SetFrameCaches(CGameObject* pRootObject);
	void UpdateMatrix();

	void Stop() { speed_ = 0.f; }
	void Start() { speed_ = 1.f; }

private:
	void UpdatePosition(const float& fElapsedTime);

	// 콜백을 체크하는 시간 callback_key.time +- callback_check_time
	static const float callback_check_time_;

	float position_;
	float speed_;
	float weight_;

	bool enable_;

	AnimationLoopType m_LoopType;

	CAnimationSet* m_pAnimationSet;

	std::vector<CallbackKey> callback_keys_;
};

class CAnimationController
{
public:
	CAnimationController();
	~CAnimationController() {}

	void LoadAnimationFromFile(std::ifstream& InFile);

	void Animate(const float& fElapsedTime, CGameObject* pRootObject);

	void SetFrameCaches(CGameObject* pRootObject);

	void EnableTrack(const int& index);

	void ChangeAnimation(const int& index);

	void SetCallbackKey(const int& index, const float& time, const CAnimationCallbackFunc& callback_func);

private:
	const float animation_blend_speed_ = 3.f; // 애니메이션 교체시 교체 속도 단위 once/s

	bool is_animation_chainging_ = false;

	int prev_index_;
	int curr_index_;

	float m_fTime;

	std::vector<CAnimationTrack> animation_tracks_;

};

