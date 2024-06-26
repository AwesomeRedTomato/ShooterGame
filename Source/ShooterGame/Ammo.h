// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */

UCLASS()
class SHOOTERGAME_API AAmmo : public AItemBase
{
	GENERATED_BODY()

public:
	AAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetItemState(EItemState State) override;

private:
	/** Mesh for the ammo pickup */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AmmoCollisionSphere;

	int32 HighlightColor;

public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

};
