// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StaggerPattern.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTTask_StaggerPattern : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_StaggerPattern();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stagger)
	UAnimMontage* StaggerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stagger)
	float StaggerDuration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stagger)
	float MaxStaggerGauge = 100.f;

private:
	float ElapsedTime = 0.f;

	bool bIsTaskActive = false;
};
