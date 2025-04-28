// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/EBossAttackableProjectile.h"

#include "EAoEActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AEBossAttackableProjectile::AEBossAttackableProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(15.f);
	CollisionComponent->SetCollisionProfileName(TEXT("EProjectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AEBossAttackableProjectile::OnHit);
	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->InitialSpeed = 0.f;
	MovementComponent->MaxSpeed = 3000.f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bShouldBounce = false;

	InitialLifeSpan = 10.f;
}

void AEBossAttackableProjectile::InitProjectile(float InSpeed)
{
	if (MovementComponent)
	{
		MovementComponent->Velocity = GetActorForwardVector() * InSpeed;
	}
}

// Called when the game starts or when spawned
void AEBossAttackableProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AEBossAttackableProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	FVector SpawnLocation = Hit.ImpactPoint + FVector::UpVector * 0.01f;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	GetWorld()->SpawnActor<AEAoEActor>(AoEActorClass, SpawnLocation, SpawnRotation, SpawnParameters);
	
	Destroy();
}
