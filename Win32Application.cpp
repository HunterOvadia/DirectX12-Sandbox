#include "Systems.h"
#include "Win32Application.h"
#include "BaseWindow.h"
#include "StringUtil.h"

HWND Win32Application::WindowHandle = nullptr;

int Win32Application::Run(BaseWindow *Window, HINSTANCE Instance, int CmdShow)
{
	WNDCLASSEX WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = Instance;
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.lpszClassName = "DX12SandboxClass";
	RegisterClassEx(&WindowClass);

	RECT WindowRect = { 0, 0, static_cast<LONG>(Window->GetWidth()), static_cast<LONG>(Window->GetHeight()) };
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

	WindowHandle = CreateWindow(WindowClass.lpszClassName,
		UTF16ToUTF8(Window->GetTitle()).c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL, NULL, Instance, Window);
	
	Window->OnInit();
	ShowWindow(WindowHandle, CmdShow);
	MSG Message = {};
	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	Window->OnDestroy();
	return static_cast<int>(Message.wParam);
}

LRESULT CALLBACK Win32Application::WindowProc(HWND HWnd, UINT32 Message, WPARAM WParam, LPARAM LParam)
{
	BaseWindow *Window = reinterpret_cast<BaseWindow *>(GetWindowLongPtr(HWnd, GWLP_USERDATA));
	switch (Message)
	{
		case WM_CREATE:
		{
			LPCREATESTRUCT CreateStruct = reinterpret_cast<LPCREATESTRUCT>(LParam);
			SetWindowLongPtr(HWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(CreateStruct->lpCreateParams));
		}
		break;

		case WM_KEYDOWN:
		{
			if (Window)
			{
				Window->OnKeyDown(static_cast<UINT8>(WParam));
			}
		}
		break;

		case WM_KEYUP:
		{
			if (Window)
			{
				Window->OnKeyUp(static_cast<UINT8>(WParam));
			}
		}
		break;

		case WM_PAINT:
		{
			if (Window)
			{
				Window->OnUpdate();
				Window->OnRender();
			}
		}
		break;

		case WM_SIZE:
		{
			RECT ClientRect = {};
			GetClientRect(WindowHandle, &ClientRect);

			int Width = ClientRect.right - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;

			// Do resizing
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
	}

	return DefWindowProc(HWnd, Message, WParam, LParam);
}