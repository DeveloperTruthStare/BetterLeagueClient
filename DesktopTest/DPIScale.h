#pragma once
#include <Windows.h>
#include <d2d1.h>

class DPIScale
{
	static float scale;
public:
	static void Initialize(HWND hwnd)
	{
		float dpi = GetDpiForWindow(hwnd);
		scale = dpi / 96.0f;
	}

	template <typename T>
	static D2D1_POINT_2F PixelToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scale, static_cast<float>(y) / scale);
	}
};

float DPIScale::scale = 1.0f;