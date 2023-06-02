// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Sound/SoundCue.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged		UMETA(DisplayName = "Damaged"),
	EIR_Common		UMETA(DisplayName = "Common"),
	EIR_UnCommon	UMETA(DisplayName = "UnCommon"),
	EIR_Rare		UMETA(DisplayName = "Rare"),
	EIR_Legendary	UMETA(DisplayName = "Legendary"),

	EIR_MAX			UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup			UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping	UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp		UMETA(DisplayName = "PickedUp"),
	EIS_Equipped		UMETA(DisplayName = "Equipped"),
	EIS_Falling			UMETA(DisplayName = "Falling"),

	EIS_MAX				UMETA(DisplayName = "DefaultMAX"),
};

UCLASS()
class SHOOTERGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** AreaSphere Overlap �� ȣ�� */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	/** AreaSphere Overlap ���� �� ȣ�� */
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/** �ʱ� ���� Custom Depth Enabled */
	virtual void InitializeCustomDepth();

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** ������ ���� ���� */
	virtual void SetItemState(EItemState State);

private:
	/** ���� Ʈ���̽� �浹 ���� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	/** ������ ���̷�Ż �޽� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/** ������ ���� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	/** ĳ���� overlap ���� ��ü */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	/** �������� ȹ�� ȿ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* PickupSound;

	/** �������� ���� ȿ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;

	/** ������ ���ε� �� ������ �̸� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/** ������ ���ε� �� ������ ���� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	/** ������ ���ε� �� ������ ��͵� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/** ������ ���ε� �� �������� ���(�� ����) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/** ������ ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	/** ������ ���� Ÿ�̸� */
	FTimerHandle ThrowItemTimer;
	float ThrowItemTime;

	/** ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bFalling;

	/** ��Ÿ�� ������ ������ Material Instance Index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;

	/** ��Ÿ�� ������ ������ Material Instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	
	/** Dynamic Material Instance���� ����� Material Instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;

	/** Custom Depth ���� ���� ���� */
	bool bCanChangeCustomDepth;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	/** Ȱ�� ���(�� ����) */
	void SetActiveStars();

	/** ���¿� ���� ������ �Ӽ� ���� */
	virtual void SetItemProperties(EItemState State);

	/** ������ ���� ȿ�� */
	void ThrowItem();
	void StopFalling();

	/** ȹ�� ȿ���� ��� */
	void PlayPickupSound();

	/** Render Custom Depth ture/false */
	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

	void EnableGlowMaterial();
	void DisableGlowMaterial();
};