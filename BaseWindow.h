#pragma once
#include "Win32Application.h"

class BaseWindow
{
public:
	BaseWindow(std::wstring Name);
	virtual ~BaseWindow();

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

	virtual void OnKeyDown(UINT8 Key) {}
	virtual void OnKeyUp(UINT8 Key) {}

	UINT32 GetWidth() const { return Width; }
	UINT32 GetHeight() const { return Height; }
	const WCHAR *GetTitle() const { return Title.c_str(); }
	bool UseWarpDevice() const { return m_UseWarpDevice; }


protected:
	UINT32 Width;
	UINT32 Height;
	float AspectRatio;
	bool m_UseWarpDevice;

private:
	std::wstring Title;
};