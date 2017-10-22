// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "Equipment.h"

void UEquipment::FillMap()
{
	TArray<FString> arr;

	arr.Add("Mosin");
	AmmunitionTypesMap.Add("7.62x54", FAmmoType(arr, 20, 50));

	arr.Empty();
	arr.Add("Tokariev");
	AmmunitionTypesMap.Add("7.62x25", FAmmoType(arr, 0, 100));
}

// Sets default values for this component's properties
UEquipment::UEquipment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	FillMap();

	MyCurrentWeapon = 0;
	MyWeapons.Add(NULL);
	MyWeapons.Add(NULL);
}

// Called when the game starts
void UEquipment::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UEquipment::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

bool UEquipment::PickUpGun(AGun* pickedGun)
{
	if (pickedGun->GetMyWeaponType() == EWeaponType::MainWeapon)
	{
		MyWeapons[0] = pickedGun;
		return true;
	}
	else
	{
		if (MyWeapons[1] == NULL)
		{
			MyWeapons[1] = pickedGun;
			return true;
		}

		return false;
	}
}

bool UEquipment::SwitchGun()
{
	int previousWeapon = MyCurrentWeapon;

	MyCurrentWeapon++;
	MyCurrentWeapon %= MyWeapons.Num();

	if (MyWeapons[MyCurrentWeapon] == NULL)
	{
		MyCurrentWeapon = previousWeapon;
		return false;
	}

	MyWeapons[previousWeapon]->GetMesh()->SetHiddenInGame(true);
	MyWeapons[MyCurrentWeapon]->GetMesh()->SetHiddenInGame(false);
	
	return true;
}

int UEquipment::SendAmmo()
{
	int curAmount = AmmunitionTypesMap[MyWeapons[MyCurrentWeapon]->GetMyAmmoType()].GetCurrentAmount();

	if (curAmount > 0)
	{
		int bulletsToSend = MyWeapons[MyCurrentWeapon]->GetMagazineCapacity() - MyWeapons[MyCurrentWeapon]->GetBulletCount();

		if (bulletsToSend > curAmount)
		{
			AmmunitionTypesMap[MyWeapons[MyCurrentWeapon]->GetMyAmmoType()].SetCurrentAmount(0);
			return curAmount;
		}
		else
		{
			AmmunitionTypesMap[MyWeapons[MyCurrentWeapon]->GetMyAmmoType()].SetCurrentAmount(curAmount - bulletsToSend);
			return MyWeapons[MyCurrentWeapon]->GetBulletCount() + bulletsToSend;
		}
	}
	else
	{
		return 0;
	}
}

bool UEquipment::AddAmmo(FString ammoName, int count)
{
	if (AmmunitionTypesMap[ammoName].GetCurrentAmount() < AmmunitionTypesMap[ammoName].GetMaxAmount())
	{
		int finalCount = AmmunitionTypesMap[ammoName].GetCurrentAmount() + count;
		AmmunitionTypesMap[ammoName].SetCurrentAmount(FMath::Clamp(finalCount, 0, AmmunitionTypesMap[ammoName].GetMaxAmount()));
		return true;
	}
	else
	{
		return false;
	}
}

int UEquipment::GetCurrentWeaponAmmoCount()
{
	return AmmunitionTypesMap[MyWeapons[MyCurrentWeapon]->GetMyAmmoType()].GetCurrentAmount();
}

