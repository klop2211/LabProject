#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Player.h"
#include "Object.h"
#include "DescriptorManager.h"
#include "AudioManager.h"
#include "AnimationCallbackFunc.h"

CScene::CScene()
{

}

CScene::~CScene()
{
}


void CScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 50.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

}


#define _WITH_OBJECT_LIGHT_MATERIAL_DESCRIPTOR_TABLE

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[4];
	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	pd3dDescriptorRanges[0].BaseShaderRegister = 4; //Lights
	pd3dDescriptorRanges[0].NumDescriptors = 1;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	pd3dDescriptorRanges[1].BaseShaderRegister = 17; //Texture(detail)
	pd3dDescriptorRanges[1].NumDescriptors = 1;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	pd3dDescriptorRanges[2].BaseShaderRegister = 18; //Texture(base)
	pd3dDescriptorRanges[2].NumDescriptors = 1;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	pd3dDescriptorRanges[3].BaseShaderRegister = 0; //Texture(DiffuseColor)
	pd3dDescriptorRanges[3].NumDescriptors = 1;

	D3D12_ROOT_PARAMETER pd3dRootParameters[8];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 16; //Game Objects
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1; //Lights
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1; //Terrain Textures
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 5; //BoneOffsets
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 6; //BoneTransforms
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1; //Terrain Textures
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1; //DiffuseColor Texuture
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3];
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	D3D12_STATIC_SAMPLER_DESC d3dSamplerDescs;
	d3dSamplerDescs.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDescs.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDescs.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDescs.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDescs.MipLODBias = 0;
	d3dSamplerDescs.MaxAnisotropy = 1;
	d3dSamplerDescs.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDescs.MinLOD = 0;
	d3dSamplerDescs.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDescs.ShaderRegister = 0;
	d3dSamplerDescs.RegisterSpace = 0;
	d3dSamplerDescs.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}


void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 조명정보
	UINT ncbLightsBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	d3d12_lights_ = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbLightsBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	d3d12_lights_->Map(0, NULL, (void**)&mapped_lights_);

	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dcbvDesc;
	d3dcbvDesc.BufferLocation = d3d12_lights_->GetGPUVirtualAddress();
	d3dcbvDesc.SizeInBytes = ncbLightsBytes;
	pd3dDevice->CreateConstantBufferView(&d3dcbvDesc, descriptor_manager_->GetCbvNextCPUHandle());
	d3d12_lights_gpu_descriptor_start_handle_ = descriptor_manager_->GetCbvNextGPUHandle();

	descriptor_manager_->AddCbvIndex();
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(mapped_lights_, m_pLights, sizeof(LIGHTS));
}

void CScene::ReleaseShaderVariables()
{
	if (d3d12_lights_)
	{
		d3d12_lights_->Unmap(0, NULL);
		d3d12_lights_->Release();
	}
}

void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice)
{
	player_->CreateShaderResourceViews(pd3dDevice, descriptor_manager_);

	terrain_->CreateShaderResourceViews(pd3dDevice, descriptor_manager_);

	for (auto& Object : objects_)
		Object->CreateShaderResourceViews(pd3dDevice, descriptor_manager_);
}

void CScene::ReleaseUploadBuffers()
{
	for (auto& pObject : objects_) pObject->ReleaseUploadBuffers();
}

