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
INT CApplication::Start(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	INT result=Init(hInstance, hPrevInstance, nCmdShow, lpszCmdParam);
	return result;
}
void CApplication::Run()
{
	MsgLoop();
}
CApplication::~CApplication()
{
	this->Cleanup();
	delete instance;
}