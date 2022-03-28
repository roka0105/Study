#pragma once
//1.절대경로로 찾기
// \\를 넣은 이유는 이스케이프 시퀀스를 방지하기 위한것입니다.

#pragma comment(lib,"C:\\Users\\user\\Desktop\\Study\\school study\\3_1\\DirectX\\day_01\\day_01~03\\lib\\Debug\\libProject.lib")
#include "C:\Users\user\Desktop\Study\school study\3_1\DirectX\day_01\day_01~03\lib\Debug\libProject.h"

//2.상대경로로 찾기 
//lib 파일과 h 파일을 실행할 프로젝트의 소스파일이 있는곳에 갖다 놓으면 상대 경로로 이용 가능
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

