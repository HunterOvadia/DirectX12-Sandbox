#pragma once

inline void
ThrowIfFailed(HRESULT Result)
{
	if (FAILED(Result))
	{
		throw std::exception();
	}
}