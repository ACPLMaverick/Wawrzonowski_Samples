#pragma once

#include "UObject/ObjectBase.h"
#include "TheWhiteDeathGameMode.h"
#include "Singleton.generated.h"

template <class T> 
UCLASS(config = Game)
class USingleton : public UObject
{
	GENERATED_BODY()

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
T* USingleton<T>::_instance = nullptr;