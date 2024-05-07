#include "stdafx.h"
#include "Mawang.h"
#include "Animation.h"

const std::string CMawang::mawang_model_file_name_ = "../Resource/Model/Mawang_Model.bin";
//const std::string CMawang::mawang_model_file_name_ = "../Resource/Model/Player_Model.bin";

CMawang::CMawang()
{
}

CMawang::CMawang(const CModelInfo& model)
{
	set_child(model.heirarchy_root);

	animation_controller_ = model.animation_controller;

	animation_controller_->SetFrameCaches(this);

	animation_controller_->EnableTrack(0);

	axis_transform_matrix_ = new XMFLOAT4X4
	(1, 0, 0, 0,
		0, 0, -1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1);

	SetShader(4);
}

CMawang::~CMawang()
{
}
