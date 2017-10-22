// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "SkillTreeWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEWHITEDEATH_API USkillTreeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

#pragma region Main Screen
	
#pragma region Properties

	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadWrite)
	int CurrentLevel;

	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadWrite)
	int XPCount;

	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadWrite)
	int XPToNextLevel;

	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadWrite)
	int TokenCount;

#pragma endregion

#pragma region Animations

	UFUNCTION(BlueprintNativeEvent)
	void PlayShowMenu();
	virtual void PlayShowMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void PlayHideMenu();
	virtual void PlayHideMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void PlayShowAcceptDialog();
	virtual void PlayShowAcceptDialog_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void PlayHideAcceptDialog();
	virtual void PlayHideAcceptDialog_Implementation();

#pragma endregion

#pragma region ControllerEvents

	UFUNCTION(BlueprintNativeEvent)
	void HoveredSkillMoveLeft();
	virtual void HoveredSkillMoveLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void HoveredSkillMoveUp();
	virtual void HoveredSkillMoveUp_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void HoveredSkillMoveRight();
	virtual void HoveredSkillMoveRight_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void HoveredSkillMoveDown();
	virtual void HoveredSkillMoveDown_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void ResetSkillTree();
	virtual void ResetSkillTree_Implementation();

#pragma endregion

#pragma endregion

#pragma region Notifications

#pragma region Properties

	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadWrite)
	bool IsNotificationAnimationPlaying;

#pragma endregion

#pragma region Animations

	UFUNCTION(BlueprintNativeEvent)
	void PlayShowNotification(FName s);
	virtual void PlayShowNotification_Implementation(FName s);

#pragma endregion

#pragma endregion
	
	
};
