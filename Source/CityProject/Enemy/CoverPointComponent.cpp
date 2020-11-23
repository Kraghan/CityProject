// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverPointComponent.h"

FCoverageData::FCoverageData()
	: FCoverageData(FVector::ForwardVector, 180.f)
{
}

FCoverageData::FCoverageData(FVector Direction, float Angle)
	: CoverageAngle(Angle)
	, CoverageDirection(Direction)
{
}

// Sets default values for this component's properties
UCoverPointComponent::UCoverPointComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CoverageDatas.Add(FCoverageData());
}

bool UCoverPointComponent::CoverIsFree() const
{
	return CharacterInCover != nullptr;
}


// Called when the game starts
void UCoverPointComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < CoverageDatas.Num(); ++i)
	{
		CoverageDatas[i].CoverageDirection.Normalize();
	}
	
}


// Called every frame
void UCoverPointComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

