// Fill out your copyright notice in the Description page of Project Settings.


#include "CityGenerator.h"

#include "Gomorrah/Gomorrah.h"

FCityBlock::FCityBlock()
	:FCityBlock(FVector2D(1,1), FVector2D())
{
}

FCityBlock::FCityBlock(FVector2D BlockSize, FVector2D BlockLocation)
	: BlockSize(BlockSize)
	, BlockLocation(BlockLocation)
	, FloorComponentRef(nullptr)
{
	
}

// Sets default values
ACityGenerator::ACityGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	MeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("City Block Meshes"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetStaticMesh(FloorMesh);
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

void ACityGenerator::ClearCity()
{
	MeshComponent->ClearInstances();
	
	for(int i = 0; i < CityBlocks.Num(); ++i)
	{
		// Do something
		
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
				CityBlocks.Add(FCityBlock(FVector2D(sizeX, sizeY), blockLocation));

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
				CityBlocks.Add(FCityBlock(FVector2D(1, 1), FVector2D(x,y)));
			}
		}		
	}
}

void ACityGenerator::CreateMeshes()
{
	for(int i = 0; i < CityBlocks.Num(); ++i)
	{
		FVector relativeLocation = FVector(CityBlocks[i].BlockLocation.X * BaseBlockSize,
			CityBlocks[i].BlockLocation.Y * BaseBlockSize, 0);

		// Scale offset because root is in center
		relativeLocation.X += (BaseBlockSize * CityBlocks[i].BlockSize.X) / 2;
		relativeLocation.Y += (BaseBlockSize * CityBlocks[i].BlockSize.Y) / 2;
		
		int MeshIndex = MeshComponent->AddInstance(FTransform(FRotator(), relativeLocation, FVector(CityBlocks[i].BlockSize.X,CityBlocks[i].BlockSize.Y, 1)));

		//CityBlocks[i].FloorComponentRef = MeshComponent;
	}
}

void ACityGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//Randomize();
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

// Called every frame
void ACityGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

