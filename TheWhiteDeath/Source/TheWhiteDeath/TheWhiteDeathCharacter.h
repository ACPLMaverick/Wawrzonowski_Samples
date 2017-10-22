// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "CoverCharacter.h"
#include "Public/Grenade.h"
#include "Public/Equipment.h"
#include "TheWhiteDeathCharacter.generated.h"

class UHitBox;
class UInputComponent;
class ACover;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;

USTRUCT()
struct FCharacterHitBoxes
{
	GENERATED_BODY()

	union
	{
		UChildActorComponent* tab[14];
		struct
		{
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* Torso;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* Head;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* LArm;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* LForearm;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* LHand;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* RArm;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* RForearm;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* RHand;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* LTigh;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* LCalf;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* LFoot;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* RTigh;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* RCalf;
			UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
			class UChildActorComponent* RFoot;
		};
	};
};

UCLASS(config=Game)
class ATheWhiteDeathCharacter : public ACoverCharacter
{
	GENERATED_BODY()

protected:

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UChildActorComponent* Gun;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HitBoxes", meta = (AllowPrivateAccess = "true"))
	//FCharacterHitBoxes HitBoxes;

	/** Equipment component: Keeps weapons and ammunition */
	UPROPERTY(VisibleDefaultsOnly, Category = "Equipment", BlueprintReadOnly)
	class UEquipment* Equipment;

	/** Grenade class to spawn */
	UPROPERTY(Category = "Grenade", EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AGrenade> GrenadeClass;

	/** These have to reside here, as they are being used both by player (for registration) and enemies. */
	UPROPERTY(Category = "Perception", BlueprintReadOnly)
	class UAISenseConfig_Sight* _configSight;

	UPROPERTY(Category = "Perception", BlueprintReadOnly)
	class UAISenseConfig_Hearing* _configHearing;

	UPROPERTY(Category = "Perception", BlueprintReadOnly)
	class UAISenseConfig_Damage* _configDamage;

public:

#pragma region Properties

	/** Maximum health that player has. */
	UPROPERTY(Category = "Character", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	/** Maximum stamina that player has. */
	UPROPERTY(Category = "Character", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 5.0f;

	/** Current health that player has. */
	UPROPERTY(Category = "Character", BlueprintReadOnly)
	float Health = FLT_MAX;

	/** Current stamina that player has. */
	UPROPERTY(Category = "Character", BlueprintReadOnly)
	float Stamina = FLT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
	bool UseTickLocationWantsToHit = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		float StepLoudness = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		float StepLoudnessMaxRadius = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		float SightRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		float LoseSightRadius = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		float SightHalfAngleDegrees = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		float HearingRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		float StimuliMaxAge = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception", meta = (AllowPrivateAccess = "true"))
		bool CanDetectDeadBodies = true;

	/** Pointer to Grenade for invoking its OnRelease method */
	UPROPERTY(Category = "Grenade", EditDefaultsOnly, BlueprintReadWrite)
	class AGrenade* Grenade;

#pragma endregion

protected:

#pragma region Protected

	TArray<ECollisionChannel> _collisionChannelsToCheck;

	FVector _locationWantsToHit;
	FVector _deathPosition;

	const float _staminaDownPerSecond = 1.0f;
	const float _staminaUpPerSecondWalk = 1.0f;
	const float _staminaUpPerSecondStand = 2.5f;
	const float _recoilMultiplierBase = 0.005f;
	const float _dispersionMultiplierBase = 1.0f;
	const float _recoilMultiplierCover = 0.0f;
	const float _dispersionMultiplierCover = 0.1f;
	const float _recoilSpeed = 30.0f;

	float _recoilMultiplier = _recoilMultiplierBase;
	float _dispersionMultiplier = _dispersionMultiplierBase;

	float _recoilTarget;
	float _recoilCurrent = 0.0f;
	float _healthRegainTime = 0.0f;

	bool _bAlive = true;

	bool _bSprintInProgress;

	bool _bRecoilInProgress;

	bool _bHasTakenDamageThisFrame;

#pragma endregion

public:
	ATheWhiteDeathCharacter(const class FObjectInitializer& PCIP);

	virtual void BeginPlay();

	virtual void Tick(float deltaTime) override;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* DieAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAutoAnimation;

	/** AnimMontage to play each time we release fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* ReleaseFireAnimation;

	/** AnimMontage to play each time we release fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* ReleaseFireAutoAnimation;

	/** AnimMontage to play each time we repeat bolt action rifle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* BoltActionRepeatAnimation;

	/** AnimMontage to play each time we reload bolt action rifle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* BoltActionReloadAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoundDeath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoundFootstepsConcrete;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoundFootstepsWood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoundFootstepsSnow;

	virtual class USkeletalMeshComponent* GetCharacterMesh() override { return Mesh1P; }

	/**
	* Called when Character crouches. Called on non-owned Characters through bIsCrouched replication.
	* @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	* @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	*/
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/**
	* Called when Character stops crouching. Called on non-owned Characters through bIsCrouched replication.
	* @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	* @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	*/
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/** Fires a projectile. */
	virtual void OnFire();

	/** Releases finger from the trigger. */
	virtual void OnReleaseFire();

	/** Reloads weapon. */
	virtual void OnReload();

	/** Start throwing grenade. */
	virtual void OnBeginThrowGrenade();

	/** Release grenade */
	virtual void OnReleaseGrenade();

	/** Handles moving forward/backward */
	virtual void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	virtual void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	virtual void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	virtual void LookUpAtRate(float Rate);

	virtual bool CanCrouch();
	virtual bool CanSprint();

	void StartCrouch();
	void EndCrouch();

	void StartSprint();
	void EndSprint();

	/** 
	 * Invokes graphical effects when player gets hit
	 * @param DmgAmount		Amount of damage 
	 * @param Direction		Where we get damage from	
	 */
	virtual void OnHit(float DmgAmount, FVector Direction);

	// ///////////////////////////////////////////////////////////////////

	protected: 

#pragma region Functions Protected



	void TickDeath(float dt);
	void TickRecoil(float dt);
	void TickCrouch(float dt);
	void TickSprint(float dt);
	void TickLocationWantsToHit(float dt);

	virtual void Die();

	virtual void GetLocationWantsToHitData(FVector& outOrigin, FVector& outDirection);

#pragma endregion

	// ///////////////////////////////////////////////////////////////////

	


public:

#pragma region Functions Public

#pragma region Accessors

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	bool GetHasTakenDamageThisFrame() const { return _bHasTakenDamageThisFrame; }

	FORCEINLINE class UEquipment* GetEquipment() const { return Equipment; }

#pragma endregion

	void ApplyRecoil(float recoil);

	UFUNCTION(BlueprintCallable, Category = "Accessors")
	virtual FVector GetLocationWantsToHit();

	UFUNCTION(BlueprintCallable, Category = "Accessors")
	void SetLocationWantsToHit(FVector target) { _locationWantsToHit = target; }
	float GetDispersionMultiplier() { return _dispersionMultiplier; }

#pragma endregion
};

