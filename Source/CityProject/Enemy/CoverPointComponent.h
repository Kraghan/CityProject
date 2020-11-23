// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Gomorrah/Player/GomorrahBaseCharacter.h"

#include "CoverPointComponent.generated.h"

USTRUCT(BlueprintType)
struct FCoverageData
{
	GENERATED_BODY()

	FCoverageData();

	FCoverageData(FVector Direction, float Angle);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Cover Data")
	float CoverageAngle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Cover Data")
	FVector CoverageDirection;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOMORRAH_API UCoverPointComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCoverPointComponent();

	UFUNCTION(BlueprintCallable, Category="Cover")
	bool CoverIsFree() const;
	
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category="Cover")
	AGomorrahBaseCharacter* CharacterInCover = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Cover")
	TArray<FCoverageData> CoverageDatas;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
