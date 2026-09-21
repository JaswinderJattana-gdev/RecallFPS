// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RecallWeapon.generated.h"

class ARecallProjectile;

UCLASS()

class RECALLFPS_API ARecallWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* MuzzlePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RecallPoint;
	
public:	
	// Sets default values for this actor's properties
	ARecallWeapon();
	void Fire();
	void Recall();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ARecallProjectile> ProjectileClass;

	UPROPERTY();
	TArray<ARecallProjectile*> ActiveProjectiles;	

	FVector GetRecallLocation() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
