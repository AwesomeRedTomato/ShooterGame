// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "BulletHitInterface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyController.h"
#include "Components/SphereComponent.h"
#include "Enemy.generated.h"

UCLASS()
class SHOOTERGAME_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void AgroSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** �Ѿ� �浹 ��ƼŬ */
	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	/** �������� �Ծ��� �� ȣ�� */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	/** ��� */
	void Die();
	/** ��� �� �Ҹ� */
	void Destroy();

	/** �ǰ� ��Ÿ�� ��� */
	void PlayHitMontage(FName Section, float PlayRate);

	UFUNCTION(BlueprintCallable)
	void PlayHipFireMontage();

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);


private:
	/** �� �ν� �浹 �� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AgroSphere;

	/** �ǰ� ��ƼŬ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/** �ǰ� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;

	/** �ǰ� ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	/** ���� ü�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	/** �ִ� ü�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	/** �Ӹ� bone �̸� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

	/** ü�¹� ǥ�� �ð� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;
	FTimerHandle HealthBarTimer;

	/** ��� �� ���� �Ҹ������ �� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DestroyTime;
	FTimerHandle DestroyTimer;

	/** �ǰ� �ִϸ��̼� Ÿ�̸� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitReactTime;
	FTimerHandle HitReactTimer;

	/** �ǰ� �� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	bool bStunned;

	/** ���� Ȯ���� �ǰ� �� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	float StunChance;

	/** ���� ���� �ȿ� �ִ��� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	bool bFireFinish;

	/** �Ѿ� ���� Ʈ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	UParticleSystem* BeamParticles;

	/** �߻� �ִϸ��̼� ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	bool bInSight;

	AEnemyController* EnemyController;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;


public:
	// Get
	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

};
