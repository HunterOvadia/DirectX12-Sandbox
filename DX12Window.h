#pragma once

#include "BaseWindow.h"
#include "DeviceResources.h"

class DX12Window : public BaseWindow
{
public:
	DX12Window(std::wstring Name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

private:
	DX::DeviceResources DeviceResources;
};