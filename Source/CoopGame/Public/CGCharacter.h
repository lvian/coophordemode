// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CGCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ACGWeapon;
class UCGHealthComponent;

UCLASS()
class COOPGAME_API ACGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACGCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void DoJump();

	void BeginZoom();
	
	void EndZoom();
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(Replicated)
	ACGWeapon* CurrentWeapon;
	
	

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Weapon")
	UCGHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ACGWeapon> StartingWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category="Weapon")
	FName WeaponAttachSocketName;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta= (ClapMin = 0.1, ClampMax = 100))
	float ZoomInterpolationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ZoomedFOV;
	
	float DefaultFOV;

	UFUNCTION()
	void OnHealthChanged(UCGHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void StopFire();

	FVector GetPawnViewLocation() const override;
};
