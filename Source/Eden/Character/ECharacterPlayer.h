// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ECharacterBase.h"
#include "InputActionValue.h"
#include "GameData/EWeaponDataAsset.h"
#include "Interface/EAnimationBowInterface.h"
#include "Interface/ECharacterHUDInterface.h"
#include "Item/EBothSkillVFXActor.h"
#include "UI/EInventoryWidget.h"
#include "UI/ECrosshairWidget.h"
#include "UI/EStatPanelWidget.h"
#include "ECharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class EDEN_API AECharacterPlayer : public AECharacterBase, public IEAnimationBowInterface, public IECharacterHUDInterface
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> OpenInventoryAction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Input,Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> BowZoomAction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Input,Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> OpenStatAction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Input,Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SkillAction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Input,Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Input,Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> OpenSettingAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Input,Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;

	void TryBowChargeStart();
	void TryBowChargeEnd();
	void Attack();

	// 무기 섹션
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* BothHand_WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* OneHandL_WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* OneHandR_WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* Bow_WeaponMesh;

// 무기스왑 애니메이션
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	UEWeaponDataAsset* OneHandedData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	UEWeaponDataAsset* BowData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	UEWeaponDataAsset* BothHandedData;
	
	UEWeaponDataAsset* PendingWeaponData;

	void SwapOneHanded();
	void SwapBow();
	void SwapBothHanded();

	void PlayWeaponSwapMontage(UEWeaponDataAsset* NewWeaponData);
	void OnWeaponSwapMontageEnded(UAnimMontage* Montage, bool bInterrupted);

// 인벤토리 섹션
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TObjectPtr<class UEInventoryComponent> InventoryComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UEInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	UEInventoryWidget* InventoryWidgetInstance;

	bool bInventoryOpen = false;

	void ToggleInventoryUI();
	
// 활 섹션
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = UI)
	TSubclassOf<UECrosshairWidget> CrosshairWidgetClass;

	UPROPERTY()
	UECrosshairWidget* CrosshairWidgetInstance;

	virtual void ShootArrow() override;

	void BowZoomIn();
	void BowZoomOut();

	bool bIsAttackInput = false;
	bool bIsZoomedIn = false;

// 스킬 섹션
protected:
	bool bCanUseSkill = true;
	FTimerHandle SkillCooldownTimerHandle;
	
	void ExecuteSkill();
	void SkillEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void ResetSkillCooldown();

	APawn* FindNearestPawnInAttackRange();
	virtual void ShootHomingArrow(APawn* Nearest) override;

// 양손 무기 스킬 섹션
protected:
	TArray<FVector> VFXSpawnLocations;
	int32 VFXSpawnIndex = 0;
	FTimerHandle VFXTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	UParticleSystem* SkillEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	UParticleSystem* LastSkillEffect;
	
	virtual void ExecuteBothSkill() override;
	void SpawnNextVFX();
	void SpawnSequentialVFX();
	void SpawnLastVFX();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VFX)
	TSubclassOf<class AEBothSkillVFXActor> BothSkillVFXActor;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skill)
	APawn* NearestTargetPawn = nullptr;

// 경험치 섹션
protected:
	UFUNCTION()
	void ExpGain(float InExp);
	
// UI 섹션
protected:
	UPROPERTY()
	TObjectPtr<class UEHUDWidget> HUDWidgetInstance;
	
	virtual void SetupHUDWidget(class UEHUDWidget* InHUDWidget) override;

//스탯 섹션 (StatComponent는 Base의 공통컴포넌트로 생략)
public:
	void ToggleStatUI();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = UI)
	TSubclassOf<UEStatPanelWidget> StatPanelWidgetClass;

	UPROPERTY()
	UEStatPanelWidget* StatPanelWidgetInstance;

	bool bStatPanelOpen = false;

	UFUNCTION(BlueprintCallable)
	void ToggleSettingUI();

	UPROPERTY(EditAnywhere,Category = UI)
	TSubclassOf<UUserWidget> SettingUIClass;

	UPROPERTY()
	UUserWidget* SettingUIInstance;

	bool bSettingUIOpen = false;
};
