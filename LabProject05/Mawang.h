#pragma once
#include "Object.h"

class CMawang : public CGameObject
{

public:
	static const std::string mawang_model_file_name_;

	CMawang();
	CMawang(const CModelInfo& model);
	~CMawang();


};

