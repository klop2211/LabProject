#pragma once
#include "Component.h"

class CObbComponent :
    public CComponent
{
private:
    BoundingOrientedBox obb_;
    BoundingOrientedBox animated_obb_;
    CGameObject* parent_socket_ = NULL;
    //CCubeMesh* debug_cube_ = NULL;

public:
    CObbComponent() {}
    CObbComponent(CGameObject* owner, const BoundingBox& aabb, CGameObject* parent_socket);
    ~CObbComponent() {}

    //getter
    BoundingOrientedBox animated_obb() const { return animated_obb_; }

    void Update();

    bool Intersects(const BoundingOrientedBox& other);

};

