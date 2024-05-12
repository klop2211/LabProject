#pragma once
#include "Object.h"

class CRotationComponent;

class CMawang : public CGameObject
{

public:
	static const std::string mawang_model_file_name_;

	CRotationComponent* rotation_component_ = NULL;

	CMawang();
	CMawang(const CModelInfo& model);
	~CMawang();

	CRotationComponent* rotation_component() const { return rotation_component_; }

};

