// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum class EBossCombatState : uint8
{
	EBCS_Unoccupied				UMETA(DisplayName = "Unoccupied"),
	EBCS_Swing					UMETA(DisplayName = "Swing"),
	EBCS_SoulSiphon				UMETA(DisplayName = "SoulSiphon"),
	EBCS_SpeedBurst				UMETA(DisplayName = "SpeedBurst"),
	EBCS_Ultimate				UMETA(DisplayName = "Ultimate"),

	EBCS_MAX					UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SHOOTERGAME_API ABoss : public AEnemy
{
	GENERATED_BODY()
	
public:
	ABoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ACharacterBase* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	bool bIsOverlapAgroSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	bool bIsOverlapCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossCombatState BossCombatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WeaponCollisionBox;

	/** 기본 공격 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float SwingDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SwingHitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float SoulSiphonDamage;

public:
	virtual void CombatSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void CombatSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	UFUNCTION()
	void WeaponCollisionBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		AController* EventInstigator, 
		AActor* DamageCauser) override;

	void TraceAgroSphere();

	/** 체력 바 HUD 활성화 */
	virtual void ShowHealthBar_Implementation() override;

	/** 기본 공격 */
	UFUNCTION(BlueprintCallable)
	void Swing();
	
	/** 공격 시 무기 충돌 활성화 */
	UFUNCTION(BlueprintCallable)
	void ActivateWeaponCollision();
	
	/** 무기 충돌 비활성화 */
	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void SoulSiphon();

	UFUNCTION(BlueprintCallable)
	void SpeedBurst();
	
public:
	FORCEINLINE EBossCombatState GetBossCombatState() const { return BossCombatState; }
	FORCEINLINE void SetBossCombatState(EBossCombatState State)  { BossCombatState = State; }
};
