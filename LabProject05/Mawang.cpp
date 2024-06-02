#include "stdafx.h"
#include "Mawang.h"
#include "Animation.h"

const std::string CMawang::mawang_model_file_name_ = "../Resource/Model/Mawang_Model.bin";

CMawang::CMawang()
{
}

CMawang::CMawang(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const CModelInfo& model) : CRootObject(model)
{
	CreateBoneTransformMatrix(device, command_list);

	axis_transform_matrix_ = new XMFLOAT4X4
	(1, 0, 0, 0,
		0, 0, -1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1);

	collision_type_ = CollisionType::Dynamic;

	animation_controller_->SetLoopType(0, AnimationLoopType::Repeat);
	animation_controller_->SetDeadCallbackKey(1, this);

	SetShader(4);
}

void CMawang::PlayDeadAnimation()
{
	animation_controller_->ChangeAnimation(1);
}

CMawang::~CMawang()
{
}

void CMawang::TakeDamage(float damage_amount, const CDamageEvent& damage_event, CRootObject* instigator, CRootObject* damage_causer)
{
	hp_ -= damage_amount;
	if(hp_ <= 0.f)
		PlayDeadAnimation();
}