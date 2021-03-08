// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InteriorGenerator.generated.h"

UCLASS()
class CITYPROJECT_API AInteriorGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteriorGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
