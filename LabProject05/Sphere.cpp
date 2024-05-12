#include "stdafx.h"
#include "Sphere.h"

CSphere::CSphere(const CModelInfo& model) : CWeapon(model)
{
	type_ = WeaponType::Sphere;
}
