#pragma once
//1.�����η� ã��
// \\�� ���� ������ �̽������� �������� �����ϱ� ���Ѱ��Դϴ�.

#pragma comment(lib,"C:\\Users\\user\\Desktop\\Study\\school study\\3_1\\DirectX\\day_01\\day_01~03\\lib\\Debug\\libProject.lib")
#include "C:\Users\user\Desktop\Study\school study\3_1\DirectX\day_01\day_01~03\lib\Debug\libProject.h"

//2.����η� ã�� 
//lib ���ϰ� h ������ ������ ������Ʈ�� �ҽ������� �ִ°��� ���� ������ ��� ��η� �̿� ����
//#pragma comment(lib,"libProject.lib")
//#include "libProject.h"
#include <stdio.h>
#include <Windows.h>
class CWin
{
private:
	HWND hwnd;
	MSG Message;
	WNDCLASS WndClass;
	HINSTANCE g_hInst;
	HINSTANCE g_prevhInst;
	LPSTR lpszCmdParam;
	int nCmdShow;
protected:
	void Init(HINSTANCE Inst, HINSTANCE hPrevInst, int cmdShow, LPSTR CmdParam);
	void RegistWinClass();
	int DrawWindow();
	void MsgRoop();
	static LRESULT CALLBACK MsgProcessing(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
};

