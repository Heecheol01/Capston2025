// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_StaggerCheck.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API UBTService_StaggerCheck : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_StaggerCheck();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bIsTriggered85 = false;

	bool bIsTriggered75 = false;;
};
