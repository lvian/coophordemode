// Fill out your copyright notice in the Description page of Project Settings.

#include "CGPowerup.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACGPowerup::ACGPowerup()
{
	PowerupInterval = 0.f;
	TotalNrOfTIcks = 0;
	TicksProcessed = 0;

	SetReplicates(true);

	bIsPowerUpActivated = false;
}




void ACGPowerup::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNrOfTIcks)
	{
		OnExpired();

		bIsPowerUpActivated = false;
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);

	}

}

void ACGPowerup::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerUpActivated);
}

void ACGPowerup::ActivatePowerup(AActor* OtherActor)
{
	OnActivated( OtherActor);

	bIsPowerUpActivated = true;
	OnRep_PowerupActive();
	
	if (PowerupInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ACGPowerup::OnTickPowerup, PowerupInterval, true);
	}
	else {
		OnTickPowerup();
	}


}



void ACGPowerup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGPowerup, bIsPowerUpActivated);

}