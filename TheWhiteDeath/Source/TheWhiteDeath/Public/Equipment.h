// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Public/Gun.h"
#include "Equipment.generated.h"

#pragma region Struct

USTRUCT()
struct FAmmoType
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<FString> WeaponsNames;

	UPROPERTY()
	int CurrentAmount;

	UPROPERTY()
	int MaxAmount;

public:
	FAmmoType()
	{
		this->WeaponsNames.Init("", 1);
		this->CurrentAmount = -1;
		this->MaxAmount = -1;
	}

	FAmmoType(TArray<FString>& strArr, int curAmount, int mAmount)
	{
		this->WeaponsNames = strArr;
		this->CurrentAmount = curAmount;
		this->MaxAmount = mAmount;
	}

	int GetMaxAmount() { return MaxAmount; }
	int GetCurrentAmount() { return CurrentAmount; }
	void SetCurrentAmount(int32 value) { CurrentAmount = value; }

};

#pragma endregion

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEWHITEDEATH_API UEquipment : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(Category = "Equipment", EditDefaultsOnly)
	TMap<FString, FAmmoType> AmmunitionTypesMap;

#pragma region Protected Functions

	void FillMap();

#pragma endregion

public:	

#pragma region Properties

	UPROPERTY(Category = "Equipment", EditAnywhere, BlueprintReadWrite)
	TArray<AGun*> MyWeapons;

	UPROPERTY(Category = "Equipment", EditAnywhere, BlueprintReadWrite)
	int32 MyCurrentWeapon;

	UPROPERTY(Category = "Equipment", EditAnywhere, BlueprintReadWrite)
	int GrenadesCount;	//MAX GRENADES COUNT = 3

#pragma endregion

	// Sets default values for this component's properties
	UEquipment();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

#pragma region Public Functions

	bool PickUpGun(AGun* pickedGun);
	bool SwitchGun();
	int SendAmmo();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool AddAmmo(FString ammoName, int count);

#pragma region Getters

	int GetCurrentWeaponAmmoCount();

#pragma endregion

#pragma endregion

};
