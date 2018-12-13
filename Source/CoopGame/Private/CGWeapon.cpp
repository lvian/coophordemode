// Fill out your copyright notice in the Description page of Project Settings.

#include "CGWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

int32 DebugWeaponDrawing;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for WEapons"),
	ECVF_Cheat);

// Sets default values
ACGWeapon::ACGWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");

	MuzzleSocketName = "MuzzleSocket";

	TracerTargetName = "Target";
	
	BaseDamage = 20;

	BulletSpread = 2.0f;

	RateOfFire = 600;

	SetReplicates(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void ACGWeapon::BeginPlay()
{
	Super::BeginPlay();

	LastFireTime = GetWorld()->TimeSeconds;
	TimeBetweenShots = 60 / RateOfFire;
}


void ACGWeapon::Fire()
{

	if (Role != ROLE_Authority)
	{
		ServerFire();
	}

	//trace world, from pawn to crosshair
	AActor* MyOwner = GetOwner();
	
	LastFireTime = GetWorld()->TimeSeconds;

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection =  FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		//Tracer end point
		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLISION_WEAPON, QueryParams))
		{
			//Hit something
			AActor* HitActor = Hit.GetActor();



			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESH_VULNERABLE)
			{
				ActualDamage *= 2;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			PlayImpact(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.f, 0, 1.f);
		}


		PlayFireEffects(TracerEndPoint);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ACGWeapon::PlayImpact(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
		case SURFACE_FLESH_DEFAULT:
			SelectedEffect = DefaultImpactEffect;
			break;
		case SURFACE_FLESH_VULNERABLE:
			SelectedEffect = FleshImpactEffect;
			break;
		default:
			SelectedEffect = DefaultImpactEffect;
			break;
	}


	//UE_LOG(LogClass, Warning, TEXT("Surface: %s"), *SelectedEffect->GetName());
	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector  ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}

	
}

void ACGWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ACGWeapon::ServerFire_Validate()
{
	return true;
}



void ACGWeapon::PlayFireEffects(const FVector &TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}

	}

	APawn* Owner = Cast<APawn>(GetOwner());
	if (Owner)
	{
		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(PlayCameraShake);
		}
	}
	

}


void ACGWeapon::StartFire()
{
	float FirstDelay = FMath::Max( LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.f );
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ACGWeapon::Fire, TimeBetweenShots, true, FirstDelay);

}


void ACGWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);

}


void ACGWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpact(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}



void ACGWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGWeapon, HitScanTrace, COND_SkipOwner);

}