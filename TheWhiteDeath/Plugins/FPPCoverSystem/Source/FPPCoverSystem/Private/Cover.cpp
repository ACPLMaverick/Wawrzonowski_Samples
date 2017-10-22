// Fill out your copyright notice in the Description page of Project Settings.

#include "UFPPCoverSystemPrivatePCH.h"
#include "Cover.h"
#include "CoverCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ACover::ACover() :
	CoverEmergeDirection(0.0f, 1.0f, 1.0f),
	NeedToCrouch(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InternalCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("InternalCollider"));
	InternalCollider->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	InternalCollider->SetCollisionProfileName(TEXT("OverlapAll"));
	InternalCollider->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	InternalCollider->SetSimulatePhysics(false);
	InternalCollider->SetMobility(EComponentMobility::Static);
	RootComponent = InternalCollider;

	ExternalCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ExternalCollider"));
	ExternalCollider->SetBoxExtent(FVector(64.0f, 64.0f, 64.0f));
	ExternalCollider->SetCollisionProfileName(TEXT("OverlapAll"));
	InternalCollider->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	ExternalCollider->SetSimulatePhysics(false);
	ExternalCollider->SetMobility(EComponentMobility::Static);
	ExternalCollider->SetupAttachment(InternalCollider);
}

// Called when the game starts or when spawned
void ACover::BeginPlay()
{
	Super::BeginPlay();
	
	InternalCollider->OnComponentBeginOverlap.Clear();
	InternalCollider->OnComponentEndOverlap.Clear();
	ExternalCollider->OnComponentBeginOverlap.Clear();
	ExternalCollider->OnComponentEndOverlap.Clear();
	InternalCollider->OnComponentBeginOverlap.AddDynamic(this, &ACover::OnInternalBeginOverlap);
	InternalCollider->OnComponentEndOverlap.AddDynamic(this, &ACover::OnInternalEndOverlap);
	ExternalCollider->OnComponentBeginOverlap.AddDynamic(this, &ACover::OnExternalBeginOverlap);
	ExternalCollider->OnComponentEndOverlap.AddDynamic(this, &ACover::OnExternalEndOverlap);
	
	_boundUpWorld = ExternalCollider->GetScaledBoxExtent().Z * ExternalCollider->GetUpVector() * 0.5f + GetActorLocation();
	_boundLeftWorld = ExternalCollider->GetScaledBoxExtent().Y * ExternalCollider->GetRightVector() * -0.5f + 
		ExternalCollider->GetScaledBoxExtent().X * ExternalCollider->GetForwardVector() * -0.5f +
		ExternalCollider->GetScaledBoxExtent().Z * ExternalCollider->GetUpVector() * -0.5f +
		GetActorLocation();
	_boundRightWorld = ExternalCollider->GetScaledBoxExtent().Y * ExternalCollider->GetRightVector() * 0.5f +
		ExternalCollider->GetScaledBoxExtent().X * ExternalCollider->GetForwardVector() * 0.5f +
		ExternalCollider->GetScaledBoxExtent().Z * ExternalCollider->GetUpVector() * 0.5f +
		GetActorLocation();

	_boundRightRearWorld = ExternalCollider->GetScaledBoxExtent().Y * ExternalCollider->GetRightVector() * 0.5f +
		ExternalCollider->GetScaledBoxExtent().X * ExternalCollider->GetForwardVector() * -0.5f +
		ExternalCollider->GetScaledBoxExtent().Z * ExternalCollider->GetUpVector() * -0.5f +
		GetActorLocation();
}

// Called every frame
void ACover::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ACover::OnInternalBeginOverlap(UPrimitiveComponent * overlappedComponent, AActor * otherActor, UPrimitiveComponent * otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
	ACoverCharacter* character = Cast<ACoverCharacter>(otherActor);
	if (character != NULL && !_charactersInCoverArea.Contains(character) && otherComp->IsA<UCapsuleComponent>())
	{
		character->OnEnterCoverArea(this);
		_charactersInCoverArea.AddUnique(character);
		UE_LOG(LogTemp, Log, TEXT("ENTER"));
	}
}

void ACover::OnExternalBeginOverlap(UPrimitiveComponent * overlappedComponent, AActor * otherActor, UPrimitiveComponent * otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{

}

void ACover::OnInternalEndOverlap(UPrimitiveComponent * overlappedComponent, AActor * otherActor, UPrimitiveComponent * otherComp, int32 otherBodyIndex)
{

}

void ACover::OnExternalEndOverlap(UPrimitiveComponent * overlappedComponent, AActor * otherActor, UPrimitiveComponent * otherComp, int32 otherBodyIndex)
{
	ACoverCharacter* character = Cast<ACoverCharacter>(otherActor);
	if (character != NULL && _charactersInCoverArea.Contains(character) && otherComp->IsA<UCapsuleComponent>())
	{
		character->OnExitCoverArea();
		_charactersInCoverArea.Remove(character);
		UE_LOG(LogTemp, Log, TEXT("EXIT"));
	}
}

FVector ACover::GetReflectedPositionAcrossCover(FVector currentPosition, FVector currentDirection)
{
	FVector dir = GetActorLocation() - currentPosition;
	dir = dir.MirrorByVector(GetActorRightVector());
	dir += GetActorLocation();
	dir.Z = currentPosition.Z;

	float offset = 25.0f;

	return dir + offset * GetCoverForward(currentPosition);
}

FVector ACover::GetCoverForward(FVector currentPosition)
{
	FVector dir = (GetActorLocation() - currentPosition);
	dir.Z = 0.0f;
	dir.Normalize();
	return dir;
}

FVector ACover::GetCoverRight(FVector currentPosition)
{
	return -FVector::CrossProduct(GetCoverForward(currentPosition), GetActorUpVector());
}