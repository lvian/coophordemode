// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CGTrackerBot.generated.h"


class UCGHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ACGTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACGTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComp;


	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category="Tracker")
	float MovementForce;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float RequiredDistanceToTarget;

	UCGHealthComponent* HealthComp;

	UFUNCTION()
	void HandleTakeDamage(UCGHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	UParticleSystem* ExplosionEffect;

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionDamage;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USoundCue* SelfDestructionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USoundCue* ExplodeSound;

	FTimerHandle TimerHandle_DamageSelf;

	void DamageSelf();

	bool bStartedSelfDestruction;

	void OnCheckNearbyBots();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	int32 PowerLevel;

	FTimerHandle TimerHandle_RefreshPath;

	void RefreshPath();
};
