// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ECharacterBase.h"
#include "InputActionValue.h"
#include "ECharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API AECharacterPlayer : public AECharacterBase
{
	GENERATED_BODY()
	
public:
	AECharacterPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void SetDead() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> OneHandedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> BowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> BothHandedAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Attack();

// 무기스왑 애니메이션
protected:
	UPROPERTY(EditAnywhere, blueprintReadWrite, Category = WaeponAnimation)
	UAnimMontage* WeaponSwapMontage_OneHanded;

	UPROPERTY(EditAnywhere, blueprintReadWrite, Category = WaeponAnimation)
	UAnimMontage* WeaponSwapMontage_Bow;

	UPROPERTY(EditAnywhere, blueprintReadWrite, Category = WaeponAnimation)
	UAnimMontage* WeaponSwapMontage_BothHanded;

	EWeaponType PendingWeaponType;

	void SwapOneHanded();
	void SwapBow();
	void SwapBothHanded();

	void PlayWeaponSwapMontage(EWeaponType NewWeaponType, UAnimMontage* Montage);
	void OnWeaponSwapMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
