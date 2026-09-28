// Fill out your copyright notice in the Description page of Project Settings.


#include "RecallProjectile.h"
#include "RecallWeapon.h"
#include "RecallTarget.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ARecallProjectile::ARecallProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;	
	CollisionComponent->InitSphereRadius(5.0f);

	CollisionComponent->OnComponentHit.AddDynamic(this, &ARecallProjectile::OnProjectileHit);

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARecallProjectile::OnProjectileOverlap);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));	
	ProjectileMesh->SetupAttachment(RootComponent);	
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));	
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // No gravity

}

// Called when the game starts or when spawned
void ARecallProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARecallProjectile::StartRecall()
{
	// Change the state to recalling
	CurrentState = ERecallProjectileState::Recalling;

	// Clear the list of hit actors to allow penetration again
	HitActors.Empty();

	// Reactivate the projectile movement component to start moving towards the weapon
	ProjectileMovement->Activate();
	UE_LOG(LogTemp, Warning, TEXT("Projectile entered recalling state."));
}

void ARecallProjectile::SetOwnerWeapon(ARecallWeapon* Weapon)
{
	OwnerWeapon = Weapon;
}

// Called every frame
void ARecallProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == ERecallProjectileState::Recalling && IsValid(OwnerWeapon))
	{
		FVector Direction = (OwnerWeapon->GetRecallLocation() - GetActorLocation()).GetSafeNormal();

		ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;

		float DistanceToWeapon = FVector::Dist(GetActorLocation(), OwnerWeapon->GetRecallLocation());
		if (DistanceToWeapon < 50.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Projectile reached the weapon."));
			OwnerWeapon->OnProjectileReturned(this);
			Destroy();
		}
	}
}

void ARecallProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,FVector NormalImpulse,const FHitResult& Hit)
{
	// If the projectile hits something else, we can embed it into that surface
	if (CurrentState == ERecallProjectileState::Travelling)
	{
		CurrentState = ERecallProjectileState::Embedded;

		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();

		UE_LOG(LogTemp, Warning, TEXT("Projectile embedded."));
	}
}

void ARecallProjectile::OnProjectileOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ARecallTarget* Target = Cast<ARecallTarget>(OtherActor);

	if (Target && !HitActors.Contains(OtherActor))
	{
		HitActors.Add(OtherActor);

		// Determine the damage amount based on the current state
		float DamageAmount =
			(CurrentState == ERecallProjectileState::Recalling)
			? RecallDamage
			: OutgoingDamage;

		Target->TakeRecallDamage(DamageAmount);

		UE_LOG(LogTemp, Warning, TEXT("Projectile penetrated target."));
	}
}