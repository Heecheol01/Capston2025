// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CastingBreath.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTTask_CastingBreath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_CastingBreath();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	float ElapsedTime = 0.f;
	bool bDidBreath = false;

	float DamageAccum = 0.f;

	UPROPERTY(EditAnywhere, Category = Breath)
	float DamageInterval = 0.9f;

	UPROPERTY(EditAnywhere, Category = Breath)
	float CastTime = 3.f;

	UPROPERTY(EditAnywhere, Category = Breath)
	float BreathTime = 3.f;

	UPROPERTY(EditAnywhere, Category = Breath)
	float Range = 800.f;

	UPROPERTY(EditAnywhere, Category = Breath)
	float Angle = 30.f;

	UPROPERTY(EditAnywhere, Category = Breath)
	UAnimMontage* CastingMontage;
	
	UPROPERTY(EditAnywhere, Category = Breath)
	UAnimMontage* BreathMontage;
};
