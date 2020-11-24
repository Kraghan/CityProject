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
		CityBuilding(int x, int y);

		FVector2D Location;
		int Size = 1;
		int OffsetIndex = -1;
		int RotationIndex = -1;
		int ScaleIndex = -1;
		int MeshIndex = 0;
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

		void CreateMeshes(ACityGenerator* City);
	};
	
public:	
	// Sets default values for this actor's properties
	ACityGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="City setup")
	FVector2D CitySize = FVector2D(5,5);

	TArray<CityBlock> CityBlocks;

	UPROPERTY(VisibleInstanceOnly, Category="City setup")
	UInstancedStaticMeshComponent* FloorMeshComponent = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="City setup")
	TArray<UInstancedStaticMeshComponent*> BuildingMeshesComponent;

	UPROPERTY(VisibleInstanceOnly, Category="City setup|Optimisation")
	bool bIsDistanceCulled = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Optimisation")
	float DistanceCulling = 9000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Optimisation")
	float InstancedMeshComponentsStartCullDistance = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Optimisation")
	float InstancedMeshComponentsEndCullDistance = 4500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Building")
	TArray<FVector> BuildingOffsets;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Building")
	TArray<FVector> BuildingScales;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Building")
	TArray<FRotator> BuildingRotations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="City setup|Building")
	int BuildingMaxBlockOccupation = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Building")
	TArray<UStaticMesh*> BuildingMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Block")
	TArray<UMaterialInterface*> FloorMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category="City setup|Block")
	float BaseBlockSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Block")
	bool KeepSquaredBlock = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Block", meta = (ToolTip = "In block unit") )
	int MaxBlockSize = 1;

	UPROPERTY(EditAnywhere, Category="City setup|Block")
	UStaticMesh* FloorMesh = nullptr;

	UFUNCTION(CallInEditor, BlueprintCallable, Category="City setup")
	void Randomize();

	UFUNCTION(BlueprintNativeEvent, Category="City setup")
	void ClearCity();

	void GenerateBlocks();

	UFUNCTION(BlueprintNativeEvent, Category="City setup")
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
	void OnPostBuildingCreated(int MeshTypeIndex);
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
