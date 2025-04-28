// Fill out your copyright notice in the Description page of Project Settings.


#include "EAoEActor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "Character/ECharacterBoss.h"
#include "UI/DamageFloatingText.h"

// Sets default values
AEAoEActor::AEAoEActor()
{
 	LifeTime = 10.f;
	Damage = 20.f;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(150.f);
	CollisionComponent->SetCollisionProfileName(TEXT("EAoE"));

	RootComponent = CollisionComponent;
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComponent->SetupAttachment(CollisionComponent);
}

// Called when the game starts or when spawned
void AEAoEActor::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEAoEActor::OnSphereOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AEAoEActor::OnSphereEndOverlap);
	
	SetLifeSpan(LifeTime);

	GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &AEAoEActor::ApplyContinuousDamage, 1.0f, true);

#if ENABLE_DRAW_DEBUG
	// SphereComponent의 실제 반경 계산 (월드 스케일 반영)
	float SphereRadius = CollisionComponent->GetScaledSphereRadius();
	
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		SphereRadius,
		16,
		FColor::Green,
		false,       // false: 일정 시간 후 사라짐
		LifeTime,    // 이 AoEActor가 사라질 때까지 표시
		0,
		2.0f         // 선 두께
	);
#endif
}

void AEAoEActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DamageTimerHandle);
	
	Super::EndPlay(EndPlayReason);
}

void AEAoEActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	
	if (APawn* OverlappedPawn = Cast<APawn>(OtherActor))
	{
		OverlappingPawns.Add(OverlappedPawn);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *OverlappedPawn->GetName());
	}
}

void AEAoEActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;
	
	if (APawn* OverlappedPawn = Cast<APawn>(OtherActor))
	{
		OverlappingPawns.Remove(OverlappedPawn);
	}
}

void AEAoEActor::ApplyContinuousDamage()
{
	AECharacterBoss* Boss = Cast<AECharacterBoss>(GetOwner());
	if (!Boss) return;
	
	for (APawn* Pawn : OverlappingPawns)
	{
		if (!IsValid(Pawn)) continue;
		UGameplayStatics::ApplyDamage(Pawn, Damage, Pawn->GetController(), this, nullptr);
		GetWorld()->SpawnActor<ADamageFloatingText>(ADamageFloatingText::StaticClass(),Pawn->GetActorLocation() + FVector(0, 0, 100.f),FRotator::ZeroRotator)->Init(Damage);
		Boss->HealUp(Damage * 10.f);
	}
}


