// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

// Sets default values
ABossEnemy::ABossEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());

	SwingDamage = 20.0f;
	bIsOverlapCombatSphere = false;
}

// Called when the game starts or when spawned
void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::AgroSphereBeginOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::AgroSphereEndOverlap);

	CombatSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::CombatSphereBeginOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::CombatSphereEndOverlap);

	BossController = Cast<ABossController>(GetController());

	if (BossController)
	{
		BossController->RunBehaviorTree(BehaviorTree);
	}
}

void ABossEnemy::AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		Target = Cast<ACharacterBase>(OtherActor);
		if (Target)
		{
			BossController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Target);
		}
	}
}

void ABossEnemy::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (Target)
		{
			BossController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);
			Target = nullptr;
		}
	}
}

void ABossEnemy::CombatSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (Target)
		{
			bIsOverlapCombatSphere = true;
			BossController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsOverlapCombatSphere"), bIsOverlapCombatSphere);
		}
	}
}

void ABossEnemy::CombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (Target)
		{
			bIsOverlapCombatSphere = false;
			BossController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsOverlapCombatSphere"), bIsOverlapCombatSphere);
		}
	}
}

// Called every frame
void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABossEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossEnemy::Swing1()
{
	UE_LOG(LogTemp, Warning, TEXT("Swing1()"));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(SwingMontage);
		AnimInstance->Montage_JumpToSection(FName(TEXT("Swing1")));
	}
}

void ABossEnemy::Swing2()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(SwingMontage);
		AnimInstance->Montage_JumpToSection(FName(TEXT("Swing2")));
	}
}

