// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityGenerator.generated.h"

USTRUCT()
struct FCityBuilding
{
	GENERATED_BODY()

	int RotationIndex = 0;
	int ScaleIndex = 0;
	int MeshIndex = 0;

	TArray<int> HoloRooftopMaterialsIndex;
	TArray<int> HoloWallMaterialsIndex;
};

USTRUCT()
struct FCityBlock
{
	GENERATED_BODY()

	FCityBlock();
	
	FCityBlock(FVector2D BlockSize, FVector2D BlockLocation);
	
	FVector2D BlockSize = FVector2D(1,1);
	FVector2D BlockLocation;
	int FloorMaterialIndex = 0;

	TArray<FCityBuilding> Buildings;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FloorComponentRef;
};

UCLASS(Blueprintable)
class GOMORRAH_API ACityGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACityGenerator();

	UPROPERTY(EditAnywhere, Category="City setup")
	FVector2D CitySize = FVector2D(5,5);

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category="City setup")
	TArray<FCityBlock> CityBlocks;

	UPROPERTY(VisibleInstanceOnly, Category="City setup")
	UInstancedStaticMeshComponent* MeshComponent = nullptr; 

	UPROPERTY(EditAnywhere, Category="City setup|Building")
	TArray<FVector> BuildingScales;

	UPROPERTY(EditAnywhere, Category="City setup|Building")
	TArray<FRotator> BuildingRotations;

	UPROPERTY(EditAnywhere, Category="City setup|Building")
	TArray<UStaticMesh*> BuildingMeshes;

	UPROPERTY(EditAnywhere, Category="City setup|Building|Holo")
	TArray<UMaterialInterface*> HoloWallMaterials;

	UPROPERTY(EditAnywhere, Category="City setup|Building|Holo")
	TArray<UMaterialInterface*> HoloRooftopMaterials;

	UPROPERTY(EditAnywhere, Category="City setup|Block")
	TArray<UMaterialInterface*> FloorMaterials;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category="City setup|Block")
	float BaseBlockSize = 50;

	UPROPERTY(EditAnywhere, Category="City setup|Block")
	bool KeepSquaredBlock = true;

	UPROPERTY(EditAnywhere, Category="City setup|Block", meta = (ToolTip = "In block unit") )
	int MaxBlockSize = 1;

	UPROPERTY(EditAnywhere, Category="City setup|Block")
	UStaticMesh* FloorMesh = nullptr;

	UFUNCTION(CallInEditor, BlueprintCallable, Category="City setup")
	void Randomize();

	void ClearCity();

	void GenerateBlocks();

	void CreateMeshes();

	int GetNumberOfBlocksMax() const
	{
		return CitySize.X * CitySize.Y;
	}
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
