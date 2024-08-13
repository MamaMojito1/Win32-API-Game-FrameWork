#include "renderer.h"

namespace Application
{
	void Renderer::SetPixel(int x, int y, const RGBColor& color)
	{
		BitmapBuffer& buffer = getInstance().buffer;

		// clipping
		if (x < 0 || x > buffer.width || y < 0 || y > buffer.height)
			return;

		// convert (u8, u8, u8) rgb color to u32 representation
		uint32_t raw_color = (color.red << 16) | (color.green << 8) | (color.blue << 0);

		uint8_t* row = (uint8_t*)buffer.memory + x * bytes_per_pixel + y * buffer.pitch;
		uint32_t* pixel = (uint32_t*)row;
		*pixel = raw_color;
	}

	void Renderer::FillRectangle(const Rect& rect, const RGBColor& color)
	{
		BitmapBuffer& buffer = getInstance().buffer;

		int minX = rect.x;
		int minY = rect.y;
		int maxX = rect.x + rect.width;
		int maxY = rect.y + rect.height;

		// clipping
		if (minX < 0)				minX = 0;
		if (minY < 0)				minY = 0;
		if (maxX > buffer.width)	maxX = buffer.width;
		if (maxY > buffer.height)	maxY = buffer.height;

		// convert (u8, u8, u8) rgb color to u32 representation

		uint32_t raw_color = (color.red << 16) | (color.green << 8) | (color.blue << 0);

		uint8_t* row = (uint8_t*)buffer.memory + minX * bytes_per_pixel + minY * buffer.pitch;
		for (int y = minY; y < maxY; y++)
		{
			uint32_t* pixel = (uint32_t*)row;
			for (int x = minX; x < maxX; x++)
			{
				*pixel++ = raw_color;
			}
			row += buffer.pitch;
		}
	}

	void Renderer::DrawRectangle(const Rect& rect, const RGBColor& color)
	{
		BitmapBuffer& buffer = getInstance().buffer;

		int minX = rect.x;
		int minY = rect.y;
		int maxX = rect.x + rect.width;
		int maxY = rect.y + rect.height;

		// clipping
		if (minX < 0)			minX = 0;
		if (minY < 0)			minY = 0;
		if (maxX > buffer.width) maxX = buffer.width;
		if (maxY > buffer.height) maxY = buffer.height;

		for (int x = minX; x <= maxX; x++)
		{
			SetPixel(x, minY, color);
			SetPixel(x, maxY, color);
		}

		for (int y = minY; y <= maxY; y++)
		{
			SetPixel(minX, y, color);
			SetPixel(maxX, y, color);
		}
	}

	void Renderer::PlotLineLow(int x0, int y0, int x1, int y1, const RGBColor& color)
	{
		int dx = x1 - x0;
		int dy = y1 - y0;
		int yi = 1;
		if (dy < 0)
		{
			yi = -1;
			dy = -dy;
		}
		int D = (2 * dy) - dx;
		int y = y0;

		for (int x = x0; x <= x1; x++)
		{
			SetPixel(x, y, color);
			if (D > 0)
			{
				y = y + yi;
				D = D + (2 * (dy - dx));
			}
			else
			{
				D = D + 2 * dy;
			}
		}
	}

	void Renderer::PlotLineHigh(int x0, int y0, int x1, int y1, const RGBColor& color)
	{
		int dx = x1 - x0;
		int dy = y1 - y0;
		int xi = 1;
		if (dx < 0)
		{
			xi = -1;
			dx = -dx;
		}
		int D = (2 * dx) - dy;
		int x = x0;

		for (int y = y0; y <= y1; y++)
		{
			SetPixel(x, y, color);
			if (D > 0)
			{
				x = x + xi;
				D = D + (2 * (dx - dy));
			}
			else
			{
				D = D + 2 * dx;
			}
		}
	}

	int abs(int x)
	{
		if (x < 0) return -x;
		else return x;
	}

	void Renderer::DrawLine(int x0, int y0, int x1, int y1, const RGBColor& color)
	{
		if (abs(y1 - y0) < abs(x1 - x0))
		{
			if (x0 > x1)
				PlotLineLow(x1, y1, x0, y0, color);
			else
				PlotLineLow(x0, y0, x1, y1, color);
		}
		else
		{
			if (y0 > y1)
				PlotLineHigh(x1, y1, x0, y0, color);
			else
				PlotLineHigh(x0, y0, x1, y1, color);
		}
	}

	void Renderer::DrawCircle(int radius, int originX, int originY, const RGBColor& color)
	{
		for (int y = -radius; y <= radius; y++)
			for (int x = -radius; x <= radius; x++)
				if (x * x + y * y > radius * radius - radius && x * x + y * y < radius * radius + radius)
					SetPixel(originX + x, originY + y, color);
	}

	void Renderer::FillCircle(int radius, int originX, int originY, const RGBColor& color)
	{
		for (int y = -radius; y <= radius; y++)
			for (int x = -radius; x <= radius; x++)
				if (x * x + y * y < radius * radius + radius)
					SetPixel(originX + x, originY + y, color);
	}


	void Renderer::getWindowDimensions(int* outWidth, int* outHeight)
	{
		RECT ClientRect;
		GetClientRect(getInstance().windowHandle, &ClientRect);

		*outWidth = ClientRect.right - ClientRect.left;
		*outHeight = ClientRect.bottom - ClientRect.top;
	}

	void Renderer::ResizeFrameBuffer(int width, int height)
	{
		BitmapBuffer& buffer = getInstance().buffer;

		if (buffer.memory)
		{
			VirtualFree(buffer.memory, 0, MEM_RELEASE);
		}

		buffer.width = width;
		buffer.height = height;

		buffer.info.bmiHeader.biSize = sizeof(buffer.info.bmiHeader);
		buffer.info.bmiHeader.biWidth = buffer.width;
		buffer.info.bmiHeader.biHeight = -(buffer.height);
		buffer.info.bmiHeader.biPlanes = 1;
		buffer.info.bmiHeader.biBitCount = 32;
		buffer.info.bmiHeader.biCompression = BI_RGB;

		int BufferMemorySize = buffer.width * buffer.height * bytes_per_pixel;

		buffer.memory = VirtualAlloc(0, BufferMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		buffer.pitch = buffer.width * bytes_per_pixel;
	}

	void Renderer::CopyBufferToWindow(HDC DeviceContext, int WindowWidth, int WindowHeight)
	{
		BitmapBuffer& buffer = getInstance().buffer;

		if (StretchDIBits(
			DeviceContext,
			0, 0, WindowWidth, WindowHeight,
			0, 0, buffer.width, buffer.height,
			buffer.memory,
			&(buffer.info),							// if this function fails it will return 0
			DIB_RGB_COLORS,
			SRCCOPY
		) == 0)
		{
			OutputDebugString("StretchDIBits function failed!\n");
		}
	}
	void Renderer::clear()
	{
		BitmapBuffer& buffer = getInstance().buffer;

		FillRectangle({ 0, 0, buffer.width, buffer.height }, getInstance().clearColor);
	}
}