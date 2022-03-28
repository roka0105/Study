#include "CWin.h"

void CWin::Init(HINSTANCE Inst, HINSTANCE hPrevInst, int cmdShow, LPSTR CmdParam)
{
	g_hInst = Inst;
	g_prevhInst = hPrevInst;
	nCmdShow = cmdShow;
	lpszCmdParam = CmdParam;
	memset(&Message, 0, sizeof(MSG));
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.lpfnWndProc = CWin::MsgProcessing;
	WndClass.lpszClassName = TEXT("Yeram");
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
}
void CWin::RegistWinClass()
{
	RegisterClass(&WndClass);
}
int CWin::DrawWindow()
{
	hwnd = CreateWindow(TEXT("Yeram"), TEXT("Yeram"), WS_OVERLAPPEDWINDOW, 0, 0, 500, 500,
		NULL, (HMENU)NULL, g_hInst, NULL);
	if (hwnd == NULL)
		return NULL;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return 1;
}
void CWin::MsgRoop()
{
	while (WM_QUIT != Message.message)
	{
		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
#pragma region 질문할 것
			/*
			설명:
			peekmessage()함수에서 NULL 위치에 있는 인자값은 윈도우 핸들이 들어가는곳인데,
			hWnd : 메시지를 받을 윈도우이며 이 윈도우로 보내지는 메시지를 조사한다.
			이 윈도우는 반드시 같은 스레드에 소속된 윈도우여야 하며 다른 스레드로 보내지는 메시지는 조사할 수 없다.
			이 인수가 NULL이면 이 함수를 호출한 스레드로 전달된 모든 메시지를 조사한다.
			라고 한다.

			질문:
			근데 디버깅해봤을때 hwnd가 가지고 있는 윈도우 핸들과 이 함수를 호출한 곳의 스레드는 주스레드로 같다.
			같은 스레드에 소속된 윈도우인데 윈도우 destroy시 wm_quit 메세지를 다른 메세지큐에 넣는것처럼 행동하여
			peekmassage하면 메세지큐에 메세지가 없고 wm_quit 또한 들어가지 않은것처럼 동작한다.
			hwnd인자값을 NULL로 했을시에는 정상작동한다.

			결론:
			이해안되는 부분은
			<hwnd를 했을때는 같은 스레드에 소속된 윈도우임에도 메세지를 왜 정상적으로 조사할수 없는지>이다.
			*/
#pragma endregion
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			
			//MessageBox(hwnd, TEXT("g"), TEXT("h"), MB_OK);
		}
	}
}

LRESULT CALLBACK  CWin::MsgProcessing(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		}
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}