#pragma once
#include "CWin.h"
class CApplication :public CWin
{
public:
	CApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow);
	void Run();
};