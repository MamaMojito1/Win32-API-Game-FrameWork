#include "Application.h"
App
{
	float x = 250, y = 250;
	Application::Game::setGameUpdate([&](float delta)
	{
		char char_buffer[256];
		//snprintf(char_buffer, 256, "delta: %f\n", delta);
		//OutputDebugString(char_buffer);

		static int fps;
		static float timePassed = 0.0;

		fps++;
		timePassed += delta;

		if (timePassed >= 1.0f)
		{
			//snprintf(char_buffer, 256, "FPS: %d\n", fps);
			//OutputDebugString(char_buffer);

			timePassed -= 1.0f;
			fps = 0;
		}

		Application::Renderer::SetPixel(10, 10, { 0, 0, 255 });

		Application::Renderer::DrawRectangle({ 800, 450, 120, 100 }, { 0, 0, 255 });

		Application::Renderer::DrawLine(40, 100, 240, 300, { 0, 0, 255 });

		Application::Renderer::FillRectangle({ int(x + 0.5f), int(y + 0.5f), 320, 180 }, { 0, 255, 0 });

		Application::Renderer::FillCircle(100, 100, 100, { 70, 45, 34 });

		Application::Renderer::DrawCircle(50, 500, 100, { 45, 21, 53 });

		if (Application::Input::IsKeyPressed(DC_W))
			y -= 200 * delta;
		else if (Application::Input::IsKeyPressed(DC_S))
			y += 200 * delta;

		if (Application::Input::IsKeyPressed(DC_A))
			x -= 200 * delta;
		else if (Application::Input::IsKeyPressed(DC_D))
			x += 200 * delta;

		Application::Input::Position MousePosition = Application::Input::GetMousePosition();
		snprintf(char_buffer, 256, "%d, %d\n", MousePosition.x, MousePosition.y);
		OutputDebugString(char_buffer);
	}
	);

	Application::Renderer::SetClearColor({ 200, 120, 45 });

	Application::Game::start();

	return 0;
}