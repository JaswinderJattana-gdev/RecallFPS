// Fill out your copyright notice in the Description page of Project Settings.


#include "RecallWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ARecallWeapon::ARecallWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(WeaponMesh);
}

// Called when the game starts or when spawned
void ARecallWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARecallWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

