// Fill out your copyright notice in the Description page of Project Settings.

#include "CGGameState.h"
#include "Net/UnrealNetwork.h"

void ACGGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}


void ACGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGGameState, WaveState);

}

void ACGGameState::SetWaveState(EWaveState NewState)
{
	if (Role == ROLE_Authority)
	{
		EWaveState OldState = WaveState;

		WaveState = NewState;

		OnRep_WaveState(OldState);
	}
	
}
