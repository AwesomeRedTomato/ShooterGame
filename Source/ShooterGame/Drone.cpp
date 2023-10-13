// Fill out your copyright notice in the Description page of Project Settings.


#include "Drone.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "CharacterBase.h"

// Sets default values
ADrone::ADrone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DroneSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DroneSceneComponent"));
	DroneSceneComponent->SetupAttachment(GetRootComponent());

	DroneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	DroneMesh->SetupAttachment(DroneSceneComponent);

	Damage = 5.0f;
	AttackRange = 800.0f;
	bIsDeployed = false;
	bIsAttacking = false;

	DroneTime = 7.0f;
	BeamFrequencyTime = 0.5f;
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDeployed)
	{
		// 단일 구 트레이싱: 범위 내 Enemy 존재 시 일정 간격으로 공격
		const FVector Center{ GetActorLocation() };
		const FRotator Orientation{ GetActorRotation() };
		float Radius = AttackRange;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
		ObjectTypes.Add(Pawn);

		// 플레이어는 공격 대상에서 제외
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
			Target = Cast<AEnemy>(HitResult.Actor);
			
			//FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Target->GetActorLocation());
			//SetActorRotation(Direction.Rotation());
		}
	}
}

void ADrone::SetDroneTime()
{
	bIsDeployed = true;

	GetWorldTimerManager().SetTimer(
		DroneTimer,
		this,
		&ADrone::Destroy,
		DroneTime);

	GetWorldTimerManager().SetTimer(
		BeamTimer,
		this,
		&ADrone::BeamAttack,
		BeamFrequencyTime,
		true);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnParticle, GetActorLocation());
}

void ADrone::BeamAttack()
{
	if (Target)
	{
		bIsAttacking = true;

		const USkeletalMeshSocket* Socket = GetDroneMesh()->GetSocketByName("eye_rays");
		FTransform SocketTransform = Socket->GetSocketTransform(GetDroneMesh());

		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BeamParticle,
			SocketTransform);

		Beam->SetBeamEndPoint(0, Target->GetActorLocation());

		// 데미지
		UGameplayStatics::ApplyDamage(
			Target,
			Damage,
			nullptr,
			this,
			UDamageType::StaticClass());

	}
}

void ADrone::Destroy()
{
	bIsAttacking = false;

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		DestroyParticle,
		GetActorLocation());

	Super::Destroy();

	ACharacterBase* Character = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Character)
	{
		Character->SetDrone(nullptr);
	}
}
