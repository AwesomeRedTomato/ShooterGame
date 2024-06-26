// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABoss::ABoss()
{
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetMesh(), FName(TEXT("HammerCenter")));
	
	MaxHealth = 2000.0f;
	Health = 2000.0;
	
	bIsOverlapAgroSphere = false;
	bIsOverlapCombatSphere = false;

	BossCombatState = EBossCombatState::EBCS_Unoccupied;

	SwingDamage = 20.0f;
	SoulSiphonDamage = 80.0f;
	UltimateSwingDamage = 150.0f;

	// Ability cooldown time
	SoulSiphonCooldownTime = 15.0f;
	UltimateCooldownTime = 30.0f;

	bCanSoulSiphon = true;
	bCanUltimate = true;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	Target = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss::WeaponCollisionBoxBeginOverlap);

	if (GetBehaviorTree())
	{
		EnemyController->RunBehaviorTree(GetBehaviorTree());
	}
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceAgroSphere();
}

void ABoss::CombatSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		bIsOverlapCombatSphere = true;
	}
}

void ABoss::CombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		bIsOverlapCombatSphere = false;
	}
}

void ABoss::WeaponCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
		if (Character)
		{
			UGameplayStatics::ApplyDamage(
				Character,
				SwingDamage,
				EnemyController,
				this,
				UDamageType::StaticClass());

			if (SwingHitParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					SwingHitParticle,
					SweepResult.Location);
			}

			const FVector LaunchDir{ GetActorRotation().Vector() };
			const float LaunchDist = 300.0f;
			Character->LaunchCharacter(LaunchDir*LaunchDist, true, true);
		}
	}
}

float ABoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ShowHealthBar();

	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;

		if (bIsDead) return 0;

		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void ABoss::TraceAgroSphere()
{
	const FVector Center{ GetActorLocation() };
	float Radius = 2500.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Center,
		Center,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true);

	if (HitResult.GetActor())
	{
		auto Player = Cast <ACharacterBase>(HitResult.GetActor());
		if (Player)
		{
			bIsOverlapAgroSphere = true;
		}
	}
}

void ABoss::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);

	GetWorldTimerManager().SetTimer(
		HealthBarTimer,
		this,
		&AEnemy::HideHealthBar,
		HealthBarDisplayTime);

}

void ABoss::Swing()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	SetBossCombatState(EBossCombatState::EBCS_Swing);
}

void ABoss::ActivateWeaponCollision()
{
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ABoss::DeactivateWeaponCollision()
{
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ABoss::SoulSiphon()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	if (!bCanSoulSiphon) return;

	SetBossCombatState(EBossCombatState::EBCS_SoulSiphon);

	const FVector Center{ GetActorLocation() + GetActorForwardVector() * 400.0f };
	float Radius = 300.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Center,
		Center,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true);

	if (HitResult.GetActor())
	{
		ACharacterBase* Character = Cast<ACharacterBase>(HitResult.GetActor());
		if (Character)
		{
			UGameplayStatics::ApplyDamage(
				Target,
				SoulSiphonDamage,
				EnemyController,
				this,
				UDamageType::StaticClass());
		}
	}

	// Cooldown
	bCanSoulSiphon = false;
	GetWorldTimerManager().SetTimer(
		SoulSiphonTimer, 
		this, 
		&ABoss::ResetSoulSiphonCooldown, 
		SoulSiphonCooldownTime);
}

void ABoss::SpeedBurst()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	SetBossCombatState(EBossCombatState::EBCS_SpeedBurst);

}

void ABoss::Ultimate()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	if (!bCanUltimate) return;

	SetBossCombatState(EBossCombatState::EBCS_Ultimate);

	// Cooldown
	bCanUltimate = false;
	GetWorldTimerManager().SetTimer(
		UltimateCooldownTimer,
		this,
		&ABoss::ResetUltimateCooldown,
		UltimateCooldownTime);
}

void ABoss::ResetSoulSiphonCooldown()
{
	bCanSoulSiphon = true;
} 

void ABoss::ResetUltimateCooldown()
{
	bCanUltimate = true;
}