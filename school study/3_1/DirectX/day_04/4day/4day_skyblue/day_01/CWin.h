#pragma once
#include <Windows.h>
#include <d3d9.h>
#pragma warning(disable:4996)
#include <strsafe.h>
#pragma warning(default:4996)

class CWin
{
private:
	LPDIRECT3D9 g_pD3D = NULL;
	LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
	HWND hwnd;
	MSG msg;
	static BOOL endflag;
protected:
	HRESULT InitD3D(HWND hWnd);
	VOID Cleanup();
	VOID Render();
	void MsgLoop();
	INT Init(HINSTANCE Inst, HINSTANCE hPrevInst, int cmdShow, LPSTR CmdParam);
	static LRESULT CALLBACK MsgProcessing(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
};

