// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss.h"
#include "CharacterBase.h"

ABoss::ABoss()
{
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	MaxHealth = 2000.0f;
	Health = 2000.0;
	
	bIsOverlapCombatSphere = false;
	bIsOverlapAgroSphere = false;

	SwingDamage = 20.0f;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss::AgroSphereBeginOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABoss::AgroSphereEndOverlap);

}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoss::AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	if (Target == nullptr)
	{
		bIsOverlapAgroSphere = true;
		Target = Cast<ACharacterBase>(OtherActor);
		EnemyController->GetBlackboardComponent()->SetValueAsObject("Target", Target);
	}
}

void ABoss::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Target)
	{
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

void ABoss::Swing1()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	SetBossCombatState(EBossCombatState::EBCS_Swing);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Swing1Montage);
		AnimInstance->Montage_JumpToSection(FName(TEXT("Swing1")));
	}
}

void ABoss::Swing2()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	SetBossCombatState(EBossCombatState::EBCS_Swing);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Swing2Montage);
		AnimInstance->Montage_JumpToSection(FName(TEXT("Swing2")));
	}
}

void ABoss::SoulSteal()
{
	if (BossCombatState != EBossCombatState::EBCS_Unoccupied) return;
	SetBossCombatState(EBossCombatState::EBCS_SoulSteal);
}

void ABoss::SpeedBurst()
{
	const FVector DashPoint{ Target->GetActorLocation() };


}