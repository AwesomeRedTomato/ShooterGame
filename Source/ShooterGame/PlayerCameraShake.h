// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "PlayerCameraShake.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UPlayerCameraShake : public UMatineeCameraShake
{
	GENERATED_BODY()
	
public:
	UPlayerCameraShake();
};
