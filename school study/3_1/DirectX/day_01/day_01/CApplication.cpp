#include "CApplication.h"

CApplication::CApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	this->Init(hInstance, hPrevInstance, nCmdShow, lpszCmdParam);
	this->RegistWinClass();
}
void CApplication::Run()
{
	this->DrawWindow();
	this->MsgRoop();
}