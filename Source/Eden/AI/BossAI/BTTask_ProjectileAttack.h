// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Item/EBossAttackableProjectile.h"
#include "BTTask_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTTask_ProjectileAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ProjectileAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void PredictAndDrawImpactPoint(UWorld* World, const FVector& StartLocation, const FVector& LaunchVelocity, float CollisionRadius, TArray<AActor*> ActorsToIgnore, float OverrideGravityZ);
	
	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class AEBossAttackableProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = Projectile)
	FVector SpawnOffset = FVector(0.f, 0.f, 50.f);

	UPROPERTY(EditAnywhere, Category = Projectile)
	float LaunchSpeed = 1000.f;
};
