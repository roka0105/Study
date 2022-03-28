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
	//1.������ ĥ�ϱ�
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(80, 188, 223), 1.0f, 0);
	//2.start,end�ϱ�
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//�׸� ���

		g_pd3dDevice->EndScene();
	}
	//3.present �ϱ�.
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
void CWin::MsgLoop()
{
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
#pragma region ������ ��
			/*
			����:
			peekmessage()�Լ����� NULL ��ġ�� �ִ� ���ڰ��� ������ �ڵ��� ���°��ε�,
			hWnd : �޽����� ���� �������̸� �� ������� �������� �޽����� �����Ѵ�.
			�� ������� �ݵ�� ���� �����忡 �Ҽӵ� �����쿩�� �ϸ� �ٸ� ������� �������� �޽����� ������ �� ����.
			�� �μ��� NULL�̸� �� �Լ��� ȣ���� ������� ���޵� ��� �޽����� �����Ѵ�.
			��� �Ѵ�.

			����:
			�ٵ� ������غ����� hwnd�� ������ �ִ� ������ �ڵ�� �� �Լ��� ȣ���� ���� ������� �ֽ������ ����.
			���� �����忡 �Ҽӵ� �������ε� ������ destroy�� wm_quit �޼����� �ٸ� �޼���ť�� �ִ°�ó�� �ൿ�Ͽ�
			peekmassage�ϸ� �޼���ť�� �޼����� ���� wm_quit ���� ���� ������ó�� �����Ѵ�.
			hwnd���ڰ��� NULL�� �����ÿ��� �����۵��Ѵ�.

			���:
			���ؾȵǴ� �κ���
			<hwnd�� �������� ���� �����忡 �Ҽӵ� �������ӿ��� �޼����� �� ���������� �����Ҽ� ������>�̴�.
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