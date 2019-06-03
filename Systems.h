#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <shellapi.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "D3DX12.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>
#include <chrono>

#include <wrl.h>

#define LOG(str, ... ) char buffer[500]; sprintf_s(buffer, 500, str, __VA_ARGS__); OutputDebugString(buffer); 
