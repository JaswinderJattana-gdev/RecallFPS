// Fill out your copyright notice in the Description page of Project Settings.


#include "RecallProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ARecallProjectile::ARecallProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;	
	CollisionComponent->InitSphereRadius(5.0f);

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

// Called every frame
void ARecallProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

