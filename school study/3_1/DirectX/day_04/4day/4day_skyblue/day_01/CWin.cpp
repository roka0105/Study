#include "CWin.h"
BOOL CWin::endflag = false;
INT CWin::Init(HINSTANCE Inst, HINSTANCE hPrevInst, int cmdShow, LPSTR CmdParam)
{
	UNREFERENCED_PARAMETER(Inst);
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),CS_CLASSDC,MsgProcessing,0L,0L,GetModuleHandle(NULL),NULL,NULL,NULL,NULL,
		L"D3D Tutorial",NULL
	};

	RegisterClassEx(&wc);

	hwnd = CreateWindow(L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	if (SUCCEEDED(InitD3D(hwnd)))
	{
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);

		MsgLoop();
	}
	UnregisterClass(L"D3D Tutorial", wc.hInstance);
	return 0; 
}
HRESULT CWin::InitD3D(HWND hWnd)
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}
	return S_OK;
}
VOID CWin::Cleanup()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();
	if (g_pD3D != NULL)
		g_pD3D->Release();
}
VOID CWin::Render()
{
	if (NULL == g_pd3dDevice)
		return;
	//1.바탕색 칠하기
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(80, 188, 223), 1.0f, 0);
	//2.start,end하기
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//그릴 명령

		g_pd3dDevice->EndScene();
	}
	//3.present 하기.
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
void CWin::MsgLoop()
{
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
			if (endflag)
				return;
		}
	}
}

LRESULT CALLBACK  CWin::MsgProcessing(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_CREATE:
		break;
	case WM_DESTROY:
		endflag = true;
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}