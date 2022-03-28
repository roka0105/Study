#pragma once
#include "CWin.h"
class CApplication :public CWin
{
public:
	static int CAppCount;
	static CApplication* instance;
public:
	static int ShowAppCount();
	static void Create();
	static CApplication* Instance();
	static void Destroy();
	INT Start(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow);
	void Run();
private:
	CApplication();
	~CApplication();
};