#include "PhoenixSingleton.h"
template <typename T>
T* PhoenixSingleton<T>::Instance()
{   
	if (is_Destroyed)
	{
		new(instance)PhoenixSingleton<T>;
		atexit(Destroy);
		is_Destroyed = false;
	}
	else if (instance == nullptr)
	{
		Create();
	}
	return instance;
}
template <typename T>
PhoenixSingleton<T>::PhoenixSingleton()
{

}
template <typename T>
PhoenixSingleton<T>::~PhoenixSingleton()
{
	is_Destroyed = true;
}
template <typename T>
void PhoenixSingleton<T>::Create()
{
	T ins;
	instance = &ins;
}
template <typename T>
void PhoenixSingleton<T>::Destroy()
{
	instance->PhoenixSingleton<T>::~PhoenixSingleton();
}