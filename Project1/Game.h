#pragma once
#include <Windows.h>
#include <string>
#include <functional>
namespace Application
{
	class Game
	{
		friend LRESULT CALLBACK WindowCallback(
			HWND hWnd,
			UINT message,
			WPARAM wp,
			LPARAM lp
		);
	private:
		HINSTANCE hInst;
		HWND windowHandle = 0;
		bool running = false;

		std::string WindowTitle;
		int WindowWidth, WindowHeight;

		std::function<void(float delta)> update;
	public:
		Game();

		Game(const Game&) = delete;
		Game& operator= (const Game&) = delete;

		~Game() {}

		inline static Game& getInstance()
		{
			static Game game;
			return game;
		}

		inline static void start()
		{
			getInstance().startWindow();
		}

		inline static void setWindowProperties(const std::string title, const int& width, const int& height)
		{
			getInstance().WindowTitle = title;
			getInstance().WindowHeight = height;
			getInstance().WindowWidth = width;
		}

		inline static void setGameUpdate(const std::function<void(float delta)>& update)
		{
			getInstance().update = update;
		}

		inline static std::string getWindowTitle()
		{
			return getInstance().WindowTitle;
		}

		inline static int getWindowWidth()
		{
			return getInstance().WindowWidth;
		}

		inline static int getWindowHeight()
		{
			return getInstance().WindowHeight;
		}

	private:
		void startWindow();

	};
}