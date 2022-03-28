#include "CApplication.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	CApplication::Create();
	CApplication::ShowAppCount();
	CApplication::Create();
	CApplication::ShowAppCount();
	CApplication* app = CApplication::Instance();

	app->Start(hInstance, hPrevInstance, lpszCmdParam, nCmdShow);
	app->Run();
	

	return 0;
}