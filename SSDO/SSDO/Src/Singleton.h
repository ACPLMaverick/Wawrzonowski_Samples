#pragma once


template <class T> class Singleton
{
protected:
	static T* _instance;

public:
	static T* GetInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new T();
		}
		return _instance;
	}

	static void DestroyInstance()
	{
		if (_instance != nullptr)
		{
			delete _instance;
			_instance = nullptr;
		}
	}

};

template <class T>
T* Singleton<T>::_instance = nullptr;