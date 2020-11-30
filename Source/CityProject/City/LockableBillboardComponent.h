// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LockableBillboardComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYPROJECT_API ULockableBillboardComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULockableBillboardComponent();

	UPROPERTY(EditAnywhere, Category=Custom)
	bool LockRoll = true;

	UPROPERTY(EditAnywhere, Category=Custom)
	bool LockPitch = false;

	UPROPERTY(EditAnywhere, Category=Custom)
	bool LockYaw = false;

public:
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
