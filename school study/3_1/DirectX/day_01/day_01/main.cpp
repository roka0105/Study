#include "CApplication.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	CApplication* app = new CApplication(hInstance,hPrevInstance,lpszCmdParam,nCmdShow);

	app->Run();

	delete app;
	return 0;
}