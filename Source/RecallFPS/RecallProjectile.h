// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RecallProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UPrimitiveComponent;

UENUM()
enum class ERecallProjectileState : uint8
{
	Travelling,
	Embedded,
	Recalling
};

class ARecallWeapon;

UCLASS()
class RECALLFPS_API ARecallProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float OutgoingDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float RecallDamage = 30.0f;
	
public:	
	// Sets default values for this actor's properties
	ARecallProjectile();
	void StartRecall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetOwnerWeapon(ARecallWeapon* Weapon);

private:

	ERecallProjectileState CurrentState = ERecallProjectileState::Travelling;

	UPROPERTY()
	ARecallWeapon* OwnerWeapon;

	UPROPERTY()
	TArray<AActor*> HitActors;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
};
