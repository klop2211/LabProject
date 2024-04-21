#include "stdafx.h"
#include "Mawang.h"

CMawang::CMawang()
{
}

CMawang::CMawang(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{

}

CMawang::CMawang(const CMawang& other)
{
	child_ = other.child_;
}

CMawang::~CMawang()
{
}
