// Fill out your copyright notice in the Description page of Project Settings.


#include "CityGenerator.h"

FCityBuilding::FCityBuilding()
	: FCityBuilding(0,0)
{
}

FCityBuilding::FCityBuilding(int x, int y)
	: Location(FVector2D(x,y))
{
}

ACityGenerator::CityBlock::CityBlock()
	:CityBlock(FVector2D(1,1), FVector2D(), nullptr)
{
}

ACityGenerator::CityBlock::CityBlock(FVector2D BlockSize, FVector2D BlockLocation, ACityGenerator* City)
	: BlockSize(BlockSize)
	, BlockLocation(BlockLocation)
{
	if(City == nullptr)
	{
		return;
	}
	
	// Initialize an array of bool to check occupied areas
	TArray<TArray<bool>> CityBlockRepresentation;
	CityBlockRepresentation.AddDefaulted(BlockSize.X);
	for(int i = 0; i < BlockSize.X; ++i)
	{
		CityBlockRepresentation[i].AddDefaulted(BlockSize.Y);
	}

	int BuildingMaxSize = City->GetMaxBuildingSize();
	
	for(int x = 0; x < BlockSize.X; ++x)
	{
		for(int y = 0; y < BlockSize.Y; ++y)
		{
			// Skip if occupied or if empty lot
			if(CityBlockRepresentation[x][y] || FMath::RandRange(0.f, 1.f) <= City->EmptyLotPercentile)
			{
				continue;
			}
			FCityBuilding newBuilding = FCityBuilding(x,y);

			// Location
			if(City->BuildingOffsets.Num() != 0)
			{				
				newBuilding.OffsetIndex = FMath::RandRange(0, City->BuildingOffsets.Num() - 1);
			}

			// Rotation
			if(City->BuildingRotations.Num() != 0)
			{				
				newBuilding.RotationIndex = FMath::RandRange(0, City->BuildingRotations.Num() - 1);
            }

			// Scales
			if(City->BuildingScales.Num() != 0)
			{				
				newBuilding.ScaleIndex = FMath::RandRange(0, City->BuildingScales.Num() - 1);
            }
			int maxSize = BuildingMaxSize;

			for(int i = 0; i < maxSize; ++i)
			{
				if(x + i >= BlockSize.X - 1)
				{
					maxSize = i;
				}
				for(int j = 0; j < maxSize; ++j)
				{
					if(y + j >= BlockSize.Y - 1 || CityBlockRepresentation[x + i][y + j])
					{
						maxSize = j;
						break;
					}
				}	
			}

			newBuilding.Size = City->GetBuildingSizeWithProba(maxSize);

			// Mesh
			if(City->BuildingMeshes.Num() != 0)
			{
				TArray<int> buildingIndexes;
				City->GetFittingBuildingsIndexes(buildingIndexes, newBuilding.Size);
				if(buildingIndexes.Num() != 0)
				{
					
					int randIndex = FMath::RandRange(0, buildingIndexes.Num() - 1);
					newBuilding.MeshIndex = buildingIndexes[randIndex];
					newBuilding.MeshInstanceID = City->NumberOfBuildingInstancePerBuildingType[newBuilding.MeshIndex];
					City->NumberOfBuildingInstancePerBuildingType[newBuilding.MeshIndex] ++;
				}
				else
				{
					newBuilding.MeshIndex = -1;
					UE_LOG(LogActor, Warning, TEXT("No building found for a size of %d"), newBuilding.Size);
					continue;
				}
            }
			
			// Add Holograms
			City->OnPostBuildingCreated(newBuilding);

			Buildings.Add(newBuilding);

			// Fill occupation representation
			for(int i = 0; i < newBuilding.Size; ++i)
			{
				for(int j = 0; j < newBuilding.Size; ++j)
				{
					CityBlockRepresentation[x + i][y + j] = true;
				}
			}
		}	
	}
}

