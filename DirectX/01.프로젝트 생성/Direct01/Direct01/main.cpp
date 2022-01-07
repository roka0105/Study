#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN
/*
*����� å �����ϸ鼭 �����ϱ�
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
//WinMain �Ű����� ����
/*
hInstance :���ø����̼� ������,
hPrevInstance :���������� ȣȯ���� ���� ����(������ �ʿ���� �׻� NULL�� ����),
lpCmdLine :Ŀ�ǵ� ���� �Ű������� ���� NULL�� ������ ���ڿ��� ���� ������(���ø����̼��� ����� �� ���޵Ǵ� �ܾ ��ȣ),
nCmdShow :�����츦 ��� ������ ������ ����
*/


LPCTSTR lpszClass = TEXT("test");
LPDIRECT3D9 g_pD3D = NULL;//D3D ����̽� ������ D3D ��ü ����
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;//�������� ���� D3D����̽�

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//������ ������ ���� ����ü
	WNDCLASSEX wc;
	HWND hWndMain;

	//������ ����ü ������ ����
	wc.cbSize = sizeof(wc);//����ü ũ��
	wc.style = CS_HREDRAW | CS_VREDRAW;//ũ�Ⱑ ���ϸ� �ٽ� �׸��� ����
	wc.lpfnWndProc = WinProc; //������ ���ν����� ����Ų��.
	wc.cbClsExtra = 0;//������ Ŭ���� �޸𸮴� �ʿ� ����.
	wc.cbWndExtra = 0;//������ ������ �޸𸮴� �ʿ� ����.
	wc.hInstance = hInstance;//�ν��Ͻ� �ڵ�
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//���콺 Ŀ���� ȭ��ǥ�� ����
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//��� �귯��
	wc.lpszMenuName = NULL;//�޴� ���ҽ��� �̸�
	wc.lpszClassName = lpszClass;//������ Ŭ������ �̸�
	wc.hIconSm = NULL;//���� Ŭ���� ������
	//Window Ŭ���� ���
	//RegisterClassEx �Լ��� ������ �߻��� ��� 0�� ��ȯ�Ѵ�.
	if (RegisterClassEx(&wc) == 0)return false; //���� �߻��� ���

	//CreateWindow �Ű�����
	/*
	lpClassName: Window Ŭ���� �̸� null�� ������ ���ڿ��� ���� ������, �� �̸��� wc.lpszClassName�� �����ؾ���.
	lpWindowName:������ ���� ǥ���ٿ� ��Ÿ�� �ؽ�Ʈ
	dsStyle:������ ������ ��Ÿ�� ->WS_OVERLAPPEDWINDOW = ��Ʈ���� �ְ� ũ�� ���� ������ ������ ����.
							   ->WS_OVERLAPPED: ��Ʈ���� ���� ũ�Ⱑ ������ ������ ����.
							   ->WS_EX_TOPMOST|WS_VISIBLE|WS_POPUP : |�� ���յ� ������ ��Ÿ��, ������ ��üȭ������ ���鶧 ���.
	x,y:�����ÿ� ���� �� �𼭸� ��ǥ
	nWidth: ������ ��(�ȼ�����)
	nHeight: ������ ����(�ȼ�����)
	hWndParent: �θ� ������
	hMenu:������ �޴�
	hInstance: Window Ŭ������ ���� �ĺ��� �� �ִ� ���ø����̼� �ĺ���
	lpParam:�߰� ������ �Ű�����

	*/
	hWndMain = CreateWindow(lpszClass, L"HELLO WORLD", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		GetDesktopWindow(), (HMENU)NULL, wc.hInstance, NULL);
	//Direct3D �ʱ�ȭ
	//SUCCEEDED ��ȯ���� ������ �ƴ� ��� ����.
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
				/*TranslateMessage�� �޼����� �������ش�. ���� Ű �Է��� ���Դٸ� WM_KEYDOWN�� Ű�� ���� ���� �������� �����Ѵ�.
				  Ű�� ������ � �ൿ�� ó�������� ���� �޼����� WM_CHAR�� ����ִ�. WM_CHAR�� �޼���ť�� �ְ� ���� GetMessage��
				  ���� WM_CHAR�� �����Ͽ� �ش� �޼����� ���ν����� �����Ѵ�.*/
				TranslateMessage(&msg);
				/*DispatchMessage�� msg�� ó���Ǿ�� �� ���ν����� ã�Ƽ� ȣ���ϰ� �޼����� �����Ѵ�. */
				DispatchMessage(&msg);
			}
		}
	}
	//��ϵ� Ŭ���� �Ұ�
	UnregisterClass(lpszClass, wc.hInstance);
	return 0;

}
//HRESULT/LRESULT
//https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=winyong39&logNo=50151638443
//HRESULT ��������
/*Succeed 0 FAILED 1*/
/*
 S_OK ���� 0
 ������ Boolean TRUE ��(0x00)���� S_FALSE�� �Բ� ���Ǹ� �Լ��� �����Ͽ����� �ǹ� �Ѵ�.

 NOERROR
 S_OK�� ������ �ǹ� �̴�.

 S_FALSE
 S_OK�� �ݴ�� Boolean FALSE ��(0x01)���� �Լ��� ���� ������ �ǹ� �Ѵ�.

 E_OUTOFMEMORY
 �ʿ��� �޸𸮸� �Ҵ��� �� ����

 E_NOTIMPL
 ��� �Լ��� ���� �ڵ尡 ���ԵǾ� ���� �ʴ�.

 E_UNEXPRCTED
 ġ������ ���и� �ǹ��Ѵ�.

 E_INVALIDARG
 �ϳ� Ȥ�� �� �̻��� ���ڰ� Ÿ������ ����

 E_NOINTERFACE
 ��û�� �������̽��� �������� ����

 E_POINTER
 Ÿ������ ���� ������

 E_HANDLE
 Ÿ������ ���� ó��

 E_ABORT
 �۵� ����

 E_FAIL
 Ư������ ���� ����

 E_ACCESSDENIED
 �Ϲ����� ������ ������ ����*/


 /*-------------------------------------------------------
 *Direct3D �ʱ�ȭ
 ---------------------------------------------------------*/
