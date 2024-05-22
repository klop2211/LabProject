#include "stdafx.h"
#include "ObbComponent.h"
#include "Object.h"

CObbComponent::CObbComponent(CGameObject* owner, const BoundingBox& aabb, CGameObject* parent_socket) : CComponent(owner)
{
	BoundingOrientedBox::CreateFromBoundingBox(obb_, aabb);
	parent_socket_ = parent_socket;
}

void CObbComponent::Update()
{
	if (parent_socket_)
	{
		obb_.Transform(animated_obb_, XMLoadFloat4x4(&parent_socket_->GetWorldMatrix()));
	}
}

bool CObbComponent::Intersects(const BoundingOrientedBox& other)
{
	return animated_obb_.Intersects(other);
}
