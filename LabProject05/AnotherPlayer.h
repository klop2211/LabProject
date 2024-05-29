#pragma once
#include "Player.h"

class CAnotherPlayer :
    public CPlayer
{

public:
    CAnotherPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const CModelInfo& model);

    virtual void HandleCollision(CRootObject* other, const CObbComponent& my_obb, const CObbComponent& other_obb) {}

};

