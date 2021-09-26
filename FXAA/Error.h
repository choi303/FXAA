#pragma once
#include <string>
#include <comdef.h>
#include <exception>

class Error
{
public:
	static void Log(std::string msg) noexcept
	{
		const auto errorMsg = "Error: " + msg;
		MessageBoxA(nullptr, errorMsg.c_str(), "FXAA Log", MB_ICONERROR);
	}

	static void Log(const HRESULT hr, const std::string msg) noexcept
	{
		const _com_error error(hr);
		const auto message = "Error: " + msg + "\n" + error.ErrorMessage();
		MessageBoxA(nullptr, message.c_str(), "FXAA Log", MB_ICONERROR);
	}
};