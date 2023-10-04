// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum class EBossCombatState : uint8
{
	EBCS_Unoccupied				UMETA(DisplayName = "Unoccupied"),
	EBCS_Swing					UMETA(DisplayName = "Swing"),
	EBCS_SoulSteal				UMETA(DisplayName = "SoulSteal"),
	EBCS_SpeedBurst				UMETA(DisplayName = "SpeedBurst"),

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossCombatState BossCombatState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	bool bIsOverlapCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	bool bIsOverlapAgroSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Swing", meta = (AllowPrivateAccess = "true"))
	float SwingDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Swing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Swing1Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Swing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Swing2Montage;
	
	// TODO: Ability Cooldown


public:
	UFUNCTION()
	void AgroSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void AgroSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

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

	UFUNCTION(BlueprintCallable)
	void Swing1();
	UFUNCTION(BlueprintCallable)
	void Swing2();

	UFUNCTION(BlueprintCallable)
	void SoulSteal();

	UFUNCTION(BlueprintCallable)
	void SpeedBurst();
	
public:
	FORCEINLINE EBossCombatState GetBossCombatState() const { return BossCombatState; }
	FORCEINLINE void SetBossCombatState(EBossCombatState State) { BossCombatState = State; }
	
};
