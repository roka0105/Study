#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN
/*
*여기랑 책 참고하면서 공부하기
https ://ggyu-kim.tistory.com/category/IT%ED%99%9C%EB%8F%99/2D%EA%B2%8C%EC%9E%84%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D?page=4
*/
//#include <windows.h>
//#include <stdlib.h>
//#include <crtdbg.h>
#include <d3d9.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT InitD3D(HWND hWnd);
VOID Render();
VOID CleanUp();
//WinMain 매개변수 설명
/*
hInstance :애플리케이션 포인터,
hPrevInstance :이전버전의 호환성을 위해 존재(하지만 필요없고 항상 NULL로 쓴다),
lpCmdLine :커맨드 라인 매개변수에 대해 NULL로 끝나는 문자열에 대한 포인터(애플리케이션이 실행될 때 전달되는 단어나 기호),
nCmdShow :윈도우를 어떻게 보여줄 것인지 설정
*/


LPCTSTR lpszClass = TEXT("test");
LPDIRECT3D9 g_pD3D = NULL;//D3D 디바이스 생성할 D3D 객체 변수
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;//랜더링에 사용될 D3D디바이스

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//윈도우 설정을 위한 구조체
	WNDCLASSEX wc;
	HWND hWndMain;

	//윈도우 구조체 데이터 설정
	wc.cbSize = sizeof(wc);//구조체 크기
	wc.style = CS_HREDRAW | CS_VREDRAW;//크기가 변하면 다시 그리는 설정
	wc.lpfnWndProc = WinProc; //윈도우 프로시저를 가리킨다.
	wc.cbClsExtra = 0;//여분의 클래스 메모리는 필요 없다.
	wc.cbWndExtra = 0;//여분의 윈도우 메모리는 필요 없다.
	wc.hInstance = hInstance;//인스턴스 핸들
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//마우스 커서를 화살표로 설정
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//배경 브러쉬
	wc.lpszMenuName = NULL;//메뉴 리소스의 이름
	wc.lpszClassName = lpszClass;//윈도우 클래스의 이름
	wc.hIconSm = NULL;//작은 클래스 아이콘
	//Window 클래스 등록
	//RegisterClassEx 함수는 에러가 발생할 경우 0을 반환한다.
	if (RegisterClassEx(&wc) == 0)return false; //에러 발생한 경우

	//CreateWindow 매개변수
	/*
	lpClassName: Window 클래스 이름 null로 끝나는 문자열에 대한 포인터, 이 이름은 wc.lpszClassName과 동일해야함.
	lpWindowName:윈도우 제목 표시줄에 나타낼 텍스트
	dsStyle:생성할 윈도우 스타일 ->WS_OVERLAPPEDWINDOW = 컨트롤이 있고 크기 조절 가능한 윈도우 생성.
							   ->WS_OVERLAPPED: 컨트롤이 없고 크기가 고정된 윈도우 생성.
							   ->WS_EX_TOPMOST|WS_VISIBLE|WS_POPUP : |로 결합된 세가지 스타일, 게임을 전체화면으로 만들때 사용.
	x,y:윈도ㅓ우 왼쪽 위 모서리 좌표
	nWidth: 윈도우 폭(픽셀단위)
	nHeight: 윈도우 높이(픽셀단위)
	hWndParent: 부모 윈도우
	hMenu:윈도우 메뉴
	hInstance: Window 클래스로 부터 식별할 수 있는 애플리케이션 식별자
	lpParam:추가 윈도우 매개변수

	*/
	hWndMain = CreateWindow(lpszClass, L"HELLO WORLD", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		GetDesktopWindow(), (HMENU)NULL, wc.hInstance, NULL);
	//Direct3D 초기화
	//SUCCEEDED 반환값이 음수가 아닌 경우 성공.
	if (SUCCEEDED(InitD3D(hWndMain)))
	{
		ShowWindow(hWndMain, SW_SHOWDEFAULT);
		UpdateWindow(hWndMain);

		//int done = 0;
		MSG msg;
		memset(&msg, 0, sizeof(msg));

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, hWndMain, 0, 0, PM_REMOVE))
			{
				/*TranslateMessage는 메세지를 번역해준다. 만약 키 입력이 들어왔다면 WM_KEYDOWN은 키가 눌린 상태 정보만을 전달한다.
				  키가 눌려서 어떤 행동을 처리할지에 대한 메세지는 WM_CHAR에 담겨있다. WM_CHAR를 메세지큐에 넣고 다음 GetMessage가
				  오면 WM_CHAR을 수행하여 해당 메세지를 프로시저에 전달한다.*/
				TranslateMessage(&msg);
				/*DispatchMessage는 msg가 처리되어야 할 프로시저를 찾아서 호출하고 메세지를 전송한다. */
				DispatchMessage(&msg);
			}
		}
	}
	//등록된 클래스 소거
	UnregisterClass(lpszClass, wc.hInstance);
	return 0;

}
//HRESULT/LRESULT
//https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=winyong39&logNo=50151638443
//HRESULT 상태정보
/*Succeed 0 FAILED 1*/
/*
 S_OK 값은 0
 때때로 Boolean TRUE 값(0x00)으로 S_FALSE와 함께 사용되며 함수가 성공하였음을 의미 한다.

 NOERROR
 S_OK와 동일한 의미 이다.

 S_FALSE
 S_OK와 반대로 Boolean FALSE 값(0x01)으로 함수가 실패 했음을 의미 한다.

 E_OUTOFMEMORY
 필요한 메모리를 할당할 수 없음

 E_NOTIMPL
 멤버 함수에 구현 코드가 포함되어 있지 않다.

 E_UNEXPRCTED
 치명적인 실패를 의미한다.

 E_INVALIDARG
 하나 혹은 그 이상의 인자가 타당하지 않음

 E_NOINTERFACE
 요청한 인터페이스를 지원하지 않음

 E_POINTER
 타당하지 않은 포인터

 E_HANDLE
 타당하지 않은 처리

 E_ABORT
 작동 중지

 E_FAIL
 특정하지 않은 실패

 E_ACCESSDENIED
 일반적인 접근이 금지된 에러*/


 /*-------------------------------------------------------
 *Direct3D 초기화
 ---------------------------------------------------------*/
