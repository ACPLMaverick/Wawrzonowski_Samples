// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class THEWHITEDEATH_API TWDUtilities
{
public:
	TWDUtilities();
	~TWDUtilities();

	static FORCEINLINE bool VTraceSphere
	(
		AActor* ActorToIgnore,
		const FVector& Start,
		const FVector& End,
		const float Radius,
		FHitResult& HitOut,
		ECollisionChannel TraceChannel = ECC_Pawn
	)
	{
		FCollisionQueryParams TraceParams(FName(TEXT("VictoreCore Trace")), true, ActorToIgnore);
		TraceParams.bTraceComplex = true;
		//TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;

		//Ignore Actors
		TraceParams.AddIgnoredActor(ActorToIgnore);

		//Re-initialize hit info
		HitOut = FHitResult(ForceInit);

		//Get World Source
		TObjectIterator< APlayerController > ThePC;
		if (!ThePC) return false;

		return ThePC->GetWorld()->SweepSingleByChannel(
			HitOut,
			Start,
			End,
			FQuat(),
			TraceChannel,
			FCollisionShape::MakeSphere(Radius),
			TraceParams
		);
	}

	static FORCEINLINE bool VTraceSphereMulti
	(
		AActor* ActorToIgnore,
		const FVector& Start,
		const FVector& End,
		const float Radius,
		TArray<FHitResult>& HitsOut,
		ECollisionChannel TraceChannel = ECC_Pawn
	)
	{
		FCollisionQueryParams TraceParams(FName(TEXT("VictoreCore Trace")), true, ActorToIgnore);
		TraceParams.bTraceComplex = true;
		//TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;

		//Ignore Actors
		TraceParams.AddIgnoredActor(ActorToIgnore);

		//Get World Source
		TObjectIterator< APlayerController > ThePC;
		if (!ThePC) return false;

		return ThePC->GetWorld()->SweepMultiByChannel(
			HitsOut,
			Start,
			End,
			FQuat(),
			TraceChannel,
			FCollisionShape::MakeSphere(Radius),
			TraceParams
		);
	}
};
