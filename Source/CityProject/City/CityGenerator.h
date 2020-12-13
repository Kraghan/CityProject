// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityGenerator.generated.h"

USTRUCT(BlueprintType)
struct FCityBuilding
{
	GENERATED_BODY()
	FCityBuilding();
	FCityBuilding(int x, int y);

	UPROPERTY(BlueprintReadWrite)
	FVector2D Location;
	
	UPROPERTY(BlueprintReadWrite)
	int Size = 1;

	UPROPERTY(BlueprintReadWrite)
	int OffsetIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	int RotationIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	int ScaleIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	int MeshIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	int MeshInstanceID = -1;
};

USTRUCT(BlueprintType)
struct FMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AvailableForEverySize = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!AvailableForEverySize"))
	int MinSize = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!AvailableForEverySize"))
	int MaxSize = 1;
};

UCLASS(Blueprintable)
class CITYPROJECT_API ACityGenerator : public AActor
{
	GENERATED_BODY()

	struct CityBlock
	{
		CityBlock();
	
		CityBlock(FVector2D BlockSize, FVector2D BlockLocation, ACityGenerator* City);
	
		FVector2D BlockSize = FVector2D(1,1);
		FVector2D BlockLocation;
		int FloorMaterialIndex = 0;

		TArray<FCityBuilding> Buildings;

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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="City setup")
	TArray<int> NumberOfBuildingInstancePerBuildingType;

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
	TMap<int, float> BuildingSizeProbabilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Building")
	TArray<FMeshData> BuildingMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Block")
	TArray<UMaterialInterface*> FloorMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category="City setup|Block")
	float BaseBlockSize = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="City setup|Block", meta = (ToolTip = "In block unit") )
	TMap<int, float> BlockSizeProbability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "City setup|Block")
	float EmptyLotPercentile = 0.02f;

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
	void OnPostBuildingCreated(UPARAM(ref) FCityBuilding& BuildingRef);

	UFUNCTION(BlueprintCallable, Category="City setup|Building")
	int GetBuildingSizeWithProba(int MaxSize);

	UFUNCTION(BlueprintCallable, Category="City setup|Building")
	int GetMaxBuildingSize();

	UFUNCTION(BlueprintCallable, Category="City setup|Block")
    int GetBlockSizeWithProba(int MaxSize);

	UFUNCTION(BlueprintCallable, Category="City setup|Block")
    int GetMaxBlockSize();

	UFUNCTION(BlueprintCallable, Category="City setup|Building")
	void GetFittingBuildingsIndexes(TArray<int>& BuildingIndexes, int MaxSize);
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
