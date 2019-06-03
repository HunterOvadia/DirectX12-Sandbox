#include "Systems.h"
#include "DX12Window.h"

int WINAPI
WinMain(HINSTANCE Instance, HINSTANCE, LPSTR, int CmdShow)
{
	DX12Window Application(L"DX12 Sandbox");
	return Win32Application::Run(&Application, Instance, CmdShow);
}