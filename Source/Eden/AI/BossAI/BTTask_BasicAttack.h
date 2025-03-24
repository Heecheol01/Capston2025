// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTTask_BasicAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_BasicAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
