// Fill out your copyright notice in the Description page of Project Settings.


#include "RecallProjectile.h"
#include "RecallWeapon.h"
#include "RecallTarget.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"

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

	RecallStartPoint = GetActorLocation();
	RecallProgress = 0.0f;

	FVector EndPoint = OwnerWeapon->GetRecallLocation();

	const int32 MaxPathAttempts = 8;
	bool bFoundClearPath = false;

	for (int32 Attempt = 0; Attempt < MaxPathAttempts; Attempt++)
	{
		FVector RandomOffset1 =
			FMath::VRand() * FMath::FRandRange(
				RecallCurveStrength * 0.5f,
				RecallCurveStrength
			);

		FVector RandomOffset2 =
			FMath::VRand() * FMath::FRandRange(
				RecallCurveStrength * 0.5f,
				RecallCurveStrength
			);

		RecallControlPoint1 =
			FMath::Lerp(RecallStartPoint, EndPoint, 0.3f)
			+ RandomOffset1;

		RecallControlPoint2 =
			FMath::Lerp(RecallStartPoint, EndPoint, 0.6f)
			+ RandomOffset2;

		if (IsRecallPathClear(EndPoint))
		{
			bFoundClearPath = true;

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Clear recall path found on attempt %d"),
				Attempt + 1
			);

			break;
		}
	}

	if (!bFoundClearPath)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No clear recall path found. Using fallback path.")
		);

		RecallControlPoint1 =
			FMath::Lerp(RecallStartPoint, EndPoint, 0.33f);

		RecallControlPoint2 =
			FMath::Lerp(RecallStartPoint, EndPoint, 0.66f);
	}

	FVector PreviousPoint = RecallStartPoint;

	for (int32 i = 1; i <= 20; i++)
	{
		float T = i / 20.0f;
		float OneMinusT = 1.0f - T;

		FVector CurvePoint =
			FMath::Pow(OneMinusT, 3) * RecallStartPoint
			+ 3.0f * FMath::Pow(OneMinusT, 2) * T * RecallControlPoint1
			+ 3.0f * OneMinusT * FMath::Pow(T, 2) * RecallControlPoint2
			+ FMath::Pow(T, 3) * EndPoint;

		DrawDebugLine(
			GetWorld(),
			PreviousPoint,
			CurvePoint,
			FColor::Green,
			false,
			3.0f,
			0,
			3.0f
		);

		PreviousPoint = CurvePoint;
	}

	if (IsRecallPathClear(EndPoint))
	{
		UE_LOG(LogTemp, Warning, TEXT("Recall path CLEAR"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Recall path BLOCKED"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Projectile entered recalling state."));
}

bool ARecallProjectile::IsRecallPathClear(const FVector& EndPoint) const
{
	FVector PreviousPoint = RecallStartPoint;

	for (int32 i = 1; i <= 20; i++)
	{
		float T = i / 20.0f;
		float OneMinusT = 1.0f - T;

		FVector CurvePoint =
			FMath::Pow(OneMinusT, 3) * RecallStartPoint
			+ 3.0f * FMath::Pow(OneMinusT, 2) * T * RecallControlPoint1
			+ 3.0f * OneMinusT * FMath::Pow(T, 2) * RecallControlPoint2
			+ FMath::Pow(T, 3) * EndPoint;

		FHitResult Hit;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(OwnerWeapon);

		if (OwnerWeapon)
		{
			AActor* ParentActor = OwnerWeapon->GetAttachParentActor();

			if (ParentActor)
			{
				QueryParams.AddIgnoredActor(ParentActor);
			}
		}

		bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			Hit,
			PreviousPoint,
			CurvePoint,
			ECC_WorldStatic,
			QueryParams
		);

		if (bBlocked)
		{
			if (Hit.GetActor())
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Recall path blocked by: %s"),
					*Hit.GetActor()->GetName()
				);
			}

			return false;
		}

		PreviousPoint = CurvePoint;
	}

	return true;
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
		RecallProgress += DeltaTime / RecallDuration;
		RecallProgress = FMath::Clamp(RecallProgress, 0.0f, 1.0f);

		float T = RecallProgress;
		float OneMinusT = 1.0f - T;

		FVector EndPoint = OwnerWeapon->GetRecallLocation();

		FVector CurvePosition =
			FMath::Pow(OneMinusT, 3) * RecallStartPoint
			+ 3.0f * FMath::Pow(OneMinusT, 2) * T * RecallControlPoint1
			+ 3.0f * OneMinusT * FMath::Pow(T, 2) * RecallControlPoint2
			+ FMath::Pow(T, 3) * EndPoint;

		SetActorLocation(CurvePosition, false);

		FVector Direction = (OwnerWeapon->GetRecallLocation() - GetActorLocation()).GetSafeNormal();

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