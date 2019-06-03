#include "Systems.h"
#include "DeviceResources.h"
#include "DXHelper.h"
#include "BaseWindow.h"

namespace DX
{
	DeviceResources::DeviceResources() :
		IsInitialized(false)
	{

	}

	void DeviceResources::GetHardwareAdapter(IDXGIFactory2 *Factory, IDXGIAdapter1 **Adapter)
	{
		ComPtr<IDXGIAdapter1> InternalAdapter;
		*Adapter = nullptr;

		UINT32 DestAdapterIndex = 0;
		UINT64 VideoMemory = 0;
		for (UINT32 AdapterIndex = 0;
			Factory->EnumAdapters1(AdapterIndex, &InternalAdapter) != DXGI_ERROR_NOT_FOUND;
			++AdapterIndex)
		{
			DXGI_ADAPTER_DESC1 Desc;
			InternalAdapter->GetDesc1(&Desc);
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			if (SUCCEEDED(D3D12CreateDevice(InternalAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				__uuidof(ID3D12Device), nullptr)))
			{
				if (Desc.DedicatedVideoMemory > VideoMemory)
				{
					VideoMemory = Desc.DedicatedVideoMemory;
					DestAdapterIndex = AdapterIndex;
				}
			}
		}

		Factory->EnumAdapters1(DestAdapterIndex, Adapter);
	}

	bool DeviceResources::CheckTearingSupport(ComPtr<IDXGIFactory4> Factory4)
	{
		bool AllowTearing = false;
		ComPtr<IDXGIFactory5> Factory5;
		if (SUCCEEDED(Factory4.As(&Factory5)))
		{
			if (FAILED(Factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &AllowTearing, sizeof(AllowTearing))))
			{
				AllowTearing = false;
			}
		}

