// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 200.0f;
	MaxHealth = 200.0f;
	HealthBarDisplayTime = 2.0f;

	DestroyTime = 5.0f;
	HitReactTime = 0.5f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility,
		ECollisionResponse::ECR_Block);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), 
			ImpactParticles, 
			HitResult.Location, 
			FRotator(0.0f), 
			true);
	}

	ShowHealthBar();
	PlayHitMontage(FName("HitreactFront"), 0.5f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	
	GetWorldTimerManager().SetTimer(
		HealthBarTimer, 
		this, 
		&AEnemy::HideHealthBar, 
		HealthBarDisplayTime);
}

void AEnemy::Die()
{
	HideHealthBar();

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AEnemy::Destroy,
		DestroyTime);
}

void AEnemy::Destroy()
{
	AActor::Destroy();
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(HitMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, HitMontage);
	}
}
