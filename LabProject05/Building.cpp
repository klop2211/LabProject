#include "stdafx.h"
#include "Building.h"

CBuilding::CBuilding(const CModelInfo& model)
{
	set_child(model.heirarchy_root);

	axis_transform_matrix_ = new XMFLOAT4X4
	(
		1, 0, 0, 0,
		0, 0, -1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	);
}