void ACityGenerator::CityBlock::CreateMeshes(ACityGenerator* City)
{
	for(int i = 0; i < Buildings.Num(); ++i)
	{
		FCityBuilding& building = Buildings[i];

		// Add mesh
		int meshIndex = building.MeshIndex;

		if(meshIndex == -1)
		{
			continue;
		}
		
		// Calculate building location
		FVector2D blockCoordinates = BlockLocation + building.Location;
		FVector location = FVector(blockCoordinates.X * City->BaseBlockSize, blockCoordinates.Y * City->BaseBlockSize, 0);
			
		if(building.OffsetIndex != -1)
		{
			location += City->BuildingOffsets[building.OffsetIndex];	
		}

		// Calculate building rotation
		FRotator rotation = FRotator::ZeroRotator;
		if(building.RotationIndex != -1)
		{
			rotation = City->BuildingRotations[building.RotationIndex];
		}
			
		// Calculate building scale
		FVector scale = FVector::OneVector;
		if(building.ScaleIndex != -1)
		{
			scale = City->BuildingScales[building.ScaleIndex];
		}
		int scaleFactor = building.Size;

		scale *= scaleFactor;
		location += FVector(City->BaseBlockSize,City->BaseBlockSize,0) * scaleFactor / 2;			

		FTransform Transform = FTransform(rotation, location, scale);
		int meshInstanceIndex = City->BuildingMeshesComponent[meshIndex]->AddInstance(Transform);
	}
	
}

// Sets default values
ACityGenerator::ACityGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	FloorMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("City Block Meshes"));
	FloorMeshComponent->SetupAttachment(RootComponent);
	FloorMeshComponent->SetStaticMesh(FloorMesh);
}

void ACityGenerator::Randomize()
{
	// Destroy previous datas
	ClearCity();

	// Generate city blocks
	GenerateBlocks();
	
	// Create Meshes
	CreateMeshes();
}

void ACityGenerator::ClearCity_Implementation()
{
	FloorMeshComponent->ClearInstances();
	
	for(int i = 0; i < BuildingMeshesComponent.Num(); ++i)
	{
		BuildingMeshesComponent[i]->ClearInstances();
	}

	CityBlocks.Empty();

	for(int i = 0 ; i < BuildingMeshes.Num(); ++i)
	{
		if(NumberOfBuildingInstancePerBuildingType.Num() <= i)
		{
			NumberOfBuildingInstancePerBuildingType.Add(0);
		}
		else
		{
			NumberOfBuildingInstancePerBuildingType[i] = 0;
		}
	}
}

void ACityGenerator::GenerateBlocks()
{
	// Initialize an array of bool to check occupied areas
	TArray<TArray<bool>> CityBlockRepresentation;
	CityBlockRepresentation.AddDefaulted(CitySize.X);
	for(int i = 0; i < CitySize.X; ++i)
	{
		CityBlockRepresentation[i].AddDefaulted(CitySize.Y);
	}

	int BlockMaxSize = GetMaxBlockSize();
	
	for(int x = 0; x < CitySize.X; ++x)
	{
		for(int y = 0; y < CitySize.Y; ++y)
		{
			if(CityBlockRepresentation[x][y])
			{
				continue;
			}

			int maxSize = BlockMaxSize;

			for(int i = 0; i < maxSize; ++i)
			{
				if(x + i >= CitySize.X - 1)
				{
					maxSize = i;
				}
				for(int j = 0; j < maxSize; ++j)
				{
					if(y + j >= CitySize.Y - 1 || CityBlockRepresentation[x + i][y + j])
					{
						maxSize = j;
						break;
					}
				}	
			}
			
			int blockSize = GetBlockSizeWithProba(maxSize);
			CityBlocks.Add(CityBlock(FVector2D(blockSize, blockSize), FVector2D(x,y), this));

			// Update representation
			for(int i = 0; i < blockSize; ++i)
			{
				for(int j = 0; j < blockSize; ++j)
				{
					CityBlockRepresentation[x + i][y + j] = true;
				}	
			}
		}		
	}
}

