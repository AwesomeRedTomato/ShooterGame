// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"
#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ADrone::ADrone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DroneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	DroneMesh->SetupAttachment(GetRootComponent());

	Damage = 5.0f;
	AttackRange = 800.0f;
	bIsDeployed = false;

	DroneTime = 7.0f;
	BeamFrequencyTime = 0.5f;
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(
		DroneTimer,
		this,
		&ADrone::Destroy,
		DroneTime);

}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ���� �� Ʈ���̽�: ���� �� Enemy ���� �� ���� �������� ����
	const FVector Center{ GetActorLocation() };
	const FRotator Orientation{ GetActorRotation() };
	float Radius = AttackRange;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	// �÷��̾�� ���� ��󿡼� ����
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	FHitResult HitResult;

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Center,
		Center,
		Radius,
		ObjectTypes,
		false,
		ActorToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true);

	if (HitResult.bBlockingHit)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy)
		{
			// ����
			GetWorldTimerManager().SetTimer(
				BeamTimer,
				this,
				&ADrone::BeamAttack,
				BeamFrequencyTime,
				true,
				0.8f);
		}
	}
}

void ADrone::BeamAttack()
{
	// ������
	UGameplayStatics::ApplyDamage(
		Enemy,
		Damage,
		Enemy->GetController(),
		this,
		UDamageType::StaticClass());

	// Beam ��ƼŬ
	const USkeletalMeshSocket* SocketMesh = GetDroneMesh()->GetSocketByName(FName(TEXT("eye_rays")));
	FTransform BeamTransform = SocketMesh->GetSocketTransform(GetDroneMesh());

	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		BeamParticle,
		BeamTransform);

	if (Beam)
	{
		Beam->SetVectorParameter(FName("Target"), BeamTransform.GetLocation());
	}
}

void ADrone::Destroy()
{
	SetIsDeployed(false);

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		DestroyParticle,
		GetActorLocation());

	Super::Destroy();
}
