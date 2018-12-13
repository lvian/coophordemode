// Fill out your copyright notice in the Description page of Project Settings.

#include "CGGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "CGHealthComponent.h"
#include "CGGameState.h"
#include "CGPlayerState.h"

ACGGameMode::ACGGameMode()
{
	TimePrepareForWave = 3.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = ACGGameState::StaticClass();

	PlayerStateClass = ACGPlayerState::StaticClass();
}


void ACGGameMode::StartWave()
{
	WaveCount++;

	NumberOfBotsToSpawn = 3 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ACGGameMode::SpawnBotTimerElapsed, 1.0f, true);

	SetWaveState(EWaveState::WaveInProgress);
}

void ACGGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NumberOfBotsToSpawn--;

	if (NumberOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

void ACGGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaitingToComplete);
}

void ACGGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWavePrepareTime, this, &ACGGameMode::StartWave, TimePrepareForWave, false);

	SetWaveState(EWaveState::WaitingStart);
}

void ACGGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWavePrepareTime);

	if (NumberOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}
	bool bAreBotsAlive = false;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (!TestPawn || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UCGHealthComponent* HealthComp = Cast<UCGHealthComponent>( TestPawn->GetComponentByClass(UCGHealthComponent::StaticClass()) );

		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bAreBotsAlive = true;
			break;
		}
	}

	if (!bAreBotsAlive)
	{
		SetWaveState(EWaveState::WaitingToComplete);
		PrepareForNextWave();
	}
	
}

void ACGGameMode::CheckPlayersAlive()
{

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{

		APlayerController* PC = It->Get();

		if (PC && PC->GetPawn())
		{

			APawn * Pawn = PC->GetPawn();
			UCGHealthComponent* HealthComp = Cast<UCGHealthComponent>(Pawn->GetComponentByClass(UCGHealthComponent::StaticClass()));

			if (ensure(HealthComp) && HealthComp->GetHealth() > 0)
			{
				return; //Players alive
			}
		}
	}

	GameOver();
}

void ACGGameMode::GameOver()
{
	EndWave();
	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogClass, Warning, TEXT("All Players dead!"));
}



void ACGGameMode::SetWaveState(EWaveState NewState)
{
	ACGGameState* GS = GetGameState<ACGGameState>();
	
	if (ensureAlways(GS))
	{
		GS->SetWaveState( NewState);
	}
}

void ACGGameMode::RespawnDeadPlayers()
{

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{

		APlayerController* PC = It->Get();

		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}



void ACGGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ACGGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckWaveState();

}