HRESULT InitD3D(HWND hWnd)
{
	/* Direct3D ��ü�� ����� ��ü���� �� �������̽��� ���� �Ѵ�.
   IDirect3D9* Direct3DCreate9(UINT SDKVersion); 
   SDKVersion �Ű������� D3D_SDK_VERTSION�̿��� �մϴ�

   �ֳ��ϸ� ������ �ϴ� ���� ���Ǵ� DirectX�� ������ ��� �ý��ۿ� ��ġ�� ��Ÿ�� DLL�� ��ġ�ϴ��� Ȯ���ϴµ� ��� 
   */
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		MessageBox(NULL, L"����̽� ��ü  ���� ����", L"����", MB_OK);
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp;//����̽� ������ ���� ����ü
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;//â ��� ����
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//���� ȿ������ swap ȿ��
	//frame buffer ����
    /*
     Frame Buffer

    1. Sucface

    �׷��� ī��� ����Ϳ� ������ ����ϱ� ���� ������ ���۶�� �޸𸮸� �̿��մϴ�. D3D�� ������ ���۸� ũ�� �� �� �̿��ϴµ�
    �Ѱ����� Front Buffer(���� ����), Back Buffer(�ĸ� ����)�̴�.
    ���� ���۴� ���� ����Ϳ� ����ϰ� �ִ� ������ �����ϴ� �����̰�, �ĸ� ���۴� ������ �������� �� ���� �����ϰ� �ִ� ��� �����̴�.
    �� ���۵��� Surface�� �����Ǿ� �ֽ��ϴ�. ���⼭ Surface�� �̹� ó���� ���� ������ �ȼ� �������� �����ϴ� ����Ҹ� ���մϴ�.
    �ȼ� ������ ������ �ִٸ� ��� Surface�� ���� �Ѵٰ� ���� �� �ִ�. ��, ������ �����̿ܵ� ī�޶��� ���� ���� ������ ������ �� Surface�� ���� ����, stencilȿ���� ����ϸ� Stecil���۶�� �θ��ϴ�.

     ���� ���� : ���� ���� ��ġ�� ��µǰ� �ִ� �ȼ� �޸� ����, �� ���۴� ���� ������ �� �����Դϴ�. ���� ���α׷��� ���� ����. ���� X

     �ĸ� ���� : �ӵ� ����� ���� ���� ���ۿ� ������ ���� ������ �����ϰ� �ִ� �޸� �����̰� ���α׷��Ӵ� ���⿡ �����Ͽ� ���� ������ �� �ֽ��ϴ�. �ĸ� ���۴� �ٽ� 3������ ������ ����, ����, ���ٽ� ���۷� �����Ǿ� �ֽ��ϴ�.

    ���� ����: �ȼ��� �����ϰ� �ִ� ����

    ���� ����: ���ο� ���� ���� ���� ���ۿ� ���� �� ���ΰ��� ���� �Ǻ��ϴ� ���� ������ �ִ� ���۷� ������ ������ ���� �׽�Ʈ�� ���ȴ�.
    ���� ���� ī�޶��� Z�� ���� �����ϱ⿡ ���� ���� ��� Z Buffer�� �θ��⵵ �մϴ�.

    ���ٽ� ����:  ���� ���ۿ� ����ϰ� ���� ���ſ� ���� �Ǻ� ���� �����ϰ� �ִµ� �̰��� Masking(����ŷ)���̶� �մϴ�. �� ����ŷ ���� �̿��ؼ� ���ٽ� �׽�Ʈ�� ���������ο��� �����մϴ�. �ַ� �̿�Ǵ� ���� �׸��ڸ� ǥ���ϰų� ���� ����ִ� 3D��ü�� ���ܸ� ��� ���Ǵ� ���ٽ� ȿ���Դϴ�.
    https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=likegs3&logNo=220680554878
    */
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;//���� ����ȭ�� ��忡 ���缭 �ĸ� ���� ����
    
	//����̽� ������ ���� �������� ����
	/*1. ����Ʈ ���� ī�带 ��� 
	  2. HAL ����̽��� ����(HW ������ġ�� ����ϰڴٴ� ��)
	  *HAL�̶�?
	  �ϵ���� �߻�ȭ ���̾�(Hardware Abstraction Layer)'�� ����
      ��ǻ�Ϳ��� ���α׷��� ���������� �ϵ��� �� ���� ���� �ٷ� �� �ֵ��� OS���� ������ִ� ���� ������ �Ѵ�.
      APIó�� ����ϸ� ���α׷����� �Ҷ� Ư�� ����̽��� ���ӵ��� �ʴ� ���α׷����� �ϵ��� �� �ش�.��ó. ����� ����� �ϴ� ����̽���� �����翡 ���� ������ ���̱��� ���α׷��� ���� �Ű澵 �ʿ䰡 ����, HAL�� ���� �ʿ��� ����� OS�� ��û�� �ϸ� �������� OS�� �˾Ƽ� �� �� �شٴ� ���̴�.
	  https://namu.wiki/w/HAL?from=hal
	  3. ���� ó���� ��� ī�忡�� �����ϴ� SW ó���� ����(HW�� ������ ��� ���� ���� ����)*/
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		MessageBox(NULL, L"����̽� ���� ����", L"����", MB_OK);
		return E_FAIL;
	}
	return S_OK;
}
/*-------------------------------------------------------
 *ȭ�� �׸���
 ---------------------------------------------------------*/
//https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=wlsspakss&logNo=110243417
VOID Render()
{
	if (g_pd3dDevice == NULL)return;
	//�ĸ� ���۸� �Ķ������� �����.
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
	//������ ����
	//https://pidiszero.tistory.com/114  SUCCEEDED ��ũ��
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//���� ������ ��ɵ��� ������ ��

		//������ ����
		g_pd3dDevice->EndScene();
	}
	//�ĸ���۸� ���̴� ȭ��(�������)���� 
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
/*-------------------------------------------------------
 *�ʱ�ȭ�� ��ü���� �Ұ��ϱ�.
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
		if(IDCANCEL== MessageBox(hWnd, L"�����Ͻðڽ��ϱ�?(Y/N)", L"����Ȯ��", MB_OKCANCEL))
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
