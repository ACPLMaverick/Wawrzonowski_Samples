// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "GameController.h"

AGameController* AGameController::_lastInstance;

// Sets default values
AGameController::AGameController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ensure(_lastInstance == NULL);
	AGameController::_lastInstance = this;

	MapTopLeftCorner = CreateDefaultSubobject<USceneComponent>(TEXT("MapTopLeftCorner"));
	RootComponent = MapTopLeftCorner;

	MapBottomRightCorner = CreateDefaultSubobject<USceneComponent>(TEXT("MapBottomRightCorner"));
	MapBottomRightCorner->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGameController::BeginPlay()
{
	Super::BeginPlay();
	_windTimer = _windCurrentDelay;
	_windVariationTimer = _windVariationCurrentDelay;
	_wind = FVector::ZeroVector;
	_newWind = FVector::ZeroVector;
	_windVariation = FVector::ZeroVector;
	_variationChangeMultipliers = FVector2D::ZeroVector;
	_newVariationChangeMultipliers = FVector2D::ZeroVector;
}

// Called every frame
void AGameController::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	TickWind(DeltaTime);
}

void AGameController::TickWind(float deltaTime)
{
	// generate new wind and its delay
	if (_windTimer >= _windCurrentDelay)
	{
		_windTimer = 0.0f;
		_windCurrentDelay = FMath::FRandRange(WindMinChangeDelay, WindMaxChangeDelay);
		_newWind = GenerateWindVector();
	}
	else
	{
		_windTimer += deltaTime;
	}

	// lerp to new wind if one is generated
	if (FMath::Abs(_wind.SizeSquared() - _newWind.SizeSquared()) > 0.001f)
	{
		_wind = FMath::VInterpTo(_wind, _newWind, deltaTime, WindChangeSpeed);
	}
	else
	{
		_wind = _newWind;
	}

	// calculate wind variation
	if (_windVariationTimer >= _windVariationCurrentDelay)
	{
		_windVariationTimer = 0.0f;
		_windVariationCurrentDelay = FMath::FRandRange(WindVariationMinChangeDelay, WindVariationMaxChangeDelay);
		_newVariationChangeMultipliers = FVector2D(FMath::FRand(), FMath::FRand()) * 20.0f;
	}
	else
	{
		_windVariationTimer += deltaTime;
	}

	if (FMath::Abs(_variationChangeMultipliers.SizeSquared() - _newVariationChangeMultipliers.SizeSquared()) < 0.001f)
	{
		_variationChangeMultipliers = FMath::Vector2DInterpTo(_variationChangeMultipliers, _newVariationChangeMultipliers, deltaTime, 50.0f);
	}
	else
	{
		_variationChangeMultipliers = _newVariationChangeMultipliers;
	}

	_windVariation = FVector(FMath::Sin(deltaTime * _variationChangeMultipliers.X), FMath::Cos(deltaTime * _variationChangeMultipliers.Y), 0.0f) * WindVariationMultiplier;
}

FVector AGameController::GenerateWindVector()
{
	FVector wind(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f);
	wind.Normalize();
	wind *= FMath::RandRange(WindMinForce, WindMaxForce);
	return wind;
}
