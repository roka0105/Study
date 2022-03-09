#pragma once
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
	void DrawWindow();
	void MsgRoop();
	static LRESULT CALLBACK MsgProcessing(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);
};

