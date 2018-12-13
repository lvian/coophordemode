// Fill out your copyright notice in the Description page of Project Settings.

#include "CGCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "CGWeapon.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame.h"
#include "CGHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACGCharacter::ACGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	ZoomedFOV = 65.f;
	WeaponAttachSocketName = "Grip";

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<UCGHealthComponent>(TEXT("HealthComponent"));
	


}

// Called when the game starts or when spawned
void ACGCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	
	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ACGWeapon>(StartingWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}

	}
	

	HealthComp->OnHealthChanged.AddDynamic(this, &ACGCharacter::OnHealthChanged);
}

// Called every frame
void ACGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpolationSpeed);
	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ACGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACGCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ACGCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACGCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACGCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("UnCrouch", IE_Released, this, &ACGCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACGCharacter::DoJump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ACGCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ACGCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACGCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACGCharacter::StopFire);

}

FVector ACGCharacter::GetPawnViewLocation() const
{

	return CameraComp ? CameraComp->GetComponentLocation() : Super::GetPawnViewLocation();
}

void ACGCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ACGCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}


void ACGCharacter::BeginCrouch()
{
	Crouch();
}

void ACGCharacter::EndCrouch()
{
	UnCrouch();
}

void ACGCharacter::DoJump()
{
	Jump();
}

void ACGCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ACGCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ACGCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}

}

void ACGCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}

}

void ACGCharacter::OnHealthChanged(UCGHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser )
{
	if (Health <= 0 && !bDied)
	{
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.f);

	}

}

void ACGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGCharacter, CurrentWeapon);
	DOREPLIFETIME(ACGCharacter, bDied);

}