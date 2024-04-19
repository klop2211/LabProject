#pragma once

#include "Timer.h"

class CScene;
class CPlayer;
class CCamera;

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreateCommandQueueAndList();

	void OnResizeBackBuffers();
	void ChangeSwapChainState();

    void BuildObjects();
    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					hinstance_;
	HWND						hwnd_; 

	int							client_width_;
	int							client_height_;
        
	IDXGIFactory4				*dxgi_factory_ = NULL;
	IDXGISwapChain3				*dxgi_swap_chain_ = NULL;
	ID3D12Device				*d3d12_device_ = NULL;

	bool						msaa_4x_enable_ = false;
	UINT						msaa_4x_quality_levels_ = 0;

	static const UINT			swap_chain_buffers_ = 2;
	UINT						swap_chain_buffer_index_;

	std::array<ID3D12Resource*, swap_chain_buffers_> d3d12_swap_chain_back_buffers_;
	ID3D12DescriptorHeap		*d3d12_render_target_descriptor_heap_ = NULL;
	UINT						render_target_view_descriptor_increment_size_;

	ID3D12Resource				*d3d12_depth_stencil_buffer_ = NULL;
	ID3D12DescriptorHeap		*d3d12_depth_stencil_descriptor_heap_ = NULL;
	UINT						depth_stencil_view_descriptor_increment_size_;

	ID3D12CommandAllocator		*d3d12_command_allocator_ = NULL;
	ID3D12CommandQueue			*d3d12_command_queue_ = NULL;
	ID3D12GraphicsCommandList	*d3d12_command_list_ = NULL;

	ID3D12Fence					*d3d12_fence_ = NULL;
	std::array<UINT64, swap_chain_buffers_>	d3d12_fence_valeues_;
	HANDLE						fence_event_handle_;

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController;
#endif

	CGameTimer					game_timer_;

	CScene						*scene_ = NULL;
	CPlayer						*player_ = NULL;
	CCamera						*camera_ = NULL;

	POINT						old_cursor_position_;

	_TCHAR						m_pszFrameRate[50];
};

