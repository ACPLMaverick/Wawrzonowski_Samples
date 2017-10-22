// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TheWhiteDeathCharacter.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Public/HitMarkerWidget.h"
#include "Public/SkillTree.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AISightTargetInterface.h"
#include "PlayerCharacter.generated.h"

class UMapComponent;


UENUM(BlueprintType)
enum class EInputDeviceType : uint8
{
	KeyboardMouse	UMETA(DisplayName = "Keyboard & Mouse"),
	Gamepad			UMETA(DisplayName = "Gamepad")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCharacterInputTypeChanged, EInputDeviceType, NewInputType);

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API APlayerCharacter : public ATheWhiteDeathCharacter, public IGenericTeamAgentInterface, public IAISightTargetInterface
{
	GENERATED_BODY()
	
protected:

	TArray<ECollisionChannel> _canBeSeenChannelsToCheck;
	TArray<class UInteractionZone*> _interZonesAction;
	TArray<class UInteractionZone*> _interZonesSilentKill;

	FTransform _gunRelativeTransform;

	FRotator _mesh1PBaseRotation;
	FVector2D _swayVector;
	FVector2D _swayVectorLast;

	EInputDeviceType _lastInputType;
	FVector _lastCursorPosition;

	class USilentKillInteractionZone* _closestInteractionZone = NULL;

	const float _healthUpPerSecond = 15.0f;
	const float _swayMultiplierX = 2.85f;
	const float _swayMultiplierY = 1.25f;
	const float _swaySmooth = 10.0f;
	const float _deathDelay = 5.0f;

	float _deathTimer = 0.0f;
	float _haxxStickEventTimer = 0.0f;
	float _cameraDefaultFOV;
	float _cameraTargetFOV;
	float _viewConesVisibleTimer;
	float _aimAlpha = 0.0f;
	float _aimTargetAlpha = 0.0f;
	int32 _gunHideDetectorOverlapCount = 0;
	bool _bHealtRegainInProgress;
	bool _bAimingInProgress;
	bool _bIsAiming;
	bool _bSilentKillLerpInProgress;
	bool _bIsHoldingBreath;
	bool _bMarkInWorldEnabled;
	bool _bControlSchemeShown;

	//ACTIVE SKILLS
	bool _bCanSeeViewCones;
	bool _bAreViewConesVisible;
	bool _bCanFocusMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PostProcess, meta = (AllowPrivateAccess = "true"))
	class UEnemyHighlightComponent* HighlightComponent;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void Die() override;
	void DebugDie();

	virtual void GetLocationWantsToHitData(FVector& outOrigin, FVector& outDirection);

	void TickHealthRegain(float dt);
	void TickAim(float dt);
	void TickWeaponSway(float dt);
	void TickViewConesVisibilitySkill(float dt);
	void TickCheckForObjectives(float dt);
	void TickMarker(float dt);
	void TickSilentKill(float dt);
	void TickCheckGrenade(float dt);

	bool CanMove();
	virtual bool CanCrouch() override;
	virtual bool CanSprint() override;

	// The Function Pointer Variable Type; As an argument, we will be sending next level of skill
	typedef void (APlayerCharacter::*FunctionPtrType)(int);
	
	// Array of Function Pointers
	FunctionPtrType SkillFunctions[12];

	// Play a Skill Function (IMPORTANT THEY'RE NOT ACTIVE SKILLS - THEY WILL HAVE ANOTHER FUNCTIONS)
	void PlaySkillFunction(int index, int level);

	// Initialize SkillFunctions Array
	void InitSkillFunctions();

	// Skills functions
	void EmptyFunction(int id);
	void BuyViewConeVisibility(int id);	
	void BuyFocusMode(int id);

	// gun hide detection
	UFUNCTION()
		virtual void OnGunHideBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult);
	UFUNCTION()
		virtual void OnGunHideEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

