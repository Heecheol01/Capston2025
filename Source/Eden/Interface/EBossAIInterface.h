// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EBossAIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEBossAIInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DELEGATE(FBaseAttackFinished);

/**
 * 
 */
class EDEN_API IEBossAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;

	virtual void SetAIAttackDelegate(const FBaseAttackFinished& InOnAttackFinished) = 0;
	virtual void AttackByBoss() = 0;
};
