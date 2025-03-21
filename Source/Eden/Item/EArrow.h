// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EArrow.generated.h"

UCLASS()
class EDEN_API AEArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEArrow();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	class USphereComponent* CollisionComponent;

public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void InitProjectile(float InDamage, float InSpeed);

private:
	float Damage = 50.f;
};
