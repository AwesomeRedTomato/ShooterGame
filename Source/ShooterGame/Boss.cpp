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
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetMesh(), FName(TEXT("HammerCenter")));
	
	bIsTarget = false;

	MaxHealth = 2000.0f;
	Health = 2000.0;
	
	bIsOverlapCombatSphere = false;
	bIsOverlapAgroSphere = false;

	BossCombatState = EBossCombatState::EBCS_Unoccupied;

	SwingDamage = 20.0f;
	SoulSiphonDamage = 120.0f;

	BaseMovementSpeed = 500.0f;
	DashSpeed = 1500.0f;
	DashDistance = 6000.0f;

}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	AgroSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AgroSphereBeginOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AgroSphereEndOverlap);

	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss::WeaponCollisionBoxBeginOverlap);

}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FocusOnTarget(DeltaTime);
}

void ABoss::AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	if (Target == nullptr)
	{
		bIsTarget = true;
		bIsOverlapAgroSphere = true;
		Target = Cast<ACharacterBase>(OtherActor);
		EnemyController->GetBlackboardComponent()->SetValueAsObject("Target", Target);
	}
}

void ABoss::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Target)
	{
		bIsTarget = true;
		bIsOverlapAgroSphere = false;
		Target = nullptr;
		EnemyController->GetBlackboardComponent()->SetValueAsObject("Target", Target);
	}
}

void ABoss::CombatSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (Target)
		{
			bIsOverlapCombatSphere = true;
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsOverlapCombatSphere"), bIsOverlapCombatSphere);
		}
	}
}

void ABoss::CombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (Target)
		{
			bIsOverlapCombatSphere = false;
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsOverlapCombatSphere"), bIsOverlapCombatSphere);
		}
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
			const float LaunchDist = 3000.0f;
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

void ABoss::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);

	GetWorldTimerManager().SetTimer(
		HealthBarTimer,
		this,
		&AEnemy::HideHealthBar,
		HealthBarDisplayTime);

}

void ABoss::FocusOnTarget(float DeltaTime)
{
	if (Target)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		FRotator Rotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, 4.0f);
		SetActorRotation(Rotation);
	}
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
	SetBossCombatState(EBossCombatState::EBCS_SoulSiphon);

	const FVector Start{ GetActorLocation() + GetActorForwardVector() * 500.0f };
	const FVector End{ Start };
	float Radius = 300.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true);

	if (HitResult.GetActor())
	{
		ACharacterBase* Character = Cast<ACharacterBase>(HitResult.Actor);
		if (Character)
		{
			UGameplayStatics::ApplyDamage(
				Character, 
				SoulSiphonDamage, 
				EnemyController, 
				this,	
				UDamageType::StaticClass());
		}
	}
}

void ABoss::SpeedBurst()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	SetBossCombatState(EBossCombatState::EBCS_SpeedBurst);

	//const FVector ForwardDir{ this->GetActorRotation().Vector() };
	//LaunchCharacter(ForwardDir * DashDistance, true, true);
}