		return AllowTearing;
	}

	ComPtr<ID3D12Device2> DeviceResources::CreateDevice(ComPtr<IDXGIAdapter1> InAdapter)
	{
		ComPtr<ID3D12Device2> Result;
		ThrowIfFailed(D3D12CreateDevice(InAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Result)));

		return Result;
	}

	ComPtr<ID3D12CommandQueue> DeviceResources::CreateCommandQueue(ComPtr<ID3D12Device2> InDevice, D3D12_COMMAND_LIST_TYPE Type)
	{
		ComPtr<ID3D12CommandQueue> Result;

		D3D12_COMMAND_QUEUE_DESC Desc = {};
		Desc.Type = Type;
		Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Desc.NodeMask = 0;

		ThrowIfFailed(InDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&Result)));

		return Result;
	}

	ComPtr<IDXGISwapChain4> DeviceResources::CreateSwapChain(HWND HWnd, ComPtr<IDXGIFactory4> InFactory, ComPtr<ID3D12CommandQueue> InCommandQueue, UINT32 Width, UINT32 Height, UINT32 BufferCount)
	{
		ComPtr<IDXGISwapChain4> Result;

		DXGI_SWAP_CHAIN_DESC1 Desc = {};
		Desc.Width = Width;
		Desc.Height = Height;
		Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.Stereo = FALSE;
		Desc.SampleDesc = { 1, 0 };
		Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.BufferCount = BufferCount;
		Desc.Scaling = DXGI_SCALING_STRETCH;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		Desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		Desc.Flags = (CheckTearingSupport(InFactory) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

		ComPtr<IDXGISwapChain1> SwapChain1;
		ThrowIfFailed(InFactory->CreateSwapChainForHwnd(InCommandQueue.Get(), HWnd, &Desc, NULL, NULL, &SwapChain1));

		ThrowIfFailed(InFactory->MakeWindowAssociation(HWnd, DXGI_MWA_NO_ALT_ENTER));

		ThrowIfFailed(SwapChain1.As(&Result));

		return Result;
	}

	ComPtr<ID3D12DescriptorHeap> DeviceResources::CreateDescriptorHeap(ComPtr<ID3D12Device2> InDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, UINT32 NumDescriptors)
	{
		ComPtr<ID3D12DescriptorHeap> Result;

		D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
		Desc.NumDescriptors = NumDescriptors;
		Desc.Type = Type;
		Desc.Flags = Flags;

		ThrowIfFailed(InDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&Result)));
		return Result;
	}

	ComPtr<ID3D12CommandAllocator> DeviceResources::CreateCommandAllocator(ComPtr<ID3D12Device2> InDevice, D3D12_COMMAND_LIST_TYPE Type)
	{
		ComPtr<ID3D12CommandAllocator> Result;
		ThrowIfFailed(InDevice->CreateCommandAllocator(Type, IID_PPV_ARGS(&Result)));

		return Result;
	}

	ComPtr<ID3D12GraphicsCommandList> DeviceResources::CreateCommandList(ComPtr<ID3D12Device2> InDevice, ComPtr<ID3D12CommandAllocator> CommandAllocator, D3D12_COMMAND_LIST_TYPE Type)
	{
		ComPtr<ID3D12GraphicsCommandList> Result;
		ThrowIfFailed(InDevice->CreateCommandList(0, Type, CommandAllocator.Get(), NULL, IID_PPV_ARGS(&Result)));
		ThrowIfFailed(Result->Close());

		return Result;
	}

	ComPtr<ID3D12Fence> DeviceResources::CreateFence(ComPtr<ID3D12Device2> InDevice)
	{
		ComPtr<ID3D12Fence> Result;
		ThrowIfFailed(InDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Result)));

		return Result;
	}

	HANDLE DeviceResources::CreateEventHandle()
	{
		HANDLE Result;
		Result = CreateEvent(NULL, FALSE, FALSE, NULL);
		return Result;
	}

	void DeviceResources::UpdateRenderTargetViews(ComPtr<ID3D12Device2> InDevice, ComPtr<IDXGISwapChain4> InSwapChain, ComPtr<ID3D12DescriptorHeap> InDescriptorHeap)
	{
		auto RTVDescriptorSize = InDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT32 i = 0; i < FrameCount; ++i)
		{
			ThrowIfFailed(InSwapChain->GetBuffer(i, IID_PPV_ARGS(&RenderTargets[i])));
			Device->CreateRenderTargetView(RenderTargets[i].Get(), nullptr, RTVHandle);
			RTVHandle.Offset(1, RTVDescriptorSize);
		}
	}

	void DeviceResources::LoadPipeline(BaseWindow *Window)
	{
		UINT32 DXGIFactoryFlags = 0;
		
		ComPtr<IDXGIFactory4> Factory;
		ThrowIfFailed(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&Factory)));
		if (Window->UseWarpDevice())
		{
			ComPtr<IDXGIAdapter> WarpAdapter;
			ThrowIfFailed(Factory->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapter)));
			ThrowIfFailed(D3D12CreateDevice(WarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&Device)));
		}
		else
		{
			ComPtr<IDXGIAdapter1> HardwareAdapter;
			GetHardwareAdapter(Factory.Get(), &HardwareAdapter);

			Device = CreateDevice(HardwareAdapter);
			CommandQueue = CreateCommandQueue(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			SwapChain = CreateSwapChain(Win32Application::GetWindow(), Factory, CommandQueue, Window->GetWidth(), Window->GetHeight(), FrameCount);
			FrameIndex = SwapChain->GetCurrentBackBufferIndex();
			RTVHeap = CreateDescriptorHeap(Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, FrameCount);
			RTVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			UpdateRenderTargetViews(Device, SwapChain, RTVHeap);
			for (int i = 0; i < FrameCount; ++i)
			{
				CommandAllocator[i] = CreateCommandAllocator(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			}
			CommandList = CreateCommandList(Device, CommandAllocator[FrameIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);
			Fence = CreateFence(Device);
			FenceEvent = CreateEventHandle();

			IsInitialized = true;
		}
	}

	UINT64 DeviceResources::Signal(ComPtr<ID3D12CommandQueue> InCommandQueue, ComPtr<ID3D12Fence> InFence, UINT64& InFenceValue)
	{
		uint64_t FenceValueForSignal = ++InFenceValue;
		ThrowIfFailed(CommandQueue->Signal(InFence.Get(), FenceValueForSignal));

		return FenceValueForSignal;
	}

	void DeviceResources::WaitForFenceValue(ComPtr<ID3D12Fence> InFence, uint64_t InFenceValue, HANDLE InFenceEvent, std::chrono::milliseconds Duration)
	{
		if (InFence->GetCompletedValue() < InFenceValue)
		{
			ThrowIfFailed(InFence->SetEventOnCompletion(InFenceValue, InFenceEvent));
			WaitForSingleObject(InFenceEvent, static_cast<DWORD>(Duration.count()));
		}
	}

	void DeviceResources::Flush()
	{
		uint64_t FenceValueForSignal = Signal(CommandQueue, Fence, FenceValue);
		WaitForFenceValue(Fence, FenceValueForSignal, FenceEvent, std::chrono::milliseconds::max());
		FrameIndex = SwapChain->GetCurrentBackBufferIndex();
	}
}