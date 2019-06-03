#pragma once

class BaseWindow;

class Win32Application
{
public:
	static int Run(BaseWindow *Window, HINSTANCE Instance, int CmdShow);
	static HWND GetWindow() { return WindowHandle; }

protected:
	static LRESULT CALLBACK WindowProc(HWND Window, UINT32 Message, WPARAM WParam, LPARAM LParam);

private:
	static HWND WindowHandle;
};