// Fill out your copyright notice in the Description page of Project Settings.

#include "TheWhiteDeath.h"
#include "Grenade.h"


// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade"));
	//Mesh->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh->bCastDynamicShadow = true;
	Mesh->CastShadow = true;
	Mesh->CanEditSimulatePhysics();
	RootComponent = Mesh;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGrenade::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 200.f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = true;

	//Add Radial Force to Fire Impulse
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));

}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenade::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	TickPin(DeltaTime);
}

void AGrenade::TickPin(float DeltaTime)
{
	if (_bStartTimer)
	{
		_pinTimer += DeltaTime;

		if (_bIsGrenadeReady && ProjectileMovement->InitialSpeed <= 1000.f)
		{
			ProjectileMovement->InitialSpeed += 10.f;
		}

		RadialForce->SetWorldLocation(GetActorLocation());

		if (_pinTimer >= _timeToExplode)
		{
			Explode();
		}
	}
}

void AGrenade::OnReady()
{
	ProjectileMovement->InitialSpeed = 800.f;
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);

	_bIsGrenadeReady = true;
	_bStartTimer = true;
	_pinTimer = 0.0f;
}

void AGrenade::OnRelease()
{
	_bIsGrenadeReady = false;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->AddImpulseAtLocation(ProjectileMovement->Velocity * 3.0f, GetActorLocation());

	RadialForce->SetWorldLocation(GetActorLocation());

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::SanitizeFloat(GetActorLocation().X) + " " + FString::SanitizeFloat(GetActorLocation().Y) + " " + FString::SanitizeFloat(GetActorLocation().Z));

	RadialForce->FireImpulse();
}

void AGrenade::Explode_Implementation()
{
	_bStartTimer = false;
	_bIsGrenadeReady = false;
}

void AGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "BOOM!");
}
