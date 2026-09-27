// Fill out your copyright notice in the Description page of Project Settings.


#include "RecallWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "RecallProjectile.h"

// Sets default values
ARecallWeapon::ARecallWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(WeaponMesh);

	RecallPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RecallPoint"));
	RecallPoint->SetupAttachment(WeaponMesh);
}

void ARecallWeapon::Fire()
{
	if (!ProjectileClass) return;

	FTransform MuzzleTransform = MuzzlePoint->GetComponentTransform();

	if (CurrentAmmo <= 0) return;

	ARecallProjectile* Projectile = GetWorld()->SpawnActor<ARecallProjectile>(ProjectileClass, MuzzleTransform);

	if (Projectile)
	{
		ActiveProjectiles.Add(Projectile);
		UE_LOG(LogTemp,Warning,TEXT("Active Projectiles: %d"),ActiveProjectiles.Num());
		Projectile->SetOwnerWeapon(this);
		CurrentAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("Ammo: %d / %d"), CurrentAmmo, MaxAmmo);
	}
}

void ARecallWeapon::Recall()
{
	for (ARecallProjectile* Projectile : ActiveProjectiles)
	{
		if(IsValid(Projectile))
		{
			Projectile->StartRecall();
		}
	}
}
// Called when the game starts or when spawned
void ARecallWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
	
}

// Called every frame
void ARecallWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ARecallWeapon::GetRecallLocation() const
{
	return RecallPoint->GetComponentLocation();
}

void ARecallWeapon::OnProjectileReturned(ARecallProjectile* Projectile)
{
	if (ActiveProjectiles.Contains(Projectile))
	{
		ActiveProjectiles.Remove(Projectile);
		UE_LOG(LogTemp, Warning, TEXT("Projectile returned. Active Projectiles: %d"), ActiveProjectiles.Num());
		CurrentAmmo++;
		UE_LOG(LogTemp, Warning, TEXT("Ammo restored: %d / %d"), CurrentAmmo, MaxAmmo);
	}
}