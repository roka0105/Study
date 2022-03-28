#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )



/*
1.�۷ι� ������������
longptr���� g_�̸����� ����
*/

LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device



/*
  3.����� ���� �Լ�
*/
/**/
HRESULT InitD3D(HWND hWnd)
{
	/* �Ʒ� �ڵ带 �� �����ϰ� ���� 
	g_pD3D= Direct3DCreate9();
	g_pD3D->CreateDevice();*/
	


	/* 
	�Ʒ��� �����ϰ� �ٲٸ�
	g_pD3d �������ε� direct3d�� new ���ִ°��� ���ڰ��� �ް��ؼ� �������ذ�.
	createdevice�� ���� �߿��ѵ� �� direct3dcreat9�� ��ü �����ؼ� �Լ��� �����ϴ°�?
	�׳� �������� ������ �׷��� §�Ŵ� ����..
	g_pD3D= Direct3D����();
	*/
	/*g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
	//WINCLASS_EX
	D3DPRESENT_PARAMETERS d3dpp; //����ü ������ Ŭ���� �ʱ�ȭ�ϴ°Ŷ� ����ϴ�.
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//CreateWindow,Regist,Show
	//���̷�Ʈx ��ü�� ����°� createdevice �̴�.
	// g_pD3d -> ��ġ�����();
	// &g_pd3dDevice�� output ���ڷ� ����� �Ѿ��.
	
	g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice);
	*/
	

	//NULL==E_FAIL
   if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}
VOID Cleanup()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}
VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;
	/* ����
	* ������ ���� 
	* �Ķ�ȭ������ �׷����� chain swap�Ǹ� �Ķ�ȭ���� �׷�����.
	*/
	//1. Clear the backbuffer to a blue color ��ȭ���� �����.
	//��������ϴ� �������� ������ ������ ĥ�ؼ� ����°�.
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	//2. Begin the scene �׸��� �����Ѵ�. endscene �ϰ� ���� swap chain��Ŵ.
	//������� ���۴� ���� ȭ�� �׸�����.
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Rendering of scene objects can happen here
		//���⿡ �׸� ����� �־����.

		// End the scene
		g_pd3dDevice->EndScene();
	}

	//3. Present the backbuffer contents to the display
	//������ swap chain �ϴ� ���� �϶�� �ϴ� �ڵ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

/*
  2.������ â ����(����ȭ�� �Լ�)
*/
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		Render();
		ValidateRect(hWnd, NULL);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);

	// Register the window class ������ Ŭ���� ����
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"D3D Tutorial", NULL
	};
	//������ Ŭ���� ���
	RegisterClassEx(&wc);

	// Create the application's window
	HWND hWnd = CreateWindow(L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice",
		WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D initd3d �����ϸ� ������ ����� �޼��� ���� ����.
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// Show the window
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		// Enter the message loop
		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass(L"D3D Tutorial", wc.hInstance);
	return 0;
}

/*
typedef INT int ������ �ϴ� ����
int�� ������ 4byte ���µ� 64bit ü��� ���鼭 8byte�� �ٲ�.
int32,int64
int32 �� ���� �ִٰ� int64�� �ٲ�� �� ��� ���� �ٲ� ���� ������ �����Ǹ� �մϴ�.

LRESULT long ������ ���·� ���� ���ϰ��� �����͸� �ְڴٴ� �ǹ�.
winmain���� msgproc�� ���� ȣ���ϱ� ���� �Լ������Ͱ� ���ϵ�.

�ٷ������Լ� ����� �ʱ�ȭ�۾��� �ϰ� ��������� ��밡���ϴ�.
���ڴ� �����ص� �ֵ鸸 ����� �� ����.


�ٷ��� ������۸�ó�� ��ȭ���� �ΰ� ����µ� buffer��� �����.
ù��° �������� �ִ� front buffer (frame buffer)
�ι�° �������� �ִ�  back buffer(back frame buffer)

ù��° �������� ������ �ٲ�ġ���� �� �ΰ��� �����Ͱ� ����Ǿ��ִٰ�
������ ������ ���� �����ϰ� �ϴ½��̴�.page swap
Display�� �����ͷ� ����Ǿ� ��ü�Ǵ°��� swap chain�̶�� �Ѵ�.
���� ��ü�� chain �̶�� ��.


*/
