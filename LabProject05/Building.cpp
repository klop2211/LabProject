#include "stdafx.h"
#include "Building.h"
#include "Object.h"

CBuilding::CBuilding(const CModelInfo& model) : CRootObject(model)
{
	axis_transform_matrix_ = new XMFLOAT4X4
	(
		1, 0, 0, 0,
		0, 0, -1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	);

	collision_type_ = CollisionType::Static;
}
