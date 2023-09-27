// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.h"
#include "BossController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BossEnemy.generated.h"

UENUM(BlueprintType)
enum class EBossCombatState : uint8
{
	EBCS_Unoccupied				UMETA(DisplayName = "Unoccupied"),
	EBCS_Swing					UMETA(DisplayName = "Swing"),
	EBCS_SoulSteal				UMETA(DisplayName = "SoulSteal"),

	EBCS_MAX					UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class SHOOTERGAME_API ABossEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossEnemy();

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

	UFUNCTION()
	void AgroSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

	UFUNCTION()
	void CombatSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	void CombatSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	ACharacterBase* Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AgroSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatSphere;

	ABossController* BossController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EBossCombatState BossCombatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Swing", meta = (AllowPrivateAccess = "true"))
	float SwingDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Swing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwingMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	bool bIsOverlapCombatSphere;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void Swing1();
	UFUNCTION(BlueprintCallable)
	void Swing2();

public:
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE EBossCombatState GetBossCombatState() const { return BossCombatState; }
	
	FORCEINLINE void SetBossCombatState(EBossCombatState State) { BossCombatState = State; }
};
