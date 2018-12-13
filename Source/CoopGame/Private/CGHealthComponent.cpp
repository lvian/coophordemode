// Fill out your copyright notice in the Description page of Project Settings.

#include "CGHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "CGGameMode.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UCGHealthComponent::UCGHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	InitialHealth = 100;

	SetIsReplicated(true);

	TeamNum = 255;

	bIsDead = false;
}


// Called when the game starts
void UCGHealthComponent::BeginPlay()
{
	 Super::BeginPlay();

	 if (GetOwnerRole() == ROLE_Authority)
	 {
		 AActor* Owner = GetOwner();

		 if (Owner)
		 {
			 Owner->OnTakeAnyDamage.AddDynamic(this, &UCGHealthComponent::HandleTakeAnyDamage);
		 }
	 }

	 Health = InitialHealth;
}

void UCGHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UCGHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0 || bIsDead) { return; }


	//If in the same team, do not apply damage
	if ( DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser) ){ return; }

	Health = FMath::Clamp(Health - Damage, 0.f, InitialHealth);

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	ACGGameMode* GM = Cast<ACGGameMode>(GetWorld()->GetAuthGameMode());

	if (Health <= 0)
	{
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
	
}


void UCGHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCGHealthComponent, Health);

}

void UCGHealthComponent::Heal(float HealAmout)
{
	if (HealAmout <= 0 || Health <= 0) { return; }

	Health = FMath::Clamp(Health + HealAmout, 0.f, InitialHealth);
	UE_LOG(LogClass, Warning, TEXT("Health: %f"), Health);

	OnHealthChanged.Broadcast(this, Health, -HealAmout, nullptr, nullptr, nullptr);
}

bool UCGHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (!ActorA || !ActorB) { return false; }

	UCGHealthComponent* HealthCompA = Cast<UCGHealthComponent>(ActorA->GetComponentByClass(UCGHealthComponent::StaticClass()));
	UCGHealthComponent* HealthCompB = Cast<UCGHealthComponent>(ActorB->GetComponentByClass(UCGHealthComponent::StaticClass()));

	if (!HealthCompA || !HealthCompB){ return false; }

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}

float UCGHealthComponent::GetHealth() const
{
	return Health;
}