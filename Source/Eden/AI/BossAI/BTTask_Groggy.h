// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Groggy.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTTask_Groggy : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Groggy();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = Groggy)
	float WaitTime;

	float ElapsedTime = 0.f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Groggy)
	class UAnimMontage* GroggyAnimMontage;
	
};
