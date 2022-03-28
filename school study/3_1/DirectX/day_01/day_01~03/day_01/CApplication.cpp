#include "CApplication.h"
int CApplication::CAppCount = 0;
CApplication* CApplication::instance = nullptr;
int CApplication::ShowAppCount()
{
	return CAppCount;
}
void CApplication::Create()
{
	if (instance == nullptr)
	{
		instance = new CApplication();
		CAppCount++;
	}
}
CApplication* CApplication::Instance()
{
	return instance;
}
void CApplication::Destroy()
{
	instance->~CApplication();
}
CApplication::CApplication()
{
}
void CApplication::Start(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	Init(hInstance, hPrevInstance, nCmdShow, lpszCmdParam);
	RegistWinClass();
}
void CApplication::Run()
{
	int check = DrawWindow();
	if (check == NULL)
		return;
	MsgRoop();
}
CApplication::~CApplication()
{
	delete instance;
}