#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )



/*
1.글로벌 전역변수영역
longptr형태 g_이름으로 선언
*/

LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device



/*
  3.사용자 정의 함수
*/
/**/
HRESULT InitD3D(HWND hWnd)
{
	/* 아래 코드를 더 간략하게 보면 
	g_pD3D= Direct3DCreate9();
	g_pD3D->CreateDevice();*/
	


	/* 
	아래를 간단하게 바꾸면
	g_pD3d 포인터인데 direct3d가 new 해주는거임 인자값을 받게해서 생성해준것.
	createdevice가 제일 중요한데 왜 direct3dcreat9로 객체 생성해서 함수에 접근하는가?
	그냥 만든사람이 구조를 그렇게 짠거다 ㅋㅋ..
	g_pD3D= Direct3D생성();
	*/
	/*g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
	//WINCLASS_EX
	D3DPRESENT_PARAMETERS d3dpp; //구조체 윈도우 클래스 초기화하는거랑 비슷하다.
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//CreateWindow,Regist,Show
	//다이렉트x 객체를 만드는건 createdevice 이다.
	// g_pD3d -> 장치만들기();
	// &g_pd3dDevice는 output 인자로 여기로 넘어옴.
	
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
	/* 순서
	* 윈도우 생성 
	* 파란화면으로 그려지고 chain swap되며 파란화면이 그려진다.
	*/
	//1. Clear the backbuffer to a blue color 도화지를 지운다.
	//지우고자하는 바탕색과 동일한 색으로 칠해서 지우는것.
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	//2. Begin the scene 그리기 시작한다. endscene 하고 나서 swap chain시킴.
	//대기중인 버퍼는 다음 화면 그리는중.
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Rendering of scene objects can happen here
		//여기에 그릴 명령이 있어야함.

		// End the scene
		g_pd3dDevice->EndScene();
	}

	//3. Present the backbuffer contents to the display
	//이쪽이 swap chain 하는 순간 하라고 하는 코드
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

/*
  2.윈도우 창 생성(정형화된 함수)
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

	// Register the window class 윈도우 클래스 정의
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"D3D Tutorial", NULL
	};
	//윈도우 클래스 등록
	RegisterClassEx(&wc);

	// Create the application's window
	HWND hWnd = CreateWindow(L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice",
		WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D initd3d 성공하면 윈도우 만들고 메세지 루프 돌기.
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
typedef INT int 재정의 하는 이유
int가 예전에 4byte 였는데 64bit 체재로 가면서 8byte로 바뀜.
int32,int64
int32 를 쓰고 있다가 int64로 바뀌게 될 경우 전부 바꿀 수는 없으니 재정의를 합니다.

LRESULT long 포인터 형태로 쓰임 리턴값이 포인터를 주겠다는 의미.
winmain에서 msgproc을 서로 호출하기 위해 함수포인터가 리턴됨.

다렉지원함수 사용은 초기화작업을 하고 연결해줘야 사용가능하다.
쓰겠다 설정해둔 애들만 사용할 수 있음.


다렉도 더블버퍼링처럼 도화지를 두개 만드는데 buffer라고 얘기함.
첫번째 보여지는 애는 front buffer (frame buffer)
두번째 보여지는 애는  back buffer(back frame buffer)

첫번째 렌더링이 끝나면 바꿔치기할 때 두개의 포인터가 연결되어있다가
렌더링 끝나면 끊고 연결하고 하는식이다.page swap
Display에 포인터로 연결되어 교체되는것을 swap chain이라고 한다.
연결 자체는 chain 이라고 함.


*/
