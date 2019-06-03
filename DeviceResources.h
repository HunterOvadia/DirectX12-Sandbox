#pragma once

class BaseWindow;
using namespace Microsoft::WRL;

namespace DX
{
	static const UINT32 FrameCount = 2; // Triple Buffer

	class DeviceResources
	{
	public:
		DeviceResources();
		
		void LoadPipeline(BaseWindow *Window);
		UINT64 Signal(ComPtr<ID3D12CommandQueue> InCommandQueue, ComPtr<ID3D12Fence> InFence, UINT64& InFenceValue);
		void WaitForFenceValue(ComPtr<ID3D12Fence> InFence, uint64_t InFenceValue, HANDLE InFenceEvent, std::chrono::milliseconds Duration);
		void Flush();

	public:
		inline HANDLE GetFenceEvent() const { return FenceEvent; }
		inline ComPtr<ID3D12CommandAllocator> GetCommandAllocator() { return CommandAllocator[FrameIndex]; }
		inline ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return CommandList; }
		inline ComPtr<ID3D12PipelineState> GetPipelineState() { return PipelineState; }
		inline ComPtr<ID3D12Resource> GetCurrentRenderTarget() { return RenderTargets[FrameIndex]; }
		inline ComPtr<ID3D12DescriptorHeap> GetRTVHeap() { return RTVHeap; }
		inline ComPtr<ID3D12CommandQueue> GetCommandQueue() { return CommandQueue; }
		inline ComPtr<IDXGISwapChain4> GetSwapChain() { return SwapChain; }
		inline ComPtr<ID3D12Device2> GetDevice() { return Device; }

		inline UINT32 GetCurrentFrameIndex() const { return FrameIndex; }
		inline UINT32 GetRTVDescriptorSize() const { return RTVDescriptorSize; }

	private:
		void GetHardwareAdapter(IDXGIFactory2 *Factory, IDXGIAdapter1 **Adapter);
		bool CheckTearingSupport(ComPtr<IDXGIFactory4> Factory4);
		void UpdateRenderTargetViews(ComPtr<ID3D12Device2> InDevice, ComPtr<IDXGISwapChain4> InSwapChain, ComPtr<ID3D12DescriptorHeap> InDescriptorHeap);

		ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter1> InAdapter);
		ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> InDevice, D3D12_COMMAND_LIST_TYPE Type);
		ComPtr<IDXGISwapChain4> CreateSwapChain(HWND HWnd, ComPtr<IDXGIFactory4> InFactory, ComPtr<ID3D12CommandQueue> InCommandQueue, UINT32 Width, UINT32 Height, UINT32 BufferCount);
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> InDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, UINT32 NumDescriptors);
		ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> InDevice, D3D12_COMMAND_LIST_TYPE Type);
		ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device2> InDevice, ComPtr<ID3D12CommandAllocator> CommandAllocator, D3D12_COMMAND_LIST_TYPE Type);
		
		ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> InDevice);
		HANDLE CreateEventHandle();
	private:
		ComPtr<ID3D12Device2> Device;
		ComPtr<IDXGISwapChain4> SwapChain;
		ComPtr<ID3D12Resource> RenderTargets[FrameCount];
		ComPtr<ID3D12CommandAllocator> CommandAllocator[FrameCount];
		ComPtr<ID3D12CommandQueue> CommandQueue;
		ComPtr<ID3D12DescriptorHeap> RTVHeap;
		ComPtr<ID3D12PipelineState> PipelineState;
		ComPtr<ID3D12GraphicsCommandList> CommandList;

		UINT32 RTVDescriptorSize;
		UINT32 FrameIndex;
		HANDLE FenceEvent;
		ComPtr<ID3D12Fence> Fence;
		UINT64 FenceValue;

		bool IsInitialized;
	};
}

