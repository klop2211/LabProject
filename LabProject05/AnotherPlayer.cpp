#include "stdafx.h"
#include "AnotherPlayer.h"
#include "Material.h"
#include "Shader.h"


CAnotherPlayer::CAnotherPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const CModelInfo& model)
{
	model.heirarchy_root->AddRef();
	set_child(model.heirarchy_root);

	CreateBoneTransformMatrix(device, command_list);

	animation_controller_ = new CAnimationController(*model.animation_controller);
	
	animation_controller_->SetFrameCaches(this);

	animation_controller_->EnableTrack(0);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Idle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::SwordIdle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::SphereIdle, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Walk, AnimationLoopType::Repeat);
	animation_controller_->SetLoopType((int)PlayerAnimationState::Run, AnimationLoopType::Repeat);

	CMaterial* Material = new CMaterial(1);
	Material->AddTexturePropertyFromDDSFile(device, command_list, "../Resource/Model/Texture/uv.png", TextureType::RESOURCE_TEXTURE2D, 7);

	model.heirarchy_root->SetMaterial(0, Material);

	axis_transform_matrix_ = new XMFLOAT4X4
	(1, 0, 0, 0,
		0, 0, -1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1);

	CreateShaderVariables(device, command_list);

	SetShader((int)ShaderNum::Standard);

}