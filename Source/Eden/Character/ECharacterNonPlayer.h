// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ECharacterBase.h"
#include "Interface/EGeneralAIInterface.h"
#include "Components/WidgetComponent.h"
#include "UI/EEnemyHPBarWidget.h"
#include "GameData/EDropDataAsset.h"
#include "Interface/ECharacterWidgetInterface.h"
#include "Item/EDroppedItem.h"
#include "ECharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API AECharacterNonPlayer : public AECharacterBase, public IEGeneralAIInterface, public IECharacterWidgetInterface
{
	GENERATED_BODY()
	
public:
	AECharacterNonPlayer();

	virtual void PostInitializeComponents() override;

protected:
	void BeginPlay() override;
	void SetDead() override;

protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FGeneralAIAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FGeneralAIAttackFinished OnAttackFinished;

	virtual void NotifyComboActionEnd() override;

public:
	// UPROPERTY(VisibleAnywhere,Category = UI)
	// class UWidgetComponent* HealthBarWidget; //적 체력바

	UPROPERTY(EditAnywhere,Category = Drop)
	UEDropDataAsset* DropData;  //드랍 아이템

	virtual void HandleDrop(); //드랍 함수
	UPROPERTY(EditAnywhere,Category = Drop)
	TSubclassOf<AEDroppedItem> DroppedItemClass;

	// UI 섹션
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HpBar;

	virtual void SetUpCharacterWidget(class UEUserWidget* InUserWidget) override;
};
