#include "Game.h"

#include "renderer.h"

#include "Input.h"

namespace Application
{
	Game::Game()
	{
		// init window properties to default values
		WindowTitle = "Application";
		WindowWidth = 1280;
		WindowHeight = 720;
	}

	LRESULT CALLBACK WindowCallback(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		LRESULT result = 0;

		switch (message)
		{
		case WM_CLOSE:
		{
			Game::getInstance().running = false;
			OutputDebugString("Window close\n");
		} break;

		case WM_DESTROY:
		{
			Game::getInstance().running = false;
			OutputDebugString("Window destroy\n");
		} break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint32_t VKCode = wParam;

			bool wasDown = (lParam & (1 << 30)) != 0;
			bool isDown = (lParam & (1 << 31)) == 0;

			Input::processKeyboardInput(VKCode, wasDown, isDown);
		} break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		{
			Input::ProcessMouseInput(wParam, lParam);
		} break;

		case WM_MOUSEMOVE:
		{
			Input::UpdateMousePosition(lParam);
		} break;

		case WM_PAINT:
		{
			OutputDebugString("Window Paint\n");

			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(hWnd, &paint);

			int width, height;
			Renderer::getWindowDimensions(&width, &height);

			Renderer::CopyBufferToWindow(device_context, width, height);

			EndPaint(hWnd, &paint);
			
		} break;

		default:
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}

		return result;

	}

	void Game::startWindow()
	{

		Renderer::ResizeFrameBuffer(WindowWidth, WindowHeight);

		const char* className = "window";

		WNDCLASS windowClass = { 0 };

		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowCallback;
		windowClass.hInstance = hInst;
		windowClass.lpszClassName = className;

		if (!RegisterClass(&windowClass))
		{
			OutputDebugString("Failed to register window class\n");
			return;
		}

		DWORD WindowStyle = WS_MAXIMIZEBOX | WS_SYSMENU | WS_CAPTION;

		RECT WindowRect = {};
		WindowRect.left = 0;
		WindowRect.top = 0;
		WindowRect.right = WindowWidth;
		WindowRect.bottom = WindowHeight;

		AdjustWindowRect(&WindowRect, WindowStyle, false);

		windowHandle = CreateWindowEx(0, className, WindowTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, 0, 0, hInst, 0);

		if (windowHandle)
		{
			OutputDebugString("Game INIT\n");
			running = true;

			Renderer::SetWindowHandle(windowHandle);

			// init the clock
			LARGE_INTEGER cpu_frequency;
			QueryPerformanceFrequency(&cpu_frequency);

			LARGE_INTEGER last_counter;
			QueryPerformanceCounter(&last_counter);

			while (running)
			{
				LARGE_INTEGER current_counter;
				QueryPerformanceCounter(&current_counter);

				int64_t counter_elapsed = current_counter.QuadPart - last_counter.QuadPart;

				float delta = (float)counter_elapsed / (float)cpu_frequency.QuadPart;
				
				last_counter = current_counter;

				// Process windows messages
				MSG message;
				while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT) running = false;

					TranslateMessage(&message);
					DispatchMessage(&message);		// send msg to WindowCallback func
				}
				// Update and Render

				Renderer::clear();
				getInstance().update(delta);
				HDC deviceContext = GetDC(windowHandle);

				int width, height;
				Renderer::getWindowDimensions(&width, &height);

				Renderer::CopyBufferToWindow(deviceContext, width, height);

				ReleaseDC(windowHandle, deviceContext);
			}
		}
		else
		{
			OutputDebugString("Failed to create a window\n");
		}
	}


}