public:

	APlayerCharacter(const class FObjectInitializer& PCIP);

	UPROPERTY(Category = "User Interface", EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> ControlSchemeClass;

	UUserWidget* ControlScheme;

	/** 
	¯yczê owocnego finalizowania projektów i do zobaczenia w œrodê,

	Jaros³aw Andrzejczak
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* GrenadeThrowAnimation;

	// The class that will be used for the player's hitmarker
	UPROPERTY(Category = "User Interface", EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UHitMarkerWidget> HitMarkerClass;

	// The instance of the player's hitmarker
	UPROPERTY(Category = "User Interface", EditDefaultsOnly, BlueprintReadOnly)
	class UHitMarkerWidget* HitMarker;

	/** Overlap component - detects whether to hide weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkillTree")
	class UBoxComponent* GunHideDetector;

	/** Equipment component: Keeps weapons and ammunition */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkillTree")
	class USkillTree* SkillTree;

	/** And this is the faithful map that our fearless hero always carries by himself. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Map")
	class UMapComponent* Map;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Map")
	class UDecalComponent* TargetMarker;

	UPROPERTY(BlueprintAssignable, Category = "InteractionZone")
	FPlayerCharacterInputTypeChanged DelegateInputTypeChanged;

	/** AnimMontage to play each time we reload bolt action rifle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* SilentKillAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoundHoldBreath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoundReleaseBreath;

	/** Minimum and maximum camera pitch angle when standing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector2D CameraPitchRangeStand = FVector2D(-90.0f, 90.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector2D CameraPitchRangeCrouch = FVector2D(-45.0f, 90.0f);

	virtual void BeginPlay();

	virtual void Tick(float deltaTime) override;

	virtual void OnHit(float DmgAmount, FVector Direction) override;

	virtual void OnEnterCoverArea(ACover* cover) override;

	virtual void OnExitCoverArea() override;

	void OnJump();

	void OnStopJumping();

	void OpenHelp();

	virtual void OnFire() override;
	virtual void OnReleaseFire() override;
	virtual void OnReload() override;
	virtual void OnBeginThrowGrenade() override;
	virtual void OnReleaseGrenade() override;

	virtual void BeginCover(float dt) override;
	virtual void EndCover(float dt) override;

	void OnChangeAimMode();

	void OnAction();

	void OnSilentKill();

	/** Looking through weapon ironsights */
	void OnAiming();

	/** Releasing ironsight view */
	void OnReleaseAiming();

	void OnHoldBreathInternal(bool sound = false);
	void OnHoldBreath();

	void OnReleaseHoldBreathInternal(bool sound = false);
	void OnReleaseHoldBreath();

	/** Temporary method for action binding */
	void OnHitTest();

	/** Changing current weapon */
	void OnWeaponChange();
	void OnWeaponPickUp(AGun* pickedGun);

	/** Open/Hide SkillTreeMenu */
	void OnSkillTreeMenuButtonPressed();

	void OnSelectSkillRightPressed();
	void OnSelectSkillUpPressed();
	void OnSelectSkillLeftPressed();
	void OnSelectSkillDownPressed();

	void AddThreeTokens();

	bool HasSkill(int32 id);

	void OnListen();

	/** Show view cones for all enemies */
	void OnShowingViewCones();
	bool AreViewConesVisible() { return _bAreViewConesVisible; }
	void HideViewCones();

	void OnMapShowButtonPressed();
	void OnMapMarkInWorldButtonPressed();
	void OnMapMarkInWorldButtonReleased();

	void OnDeviceRecognition(FKey key);
	void OnDeviceRecognitionDPad();
	void OnDeviceRecognitionAxisMouse(float value);
	void OnDeviceRecognitionAxisGamepad(float value);
	
	UFUNCTION(BlueprintCallable, Category = "Player Character")
	EInputDeviceType GetLastInputDeviceType() { return _lastInputType; }

	/** HP PostProcess effects on hit and regain hp */
	void HealthPostProcess();

	/** Handles moving forward/backward */
	virtual void MoveForward(float Val) override;

	/** Handles stafing movement, left and right */
	virtual void MoveRight(float Val) override;

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	virtual void TurnAtRate(float Rate) override;

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	virtual void LookUpAtRate(float Rate) override;

	virtual void TurnAt(float Rate);
	virtual void LookUpAt(float Rate);

	void RegisterInteractionZoneOnInputAction(class UInteractionZone* uiz);
	void RegisterInteractionZoneOnInputSilentKill(class UInteractionZone* uiz);

	void UnregisterInteractionZoneOnInputAction(class UInteractionZone* uiz);
	void UnregisterInteractionZoneOnInputSilentKill(class UInteractionZone* uiz);

	class UInputComponent* GetMyInputComponent() { return _myInputComponent; }

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	bool GetIsAiming() { return _bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	float GetAimAlpha() { return _aimAlpha; }

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	bool GetIsAimingAndAimingInProgress() { return _bIsAiming || _bAimingInProgress; }

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	bool GetIsHoldingBreath() { return _bIsHoldingBreath; }

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	bool GetIsGunHidden() { return _gunHideDetectorOverlapCount > 0; }

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	bool GetIsGrenadeHeld() { return Grenade != NULL; }

	// IGenericTeamAgentInterface

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const { return 1; }

	// end of IGenericTeamAgentInterface

	// IAISightTargetInterface

	/**	Implementation should check whether from given ObserverLocation
	*	implementer can be seen. If so OutSeenLocation should contain
	*	first visible location
	*  Return sight strength for how well the target is seen.
	*/
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = NULL) const override;

	// end of IAISightTargetInterface

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// Method to invoke SkillTree method and invoke one of the method from FunctionPtr Array
	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool BuySkill(int id);
	
};