void ACityGenerator::CreateMeshes_Implementation()
{
	for(int i = 0; i < CityBlocks.Num(); ++i)
	{
		FVector relativeLocation = FVector(CityBlocks[i].BlockLocation.X * BaseBlockSize,
			CityBlocks[i].BlockLocation.Y * BaseBlockSize, 0);

		// Scale offset because root is in center
		relativeLocation.X += (BaseBlockSize * CityBlocks[i].BlockSize.X) / 2;
		relativeLocation.Y += (BaseBlockSize * CityBlocks[i].BlockSize.Y) / 2;
		
		FloorMeshComponent->AddInstance(FTransform(FRotator(0,0,0), relativeLocation, FVector(CityBlocks[i].BlockSize.X,CityBlocks[i].BlockSize.Y, 1)));

		CityBlocks[i].CreateMeshes(this);
	}
}

void ACityGenerator::ShowCity()
{
	FloorMeshComponent->SetVisibility(true);
	for(int i = 0; i < BuildingMeshesComponent.Num(); ++i)
	{
		BuildingMeshesComponent[i]->SetVisibility(true);
	}
}

void ACityGenerator::HideCity()
{
	FloorMeshComponent->SetVisibility(false);
	for(int i = 0; i < BuildingMeshesComponent.Num(); ++i)
	{
		BuildingMeshesComponent[i]->SetVisibility(false);
	}
}

FVector ACityGenerator::GetCenterCityPoint() const
{
	return GetActorLocation() + FVector(CitySize.X / 2, CitySize.Y / 2, 0);
}

void ACityGenerator::CityToggleEditorCulling()
{
	CullInEditor = !CullInEditor;
}

int ACityGenerator::GetBuildingSizeWithProba(int MaxSize)
{
	TMap<float, int> weightedWheel;
	TArray<int> BuildingSizes;

	BuildingSizeProbabilities.GetKeys(BuildingSizes);

	float currentProba = 0;
	for(int i = 0; i < BuildingSizes.Num(); ++i)
	{
		if(BuildingSizes[i] <= MaxSize)
		{
			currentProba += BuildingSizeProbabilities[BuildingSizes[i]];
			weightedWheel.Add(currentProba, BuildingSizes[i]);
		}
	}
	
	float value = FMath::RandRange(0.f, currentProba);
	TArray<float> probas;
	weightedWheel.GetKeys(probas);
	
	for(int i = 0; i < probas.Num(); ++i)
	{
		if(value <= probas[i])
		{
			return weightedWheel[probas[i]];
		}
	}

	return 1;
}

int ACityGenerator::GetMaxBuildingSize()
{
	TArray<int> BuildingSizes;
	BuildingSizeProbabilities.GetKeys(BuildingSizes);

	int maxSize = 1;
	for(int i = 0; i < BuildingSizes.Num(); ++i)
	{
		if(BuildingSizes[i] > maxSize)
		{
			maxSize = BuildingSizes[i];
		}
	}
	return maxSize;
}

int ACityGenerator::GetBlockSizeWithProba(int MaxSize)
{
	TMap<float, int> weightedWheel;
	TArray<int> BlockSizes;

	BlockSizeProbability.GetKeys(BlockSizes);

	float currentProba = 0;
	for(int i = 0; i < BlockSizes.Num(); ++i)
	{
		if(BlockSizes[i] <= MaxSize)
		{
			currentProba += BlockSizeProbability[BlockSizes[i]];
			weightedWheel.Add(currentProba, BlockSizes[i]);
		}
	}
	
	float value = FMath::RandRange(0.f, currentProba);
	TArray<float> probas;
	weightedWheel.GetKeys(probas);
	
	for(int i = 0; i < probas.Num(); ++i)
	{
		if(value <= probas[i])
		{
			return weightedWheel[probas[i]];
		}
	}

	return 1;
}

