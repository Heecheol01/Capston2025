// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WideAoE.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTTask_WideAoE : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_WideAoE();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stagger)
	UAnimMontage* AoEMontage;

private:
	float ElapsedTime = 0.f;
	float CastTime = 1.5f;
	float AoERadius = 1000.f;

	bool bHasExploded = false;
};