void CScene::AnimateObjects(float elapsed_time)
{
	//player_->OnPrepareRender();

	CollisionCheck();

	player_->Animate(elapsed_time);

	terrain_->Animate(elapsed_time);

	for (auto& pObject : objects_) 
		pObject->Animate(elapsed_time);

	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = player_->position_vector();
		m_pLights->m_pLights[1].m_xmf3Direction = player_->look_vector();
	}
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer, CAudioManager* audio_manager)
{
	audio_manager_ = audio_manager;

	player_ = pPlayer;

	d3d12_root_signature_ = CreateGraphicsRootSignature(pd3dDevice);

	int shader_num = 2;			// 조명 O, X 각각 1개씩
	shaders_.reserve(shader_num);

	shaders_.emplace_back(new CStandardShader);
	shaders_.emplace_back(new CTerrainShader);

	for(auto& shader : shaders_)
		shader->CreateShader(pd3dDevice, d3d12_root_signature_);


	descriptor_manager_ = new CDescriptorManager;
	descriptor_manager_->SetDescriptors(1 + 4); // 조명(cbv), 텍스처

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = descriptor_manager_->GetDescriptors();		
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	ID3D12DescriptorHeap* pd3dDescritorHeap;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&pd3dDescritorHeap);

	descriptor_manager_->SetDesriptorHeap(pd3dDescritorHeap);
	descriptor_manager_->Initialization(1);

	//TODO: Light 관련 차후 수정
	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	XMFLOAT3 xmf3Scale(400.0f, 40.0f, 400.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.0f, 0.0f, 0.0f);
	terrain_ = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, d3d12_root_signature_, _T("../Resource/Terrain/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);


	player_->SetShader(4);
	player_->set_position_vector(500, terrain_->GetHeight(500, 500), 500);
	((CEllenPlayer*)player_)->SetAnimationCallbackKey((int)EllenAnimationState::Run, 0.1, new CSoundCallbackFunc(audio_manager_, "Footstep01"));
	
	int object_num = 0; // 04.30 수정: 플레이어 객체와 터레인 객체는 따로관리(충돌체크 관리를 위해)

	CreateShaderResourceViews(pd3dDevice); // 모든 오브젝트의 Srv 생성
}

void CScene::ReleaseObjects()
{
	if (d3d12_root_signature_) d3d12_root_signature_->Release();

	delete player_;
	player_ = NULL;

	delete terrain_;
	terrain_ = NULL;

	for (auto& pObject : objects_)
	{
		delete pObject;
		pObject = NULL;
	}

	for (auto& shader : shaders_)
	{
		shader->Release();
		shader = NULL;
	}

	if (descriptor_manager_) delete descriptor_manager_;

	ReleaseShaderVariables();

	if (m_pLights) delete m_pLights;
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(d3d12_root_signature_);
	pd3dCommandList->SetDescriptorHeaps(1, &descriptor_manager_->GetDescriptorHeap());

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

#ifdef _WITH_OBJECT_LIGHT_MATERIAL_DESCRIPTOR_TABLE
	//pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dMaterialsCbvGPUDescriptorHandle);
#else
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = d3d12_lights_->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress);
#endif

	for (int i = 0; i < shaders_.size(); i++)
	{
		shaders_[i]->Render(pd3dCommandList);
		for (auto& pObject: objects_)
		{
			if (pObject->CheckShader(shaders_[i]->GetShaderNum()))
			{
				pObject->Render(pd3dCommandList, pCamera);
			}
		}
		if (shaders_[i]->GetShaderNum() == (int)ShaderNum::Standard)
			player_->Render(pd3dCommandList, pCamera);
		if (shaders_[i]->GetShaderNum() == (int)ShaderNum::Terrain)
			terrain_->Render(pd3dCommandList, pCamera);
	}


}

void CScene::UpdateCollisionList()
{
	collision_list_.clear();

	for (auto& object : objects_)
	{
		if (Vector3::Length(player_->position_vector() - object->position_vector()) < 30000.f) // 300미터보다 가까이 있는 객체
			collision_list_.push_back(object);
	}
}

void CScene::CollisionCheck()
{
	// 지면에 닿아있는지 체크
	XMFLOAT3 position = player_->position_vector();
	float terrain_height = terrain_->GetHeight(position.x, position.z);
	if (position.y > terrain_height)
		player_->set_is_fall(true);
	if (IsEqual(position.y, terrain_height))
		player_->set_is_fall(false);
	if (position.y < terrain_height)
	{
		player_->set_position_vector(position.x, terrain_height, position.z);
		player_->set_is_fall(false);
	}
		

	//TODO: 객체의 OBB를 이용해서 서로 겹치면 밀어내는 기본 충돌처리 구현
}

