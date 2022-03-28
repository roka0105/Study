#pragma once
#ifdef DLLPROJECT_EXPORTS // 파일명_EXPORTS
#define DLLPROJECT_API __declspec(dllexport)//파일명_API __declspec(dllexport)
#else
#define DLLPROJECT_API __declspec(dllimport)//파일명_API __declspec(dllimport)
#endif
extern "C" DLLPROJECT_API int Multi(int data1,int data2);
