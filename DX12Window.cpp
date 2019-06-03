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
	Time0 = Time0;

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

}

void DX12Window::OnDestroy()
{

}