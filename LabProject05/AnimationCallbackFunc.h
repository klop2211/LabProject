#pragma once

class CAudioManager;

class CAnimationCallbackFunc
{
	//CAnimationCallbackFunc* func_ = NULL;

public:
	CAnimationCallbackFunc() {}
	~CAnimationCallbackFunc();

	virtual void Util() {}
	void operator()() { Util(); }
};


class CSoundCallbackFunc : public CAnimationCallbackFunc
{
	std::string track_name_;

	//���� ���� ������� �����ϴ� ����� �Ŵ���
	CAudioManager* audio_manager_;

public:
	CSoundCallbackFunc(CAudioManager* audio_manager, const std::string& track_name);
	~CSoundCallbackFunc() {}

	virtual void Util();
};
