#pragma once
#include <stdlib.h>
template <typename T>
class PhoenixSingleton
{
protected:
	static T* instance;
	static bool is_Destroyed;
public:
	static  T* Instance();
protected:
	PhoenixSingleton();
	virtual ~PhoenixSingleton();
	static void Create();
	static void Destroy();
};
template <typename t> t* PhoenixSingleton<t>::instance = nullptr;
template <typename t> bool PhoenixSingleton<t>::is_Destroyed = false;
