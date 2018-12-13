// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGWeapon.h"
#include "CGWeaponGranade.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ACGWeaponGranade : public ACGWeapon
{
	GENERATED_BODY()

public:


protected:

	void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category="Project")
	TSubclassOf<AActor> ProjectileClass;
	
};
