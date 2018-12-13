// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGPowerup.generated.h"

UCLASS()
class COOPGAME_API ACGPowerup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACGPowerup();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerupInterval;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	int32 TotalNrOfTIcks;

	int32 TicksProcessed;

	FTimerHandle TimerHandle_PowerupTick;

	UFUNCTION()
	void OnTickPowerup();

	UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
	bool bIsPowerUpActivated;

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerupStateChanged(bool bNewState);
public:	
	
	void ActivatePowerup(AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category="PowerUps")
	void OnActivated(AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerupTicked();

};
