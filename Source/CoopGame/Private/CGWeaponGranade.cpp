// Fill out your copyright notice in the Description page of Project Settings.

#include "CGWeaponGranade.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"



void ACGWeaponGranade::Fire()
{

	//trace world, from pawn to crosshair
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);

	}

}