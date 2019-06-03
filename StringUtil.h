#pragma once

#include "Systems.h"

static std::string UTF16ToUTF8(const std::wstring& Input)
{
	const int Size = WideCharToMultiByte(CP_UTF8, 0, Input.c_str(), -1, NULL, 0, 0, NULL);

	std::vector<char> Buffer(Size);
	WideCharToMultiByte(CP_UTF8, 0, Input.c_str(), -1, &Buffer[0], Size, 0, NULL);

	return std::string(&Buffer[0]);
}