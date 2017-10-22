// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Public/InteractionZone.h"
#include "Gun.generated.h"

class ATheWhiteDeathCharacter;

#pragma region Enum Public

UENUM(BlueprintType)
enum class EGunFireMode : uint8
{
	SemiAuto		UMETA(DisplayName = "Semi Auto"),
	Auto			UMETA(DisplayName = "Auto"),
	BoltAction		UMETA(DisplayName = "Bolt-action")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	MainWeapon		UMETA(DisplayName = "Main Weapon"),
	SecondaryWeapon	UMETA(DisplayName = "Secondary Weapon")
};

UENUM(BlueprintType)
enum class EAimMode : uint8
{
	Ironsights		UMETA(DisplayName = "Ironsights"),
	Scope			UMETA(DisplayName = "Scope")
};

#pragma endregion

UCLASS()
class THEWHITEDEATH_API AGun : public AActor
{
	GENERATED_BODY()

protected:

#pragma region Properties Protected

#pragma region Elements Protected

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* AimCameraLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* ScopeAimCameraLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UParticleSystemComponent* FireParticle;

	UPROPERTY(Category = "InteractionZone", EditAnywhere, BlueprintReadWrite)
	class UInteractionZone* InteractionZone;
	
	UPROPERTY(Category = "BoxTrigger", EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;

#pragma endregion

	/** Gun's name */
	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	FString Name;

	/** AmmoType for Ammunition Map in Equipment */
	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	FString MyAmmoType;

	/** WeaponType for Weapons Array in Equipment */
	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	EWeaponType MyWeaponType;

	/** WeaponType for Weapons Array in Equipment */
	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	EAimMode MyAimMode = EAimMode::Ironsights;

	/** WeaponType for Weapons Array in Equipment */
	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	bool ScopeAvailable = false;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(Category = "Gun", EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class ATheWhiteDeathProjectile> ProjectileClass;

	/** Sound to play each time we fire. */
	UPROPERTY(Category = "Gun|Sounds", EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	/** Sound to play each time we pull trigger on empty chamber. */
	UPROPERTY(Category = "Gun|Sounds", EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireEmptyChamberSound;

	/** Sound to play each time we reload. */
	UPROPERTY(Category = "Gun|Sounds", EditAnywhere, BlueprintReadWrite)
	class USoundBase* ReloadSound;

	/** Sound to play each time we repeat (only applies on bolt-action firemode) */
	UPROPERTY(Category = "Gun|Sounds", EditAnywhere, BlueprintReadWrite)
	class USoundBase* RepeatSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(Category = "Gun|Animations", EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* FireAnimation;

	/** AnimMontage to play each time we fire last bullet */
	UPROPERTY(Category = "Gun|Animations", EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* FireAnimationLastBullet;

	/** AnimMontage to play each time we reload */
	UPROPERTY(Category = "Gun|Animations", EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* ReloadAnimation;

	/** AnimMontage to play each time we reload on empty chamber (i.e. lock is open) */
	UPROPERTY(Category = "Gun|Animations", EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* ReloadAnimationEmptyChamber;

	/** AnimMontage to play each time we repeat (only applies on bolt-action firemode) */
	UPROPERTY(Category = "Gun|Animations", EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* RepeatAnimation;

	/** This describes randomization in bullet's direction when firing a bullet "once", i.e. having a time to aim. */
	UPROPERTY(Category = "Gun|Dispersion", EditAnywhere, BlueprintReadWrite)
	FVector BulletDispersionStationary;

	/** This describes randomization in bullet's direction when firing many bullets in short period of time. */
	UPROPERTY(Category = "Gun|Dispersion", EditAnywhere, BlueprintReadWrite)
	FVector BulletDispersionBurst;

	/** Period of time mentioned in a property above. Should be at least higher than 1.0f / (MaxFiresPerMinute / 60.0f) */
	UPROPERTY(Category = "Gun|Dispersion", EditAnywhere, BlueprintReadWrite)
	float BulletDispersionOverTime;

	/** Should be a value between <0, 1>*/
	UPROPERTY(Category = "Gun|Dispersion", EditAnywhere, BlueprintReadWrite)
	float BulletDispersionCharacterVelocityInfluence;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float DamageBase;

	/** This parameters describes how much will the weapon be pulled upward after each shot. This should visually corellate with Fire animation, but has no other relation.*/
	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float Recoil;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	EGunFireMode MyFireMode;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float MaxFiresPerMinute;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float BoltActionRepeatTimeSeconds;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	int MagazineCapacity;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadOnly)
	int BulletsInMagazine;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float ReloadTimeSeconds;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float FireLoudness;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float FireLoudnessMaxRadius;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float CameraFOVIronsights = 45.0f;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float CameraFOVScope = 45.0f;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float AimSensitivityMultiplierIronsights = 1.0f;

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
	float AimSensitivityMultiplierScope = 0.5f;

#pragma endregion

#pragma region Protected

	const float _dispersionVelocityInfluenceMplier = 0.00001f;

	ATheWhiteDeathCharacter* _owner;

	int _bulletsToReload;
	FVector _currentDispersion;
	float _secondsPerFire;
	float _fireTimer = 0.0f;
	float _reloadTimer = 0.0f;
	float _dispersionTimer = 0.0f;
	bool _bTriggerPulled = false;
	bool _bBoltNeedRepeat = false;
	bool _bSemiFired = false;
	bool _bReloadInProgress;


#pragma endregion

#pragma region Functions Protected

	void TickDispersion(float DeltaTime);
	void TickShoot(float DeltaTime);
	void TickReload(float DeltaTime);

	void TrySpawnProjectile();
	void RepeatBoltAction();

#pragma endregion
	
public:	

	UPROPERTY(Category = "Gun", EditAnywhere, BlueprintReadWrite)
		bool IsPickedUp;

#pragma region Functions Public

	// Sets default values for this actor's properties
	AGun();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/// <summary>
	/// If weapon is auto or semi auto, it pulls the trigger. It is not released until next ReleaseFire() call or ammunition ends.
	/// If weapon is bolt action it pulls the trigger. At the next pull, lock is repeated, and in a call after that - another shot is made.
	/// </summary>
	virtual void Fire();

	/// <summary>
	/// Releases weapon owner's finger from the trigger, should be called between subsequent Fire() calls.
	/// </summary>
	virtual void ReleaseFire();

	/// <summary>
	/// Reloads weapon's magazine to maximum amount.
	/// </summary>
	virtual void Reload(int bulletsCount);

	/// <summary>
	/// Obvious.
	/// </summary>
	/// <returns></returns>
	virtual bool CanFire();

	/// <summary>
	/// Pick up the weapon by given character, makes him its owner.
	/// </summary>
	virtual void PickUp(ATheWhiteDeathCharacter* character);

	/// <summary>
	/// Set owner to null.
	/// </summary>
	virtual void Discard();

	virtual void ChangeAimMode(EAimMode mode) { MyAimMode = mode; }
	virtual void ChangeAimMode() { (MyAimMode == EAimMode::Ironsights && ScopeAvailable) ?  MyAimMode = EAimMode::Scope : MyAimMode = EAimMode::Ironsights; }

	UFUNCTION(Category = "Gun", BlueprintCallable)
	void OnInteraction(APlayerCharacter* pc);

#pragma region Getters

	FString GetName() { return Name; }
	FString GetMyAmmoType() { return MyAmmoType; }

	UFUNCTION(Category = "Gun", BlueprintCallable)
	EWeaponType GetMyWeaponType() { return MyWeaponType; }

	UFUNCTION(Category = "Gun", BlueprintCallable)
	EAimMode GetMyAimMode() { return MyAimMode; }

	UFUNCTION(Category = "Gun", BlueprintCallable)
	EGunFireMode GetMyFireMode() { return MyFireMode; }

	USceneComponent* GetAimLocationComponent() { if (MyAimMode == EAimMode::Ironsights) { return AimCameraLocation; } else { return ScopeAimCameraLocation; } }
	bool GetBoltActionWaitsForRepeat() { return _bBoltNeedRepeat; }
	bool GetReloadInProgress() { return _bReloadInProgress; }
	int GetMagazineCapacity() { return MagazineCapacity; }

	UFUNCTION(Category = "Gun", BlueprintCallable)
	int GetBulletCount() { return BulletsInMagazine; }
	
	float GetDamageBase() { return DamageBase; }
	float GetCameraAimedFOV() { if (MyAimMode == EAimMode::Ironsights) { return CameraFOVIronsights; } else { return CameraFOVScope; } }
	float GetAimSensitivityMultiplier() { if (MyAimMode == EAimMode::Ironsights) { return AimSensitivityMultiplierIronsights; } else { return AimSensitivityMultiplierScope; } }
	ATheWhiteDeathCharacter* GetOwner() { return _owner; }
	UInteractionZone* GetInteractionZone() { return InteractionZone; }
	UBoxComponent* GetBoxComponent() { return BoxComponent; }
	USkeletalMeshComponent* GetMesh() { return Mesh; }

#pragma endregion

#pragma region Setters

	void SetMyWeaponType(EWeaponType type) { MyWeaponType = type; }

#pragma endregion

#pragma endregion

};