HRESULT InitD3D(HWND hWnd)
{
	/* Direct3D 객체를 만들고 객체에게 줄 인터페이스를 얻어야 한다.
   IDirect3D9* Direct3DCreate9(UINT SDKVersion); 
   SDKVersion 매개변수는 D3D_SDK_VERTSION이여야 합니다

   왜냐하면 컴파일 하는 동안 사용되는 DirectX의 버전이 대상 시스템에 설치된 런타임 DLL과 일치하는지 확인하는데 사용 
   */
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		MessageBox(NULL, L"디바이스 객체  생성 실패", L"오류", MB_OK);
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp;//디바이스 생성을 위한 구조체
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;//창 모드 생성
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//가장 효율적인 swap 효과
	//frame buffer 설명
    /*
     Frame Buffer

    1. Sucface

    그래픽 카드는 모니터에 색상을 출력하기 위한 프레임 버퍼라는 메모리를 이용합니다. D3D는 프레임 버퍼를 크게 두 개 이용하는데
    한가지는 Front Buffer(전면 버퍼), Back Buffer(후면 버퍼)이다.
    전면 버퍼는 현재 모니터에 출력하고 있는 색상을 저장하는 버퍼이고, 후면 버퍼는 다음에 렌더링할 색 상을 저장하고 있는 기억 공간이다.
    이 버퍼들은 Surface로 구성되어 있습니다. 여기서 Surface란 이미 처리에 의해 생성된 픽셀 정보들을 보관하는 저장소를 뜻합니다.
    픽셀 정보를 가지고 있다면 모두 Surface를 포함 한다고 말할 수 있다. 즉, 프레임 버퍼이외도 카메라의 깊이 값을 가지고 있으면 이 Surface를 깊이 버퍼, stencil효과를 사용하면 Stecil버퍼라고 부릅니다.

     전면 버퍼 : 현재 비디오 장치에 출력되고 있는 픽셀 메모리 영역, 이 버퍼는 색상 버퍼의 한 종류입니다. 과거 프로그래머 접근 가능. 현재 X

     후면 버퍼 : 속도 향상을 위해 전면 버퍼와 동일한 색상 정보를 저장하고 있는 메모리 영역이고 프로그래머는 여기에 접근하여 값을 변경할 수 있습니다. 후면 버퍼는 다시 3종류의 버퍼인 색상, 깊이, 스텐실 버퍼로 구성되어 있습니다.

    색상 버퍼: 픽셍을 저장하고 있는 공간

    깊이 버퍼: 새로운 색상 값을 색상 버퍼에 덮어 쓸 것인가에 대해 판별하는 값을 가지고 있는 버퍼로 파이프 라인의 깊이 테스트에 사용된다.
    또한 가상 카메라의 Z축 값에 의존하기에 깊이 버퍼 대신 Z Buffer라 부르기도 합니다.

    스텐실 버퍼:  깊이 버퍼와 비슷하게 색상 갱신에 대한 판별 값을 저장하고 있는데 이것을 Masking(마스킹)값이라 합니다. 이 마스킹 값을 이용해서 스텐실 테스트를 파이프라인에서 진행합니다. 주로 이용되는 것은 그림자를 표현하거나 속이 비워있는 3D물체의 절단면 등에서 사용되는 스텐실 효과입니다.
    https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=likegs3&logNo=220680554878
    */
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;//현재 바탕화면 모드에 맞춰서 후면 버퍼 생성
    
	//디바이스 다음과 같은 설정으로 생성
	/*1. 디폴트 비디오 카드를 사용 
	  2. HAL 디바이스를 생성(HW 가속장치를 사용하겠다는 뜻)
	  *HAL이란?
	  하드웨어 추상화 레이어(Hardware Abstraction Layer)'의 약자
      컴퓨터에서 프로그램이 수만가지의 하드웨어를 별 차이 없이 다룰 수 있도록 OS에서 만들어주는 가교 역할을 한다.
      API처럼 사용하며 프로그래밍을 할땐 특정 디바이스에 종속되지 않는 프로그래밍을 하도록 해 준다.출처. 비슷한 기능을 하는 디바이스라면 제조사에 따른 세세한 차이까지 프로그램이 직접 신경쓸 필요가 없고, HAL을 통해 필요한 기능을 OS에 요청만 하면 나머지는 OS가 알아서 다 해 준다는 것이다.
	  https://namu.wiki/w/HAL?from=hal
	  3. 정점 처리는 모든 카드에서 지원하는 SW 처리로 생성(HW로 생성할 경우 더욱 높은 성능)*/
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		MessageBox(NULL, L"디바이스 생성 실패", L"오류", MB_OK);
		return E_FAIL;
	}
	return S_OK;
}
/*-------------------------------------------------------
 *화면 그리기
 ---------------------------------------------------------*/
//https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=wlsspakss&logNo=110243417
VOID Render()
{
	if (g_pd3dDevice == NULL)return;
	//후면 버퍼를 파란색으로 지운다.
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
	//렌더링 시작
	//https://pidiszero.tistory.com/114  SUCCEEDED 매크로
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//실제 렌더링 명령들이 나열될 곳

		//렌더링 종료
		g_pd3dDevice->EndScene();
	}
	//후면버퍼를 보이는 화면(전면버퍼)으로 
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
/*-------------------------------------------------------
 *초기화된 객체들을 소거하기.
 ---------------------------------------------------------*/
VOID CleanUp()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();
	if (g_pD3D != NULL)
		g_pD3D->Release();
}
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		if(IDCANCEL== MessageBox(hWnd, L"종료하시겠습니까?(Y/N)", L"종료확인", MB_OKCANCEL))
		{
			return 0;
		}
		CleanUp();
		PostQuitMessage(0);
		return 0;
	case WM_DESTROY:
		CleanUp();
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		Render();
		ValidateRect(hWnd, NULL);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
