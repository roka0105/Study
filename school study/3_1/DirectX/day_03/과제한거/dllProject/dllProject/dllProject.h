#pragma once
#ifdef DLLPROJECT_EXPORTS // ���ϸ�_EXPORTS
#define DLLPROJECT_API __declspec(dllexport)//���ϸ�_API __declspec(dllexport)
#else
#define DLLPROJECT_API __declspec(dllimport)//���ϸ�_API __declspec(dllimport)
#endif
extern "C" DLLPROJECT_API int Multi(int data1,int data2);
