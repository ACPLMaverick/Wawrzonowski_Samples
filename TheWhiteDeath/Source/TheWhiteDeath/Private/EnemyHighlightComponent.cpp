// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "EnemyHighlightComponent.h"
#include "Components/PostProcessComponent.h"


UEnemyHighlightComponent::UEnemyHighlightComponent() :
	Super()
{
	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->bUnbound = true;

	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyHighlightComponent::BeginPlay()
{
	if (MHighlight != NULL)
	{
		MIDHighlight = UMaterialInstanceDynamic::Create(MHighlight, PostProcess);
		PostProcess->AddOrUpdateBlendable(MIDHighlight, 1.0f);
	}
}

void UEnemyHighlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (_bIsHighlighting && MIDHighlight != NULL)
	{
		float fadeIn = _timer / FadeInSeconds;
		fadeIn = FMath::Clamp(fadeIn, 0.0f, 1.0f);
		float fadeOut = FMath::Clamp(_timer - (FadeInSeconds + TimeSeconds), 0.0f, 1.0f) / FadeOutSeconds;
		float fadeTotal = FMath::Clamp(fadeIn - fadeOut, 0.0f, 1.0f);

		//UE_LOG(LogTemp, Log, TEXT("%f %f"), fadeIn, fadeOut);

		MIDHighlight->SetScalarParameterValue(TEXT("LinePower"), fadeTotal);
		MIDHighlight->SetScalarParameterValue(TEXT("LineRadius"), _radius);
		MIDHighlight->SetScalarParameterValue(TEXT("HighlightPower"), fadeTotal);

		if (_timer > (FadeInSeconds + FadeOutSeconds + TimeSeconds))
		{
			_bIsHighlighting = false;
			MIDHighlight->SetScalarParameterValue(TEXT("LinePower"), 0.0f);
			MIDHighlight->SetScalarParameterValue(TEXT("LineRadius"), 0.0f);
			MIDHighlight->SetScalarParameterValue(TEXT("HighlightPower"), 0.0f);
		}
		else
		{
			_radius += DeltaTime * RadiusSpeed;
			_timer += DeltaTime;
		}
	}
}

void UEnemyHighlightComponent::Highlight()
{
	if (!_bIsHighlighting)
	{
		_radius = 0.0f;
		_timer = 0.0f;
		_bIsHighlighting = true;
	}
}

void UEnemyHighlightComponent::UnHighlight()
{
	if (_bIsHighlighting)
	{
		_timer = FadeInSeconds + TimeSeconds;
	}
}
