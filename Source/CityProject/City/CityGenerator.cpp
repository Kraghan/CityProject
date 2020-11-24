// Fill out your copyright notice in the Description page of Project Settings.


#include "CityGenerator.h"

ACityGenerator::CityBuilding::CityBuilding(int x, int y)
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
	, FloorComponentRef(nullptr)
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

	for(int x = 0; x < BlockSize.X; ++x)
	{
		for(int y = 0; y < BlockSize.Y; ++y)
		{
			// Skip if occupied
			if(CityBlockRepresentation[x][y])
			{
				continue;
			}
			CityBuilding newBuilding = CityBuilding(x,y);

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
			if(x <= BlockSize.X - City->BuildingMaxBlockOccupation && y <= BlockSize.Y - City->BuildingMaxBlockOccupation)
			{
				newBuilding.Size = FMath::RandRange(1, City->BuildingMaxBlockOccupation);
			}

			// Mesh
			if(City->BuildingMeshes.Num() != 0)
			{				
				newBuilding.MeshIndex = FMath::RandRange(0, City->BuildingMeshes.Num() - 1);
            }

			Buildings.Add(newBuilding);
			
			// Add Holograms
			City->OnPostBuildingCreated(newBuilding.MeshIndex);

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
		CityBuilding& building = Buildings[i];
		
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

		// Add mesh
		int meshIndex = building.MeshIndex;
			
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

	UE_LOG(LogTemp, Log, TEXT("Blocks before : %d"), CityBlocks.Num());
	// Generate city blocks
	GenerateBlocks();
	UE_LOG(LogTemp, Log, TEXT("Blocks after : %d"), CityBlocks.Num());
	
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

	// Place different size of block
	for(int sizeX = MaxBlockSize; sizeX > 0; --sizeX)
	{
		int sizeY = KeepSquaredBlock ? sizeX : FMath::RandRange(1, MaxBlockSize);

		int maxNumberPlaceable = CitySize.X / sizeX * CitySize.Y / sizeY;
		
		int numberToGenerate = FMath::RandRange(0, maxNumberPlaceable);

		for(int i = 0; i < numberToGenerate; ++i)
		{
			// Find random location
			FVector2D blockLocation = FVector2D(FMath::RandRange(0, static_cast<int>(CitySize.X) - sizeX),
				FMath::RandRange(0, static_cast<int>(CitySize.Y) - sizeY));

			// Check if this space is free or not
			bool spaceIsFree = true;
			for(int x = 0; x < sizeX; ++x)
			{
				for(int y = 0; y < sizeY; ++y)
				{
					if(CityBlockRepresentation[blockLocation.X + x][blockLocation.Y + y])
					{
						spaceIsFree = false;
						break;
					}
				}
				
				if(!spaceIsFree)
				{
					break;
				}
			}

			// If space is free, create block here
			if(spaceIsFree)
			{
				CityBlocks.Add(CityBlock(FVector2D(sizeX, sizeY), blockLocation, this));

				// Update representation
				for(int x = 0; x < sizeX; ++x)
				{
					for(int y = 0; y < sizeY; ++y)
					{
						CityBlockRepresentation[blockLocation.X + x][blockLocation.Y + y] = true;
					}	
				}
			}
		}
	}

	// Fill remaining empty block
	for(int x = 0; x < CitySize.X; ++x)
	{
		for(int y = 0; y < CitySize.Y; ++y)
		{
			if(!CityBlockRepresentation[x][y])
			{
				CityBlocks.Add(CityBlock(FVector2D(1, 1), FVector2D(x,y), this));
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
		
		FloorMeshComponent->AddInstance(FTransform(FRotator(), relativeLocation, FVector(CityBlocks[i].BlockSize.X,CityBlocks[i].BlockSize.Y, 1)));

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
		
		if(BuildingMeshesComponent[i]->GetStaticMesh() != BuildingMeshes[i])
		{
			BuildingMeshesComponent[i]->SetStaticMesh(BuildingMeshes[i]);	
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
		Randomize();
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

