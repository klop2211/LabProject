#pragma once
#include "Object.h"

#define MAWANG_ANIMATION_COUNT 1

class CMawang : public CGameObject
{
	const std::string mawang_model_file_name_ = "../Resource/Model/Mawang_model.bin";
	const std::array<std::string, MAWANG_ANIMATION_COUNT> mawang_animation_file_names_
	{
		"../Resource/Model/Mawang_Stretching.bin"
	};



public:
	CMawang();
	CMawang(const CMawang& other);
	~CMawang();


};

