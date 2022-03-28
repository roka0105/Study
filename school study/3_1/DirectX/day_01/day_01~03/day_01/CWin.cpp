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