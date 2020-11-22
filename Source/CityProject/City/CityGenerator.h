// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityGenerator.generated.h"

UCLASS(Blueprintable)
class CITYPROJECT_API ACityGenerator : public AActor
{
	GENERATED_BODY()

	struct CityBuilding
	{
		int RotationIndex = 0;
		int ScaleIndex = 0;
		int MeshIndex = 0;

		TArray<int> HoloRooftopMaterialsIndex;
		TArray<int> HoloWallMaterialsIndex;
	};

	struct CityBlock
	{
		CityBlock();
	
		CityBlock(FVector2D BlockSize, FVector2D BlockLocation, ACityGenerator* City);
	
		FVector2D BlockSize = FVector2D(1,1);
		FVector2D BlockLocation;
		int FloorMaterialIndex = 0;

		TArray<CityBuilding> Buildings;

		UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* FloorComponentRef;
	};
	
public:	
	// Sets default values for this actor's properties
	ACityGenerator();

	UPROPERTY(EditAnywhere, Category="City setup")
	FVector2D CitySize = FVector2D(5,5);

	TArray<CityBlock> CityBlocks;

	UPROPERTY(VisibleInstanceOnly, Category="City setup")
	UInstancedStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category="City setup")
	TArray<UInstancedStaticMeshComponent*> BuildingMeshesComponent;

	UPROPERTY(VisibleInstanceOnly, Category="City setup|Optimisation")
	bool bIsDistanceCulled = false;

	UPROPERTY(EditAnywhere, Category="City setup|Optimisation")
	float DistanceCulling = 9000;

	UPROPERTY(EditAnywhere, Category="City setup|Optimisation")
	float InstancedMeshComponentsStartCullDistance = 4000;

	UPROPERTY(EditAnywhere, Category="City setup|Optimisation")
	float InstancedMeshComponentsEndCullDistance = 4500;

	UPROPERTY(EditAnywhere, Category="City setup|Building")
	TArray<FVector> BuildingOffsets;
	
	UPROPERTY(EditAnywhere, Category="City setup|Building")
	TArray<FVector> BuildingScales;

	UPROPERTY(EditAnywhere, Category="City setup|Building")
	TArray<FRotator> BuildingRotations;

	UPROPERTY(EditAnywhere, Category="City setup|Building")
	int BuildingMaxBlockOccupation = 2;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="City setup|Building")
	TArray<UStaticMesh*> BuildingMeshes;

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

	void ShowCity();
	void HideCity();

	FVector GetCenterCityPoint() const;

	UPROPERTY(EditAnywhere, Category="City setup")
	bool CullInEditor = true;
	
	UFUNCTION(Exec, BlueprintCallable, Category="City setup")
	void CityToggleEditorCulling();

	UFUNCTION(BlueprintImplementableEvent, Category="City setup")
	void OnPostBuildingCreated(int MeshTypeIndex, int MeshInstanceIndex);
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
