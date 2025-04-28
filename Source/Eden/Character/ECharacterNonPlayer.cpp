// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ECharacterNonPlayer.h"

#include "ECharacterPlayer.h"
#include "AI/GeneralAI/EAIGeneralController.h"
#include "UI/EWidgetComponent.h"
#include "CharacterStat/ECharacterStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item\EDroppedItem.h"
#include "Physics/ECollision.h"
#include "UI/EHpBarWidget.h"

AECharacterNonPlayer::AECharacterNonPlayer()
{
	AIControllerClass = AEAIGeneralController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ENPCCAPSULE);

	static ConstructorHelpers::FObjectFinder<UEWeaponDataAsset> OneDataAssetRef(TEXT("/Script/Eden.EWeaponDataAsset'/Game/Eden/GameData/WeaponData/DA_NPCAttack.DA_NPCAttack'"));
	if (OneDataAssetRef.Object)
	{
		CurrentWeaponData = OneDataAssetRef.Object;	
	}

	HpBar = CreateDefaultSubobject<UEWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0, 0, 250.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/Eden/UI/WBP_HpBarWidget.WBP_HpBarWidget_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(100.f, 10.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AECharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AECharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->SetMaxHp(100.f);
}

void AECharacterNonPlayer::SetDead()
{
	Super::SetDead();


	if (AEAIGeneralController* AEIController = Cast<AEAIGeneralController>(GetController()))
	{
		AEIController->StopGeneralAI();
	}

	AECharacterPlayer* Player = Cast<AECharacterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player)
	{
		if (UECharacterStatComponent* StatComponent = Player->FindComponentByClass<UECharacterStatComponent>())
		{
			HandleDrop();
		}
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}

float AECharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AECharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float AECharacterNonPlayer::GetAIAttackRange()
{
	return 140.0f;
}

float AECharacterNonPlayer::GetAITurnSpeed()
{
	return 2.0f;
}

void AECharacterNonPlayer::SetAIAttackDelegate(const FGeneralAIAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AECharacterNonPlayer::AttackByAI()
{
	ProcessComboCommand();
}

void AECharacterNonPlayer::NotifyComboActionEnd()
{
	Super::NotifyComboActionEnd();
	OnAttackFinished.ExecuteIfBound();
}

void AECharacterNonPlayer::HandleDrop()
{
	if(!DropData) return;

	// 경험치 지급
	if(AECharacterPlayer* Player = Cast<AECharacterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(),0)))
	{
		if(UECharacterStatComponent* StatComponent = Player->FindComponentByClass<UECharacterStatComponent>())
		{
			StatComponent->OnExpGain.Broadcast(DropData->Exp);
			//StatComponent->OnGoldGain.Broadcast(DropData->Gold); // 나중에 골드 추가 시 사용
		}
	}

	// 아이템 드랍
	for(const FDropEntry& Entry : DropData->DropItems)
	{
		if(Entry.Item.IsNull()) continue;
		if(FMath::FRand() > Entry.DropChance) continue;

		int32 Count = FMath::RandRange(Entry.MinCount,Entry.MaxCount);
		UEItemDataAsset* ItemAsset = Entry.Item.LoadSynchronous();
		if(!ItemAsset) continue;

		FVector SpawnLoc = GetActorLocation();
		FRotator SpawnRot = FRotator::ZeroRotator;

		for (int32 i = 0; i < Count; i++)
		{
			AEDroppedItem* DroppedItem = GetWorld()->SpawnActor<AEDroppedItem>(DroppedItemClass,SpawnLoc,SpawnRot);
			DroppedItem->Init(ItemAsset);
		}
	}
}

void AECharacterNonPlayer::SetUpCharacterWidget(class UEUserWidget* InUserWidget)
{
	if (UEHpBarWidget* HpBarWidget = Cast<UEHpBarWidget>(InUserWidget))
	{
		HpBarWidget->BindStatComponent(Stat);
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UEHpBarWidget::UpdateHpBar);
	}
}
