// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaBomb.h"

// Sets default values
AManaBomb::AManaBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AManaBomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AManaBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AManaBomb::Explode()
{
}