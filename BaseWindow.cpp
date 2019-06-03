#include "Systems.h"
#include "Win32Application.h"
#include "BaseWindow.h"

BaseWindow::BaseWindow(std::wstring Name) : 
	Title(Name),
	m_UseWarpDevice(false)
{
	Width = 1280;
	Height = 720;
}

BaseWindow::~BaseWindow()
{

}
