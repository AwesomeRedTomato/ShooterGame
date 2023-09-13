// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Drone.generated.h"

UCLASS()
class SHOOTERGAME_API ADrone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* DroneMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone", meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* DestroyParticle;

	/** ���� ����(Combat Sphere Radius) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone", meta = (AllowPrivateAccess = "true"))
	float AttackRange;

	/** ���� Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone", meta = (AllowPrivateAccess = "true"))
	class AEnemy* Enemy;
	
	/** ��� ���� �ð� */
	FTimerHandle DroneTimer;
	float DroneTime;

	/** ���� �� */
	FTimerHandle BeamTimer;
	float BeamFrequencyTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone", meta = (AllowPrivateAccess = "true"))
	bool bIsDeployed;

public:
	FORCEINLINE USkeletalMeshComponent* GetDroneMesh() const { return DroneMesh; }
	
	FORCEINLINE void SetIsDeployed(bool IsDeployed) { bIsDeployed = IsDeployed; }

	void BeamAttack();
	void Destroy();
};
