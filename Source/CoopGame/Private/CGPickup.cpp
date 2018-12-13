// Fill out your copyright notice in the Description page of Project Settings.

#include "CGPickup.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"
#include "CGPowerup.h"

// Sets default values
ACGPickup::ACGPickup()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->SetSphereRadius(75.f);
	SetRootComponent(SphereComp);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComp->SetRelativeRotation(FRotator(90,0,0));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 10;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACGPickup::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority)
	{
		Respawn();
	}

}

void ACGPickup::Respawn()
{
	if (!PowerupClass) { return; }

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance= GetWorld()->SpawnActor<ACGPowerup>(PowerupClass, GetTransform(), SpawnParams);
}

void ACGPickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && PowerupInstance)
	{
		PowerupInstance->ActivatePowerup(OtherActor);
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ACGPickup::Respawn, CooldownDuration);
	}
}