int ACityGenerator::GetMaxBlockSize()
{
	TArray<int> BlockSizes;
	BlockSizeProbability.GetKeys(BlockSizes);

	int maxSize = 1;
	for(int i = 0; i < BlockSizes.Num(); ++i)
	{
		if(BlockSizes[i] > maxSize)
		{
			maxSize = BlockSizes[i];
		}
	}
	return maxSize;
}

void ACityGenerator::GetFittingBuildingsIndexes(TArray<int>& BuildingIndexes, int MaxSize)
{
	for(int i = 0; i < BuildingMeshes.Num(); ++i)
	{
		if(BuildingMeshes[i].AvailableForEverySize
			|| (BuildingMeshes[i].MaxSize >= MaxSize && BuildingMeshes[i].MinSize <= MaxSize))
		{
			BuildingIndexes.Add(i);
		}
	}
}

void ACityGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//Randomize();
	
	FloorMeshComponent->InstanceStartCullDistance = InstancedMeshComponentsStartCullDistance;
	FloorMeshComponent->InstanceEndCullDistance = InstancedMeshComponentsStartCullDistance;
	FloorMeshComponent->SetStaticMesh(FloorMesh);
	
	for(int i = 0; i < BuildingMeshes.Num(); ++i)
	{
		if(BuildingMeshesComponent.Num() <= i || !IsValid(BuildingMeshesComponent[i]))
		{
			FString str = FString("Building Mesh Component " + i);
			UInstancedStaticMeshComponent* NewComp = NewObject<UInstancedStaticMeshComponent>(this, UInstancedStaticMeshComponent::StaticClass(), FName(str));
			
			NewComp->SetupAttachment(RootComponent);
			NewComp->RegisterComponent();
			if(BuildingMeshesComponent.Num() <= i)
				BuildingMeshesComponent.Add(NewComp);
			else
				BuildingMeshesComponent[i] = NewComp;
		}
		
		if(BuildingMeshesComponent[i]->GetStaticMesh() != BuildingMeshes[i].Mesh)
		{
			BuildingMeshesComponent[i]->SetStaticMesh(BuildingMeshes[i].Mesh);	
		}

		BuildingMeshesComponent[i]->InstanceStartCullDistance = InstancedMeshComponentsStartCullDistance;
		BuildingMeshesComponent[i]->InstanceEndCullDistance = InstancedMeshComponentsStartCullDistance;
		
	}

	for(int i = BuildingMeshesComponent.Num(); i >= BuildingMeshes.Num(); --i)
	{
		//BuildingMeshesComponent.RemoveAt(i);
	}
}

// Called when the game starts or when spawned
void ACityGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	if(CityBlocks.Num() == 0)
	{
		//Randomize();
	}
}

bool ACityGenerator::ShouldTickIfViewportsOnly() const
{
	return true;
}

// Called every frame
void ACityGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	if(GetWorld() != nullptr && GetWorld()->IsEditorWorld() && !CullInEditor)
	{
		if(bIsDistanceCulled)
		{
			bIsDistanceCulled = false;
			ShowCity();
		}
		return;
	}
#endif


	auto world = GetWorld();
	if(world == nullptr)
		return;
 
	auto viewLocations = world->ViewLocationsRenderedLastFrame;
	if(viewLocations.Num() == 0)
		return;
 
	FVector camLocation = viewLocations[0];

	if(DistanceCulling > 0 && FVector::DistSquared(camLocation, GetCenterCityPoint()) >= DistanceCulling * DistanceCulling)
	{
		if(!bIsDistanceCulled)
		{
			HideCity();
			bIsDistanceCulled = true;
		}
	}
	else
	{
		if(bIsDistanceCulled)
		{
			bIsDistanceCulled = false;
			ShowCity();
		}
	}

}

