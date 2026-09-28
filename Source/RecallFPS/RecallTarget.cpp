// Fill out your copyright notice in the Description page of Project Settings.


#include "RecallTarget.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ARecallTarget::ARecallTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	RootComponent = TargetMesh;

}

// Called when the game starts or when spawned
void ARecallTarget::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

// Called every frame
void ARecallTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARecallTarget::TakeRecallDamage(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    UE_LOG(LogTemp,Warning,TEXT("Target took %.1f damage. Health: %.1f / %.1f"),DamageAmount,CurrentHealth,MaxHealth);
}