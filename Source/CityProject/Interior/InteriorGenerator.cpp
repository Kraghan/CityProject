// Fill out your copyright notice in the Description page of Project Settings.


#include "InteriorGenerator.h"


// Sets default values
AInteriorGenerator::AInteriorGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInteriorGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteriorGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

