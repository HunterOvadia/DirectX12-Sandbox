#include "Systems.h"
#include "DX12Window.h"
#include "DXHelper.h"

DX12Window::DX12Window(std::wstring Name)
	: BaseWindow(Name)
{

}

void DX12Window::OnInit()
{
	AspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
	DeviceResources.LoadPipeline(this);
}

void DX12Window::OnUpdate()
{
	static uint64_t FrameCounter = 0;
	static double ElapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock Clock;
	static auto Time0 = Clock.now();

	FrameCounter++;
	auto Time1 = Clock.now();
	auto DeltaTime = Time1 - Time0;
	Time0 = Time1;

	ElapsedSeconds += DeltaTime.count() * 1e-9;
	if (ElapsedSeconds > 1.0)
	{
		auto FPS = FrameCounter / ElapsedSeconds;
		LOG("FPS: %f\n", FPS);

		FrameCounter = 0;
		ElapsedSeconds = 0.0;
	}
}

void DX12Window::OnRender()
{
	ThrowIfFailed(DeviceResources.GetCommandAllocator()->Reset());
	ThrowIfFailed(DeviceResources.GetCommandList()->Reset(DeviceResources.GetCommandAllocator().Get(), DeviceResources.GetPipelineState().Get()));
	DeviceResources.GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			DeviceResources.GetCurrentRenderTarget().Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(DeviceResources.GetRTVHeap()->GetCPUDescriptorHandleForHeapStart(),
											    DeviceResources.GetCurrentFrameIndex(), 
												DeviceResources.GetRTVDescriptorSize());

		const float ClearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
		DeviceResources.GetCommandList()->ClearRenderTargetView(RTVHandle, ClearColor, 0, nullptr);
	}

	DeviceResources.GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			DeviceResources.GetCurrentRenderTarget().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_PRESENT)
	);
	ThrowIfFailed(DeviceResources.GetCommandList()->Close());

	ID3D12CommandList *CommandLists[] = { DeviceResources.GetCommandList().Get() };
	DeviceResources.GetCommandQueue()->ExecuteCommandLists(_countof(CommandLists), CommandLists);

	ThrowIfFailed(DeviceResources.GetSwapChain()->Present(1, 0));
	DeviceResources.Flush();
}

void DX12Window::OnDestroy()
{
	DeviceResources.Flush();
	CloseHandle(DeviceResources.GetFenceEvent());
}