#pragma once
#include "Object.h"

#define MAWANG_ANIMATION_COUNT 1

class CMawang : public CGameObject
{
	const std::string mawang_model_file_name_ = "../Resource/Model/mawang_model.bin";
	const std::array<std::string, MAWANG_ANIMATION_COUNT> mawang_animation_file_names_
	{
		"../Resource/Model/mawang_stretching.bin"
	};



public:
	CMawang();
	CMawang(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);
	CMawang(const CMawang& other);
	~CMawang();


};

