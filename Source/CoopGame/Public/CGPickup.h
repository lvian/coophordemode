// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGPickup.generated.h"

class USphereComponent;
class UDecalComponent;
class ACGPowerup;

UCLASS()
class COOPGAME_API ACGPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACGPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, Category="Componente")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Componente")
	UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "PowerUp")
	TSubclassOf<ACGPowerup> PowerupClass;

	void Respawn();

	FTimerHandle TimerHandle_RespawnTimer;

	ACGPowerup* PowerupInstance;

	UPROPERTY(EditAnywhere, Category = "Componente")
	float CooldownDuration;

public:	
	
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
