#pragma once
#include "Object.h"

class CMawang : public CRootObject
{
private:
	float hp_ = 10;

public:
	static const std::string mawang_model_file_name_;

	CMawang();
	CMawang(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const CModelInfo& model);
	~CMawang();

	virtual void TakeDamage(float damage_amount, const CDamageEvent& damage_event, CRootObject* instigator, CRootObject* damage_causer) override;

	void PlayDeadAnimation();

};

