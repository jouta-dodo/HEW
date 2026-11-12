#include<windows.h>
#include<iostream>
#include"Input.h"
#include"graphics.h"
#include"Game.h"
#include"Scene.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	const wchar_t CLASS_NAME[] = L"PuzzleGameClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName =CLASS_NAME;
	RegisterClass(&wc);

	int width = 1280;
	int height =720;

	RECT rc = { 0,0,width,height };
	AdjustWindowRect(& rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"ÉpÉYÉãÉQÅ[ÉÄ", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right-rc.left,rc.bottom-rc.top,nullptr,nullptr,hInstance,nullptr);

	if (!Graphics::Init(hwnd, width, height))
	{
		MessageBox(nullptr, L"GraphicsÇÃèâä˙âªé∏îs",L"å¥àˆî≠å©", MB_OK);
		return -1;
	}

	//èàóùåƒÇ‘Ç∆Ç±
	Game::Init();

	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Input::Update();

			if (Input::IsKeyPressed(Key::Escape))break;

			Game::Update();

			Graphics::BeginDraw(Color::SkyBlue);

			Game::Draw();
			//ï`âÊèIóπ
			Graphics::EndDraw();
		}
	}

	Graphics::Uninit();
	return 0;
}