// Fill out your copyright notice in the Description page of Project Settings.


#include "LockableBillboardComponent.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
ULockableBillboardComponent::ULockableBillboardComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;	
	bTickInEditor = true;
}

// Called every frame
void ULockableBillboardComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto world = GetWorld();
	if(world == nullptr)
		return;
 
	auto viewLocations = world->ViewLocationsRenderedLastFrame;
	if(viewLocations.Num() == 0)
		return;
 
	FVector camLocation = viewLocations[0];

	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), camLocation);

	if(LockRoll)
	{
		lookAt.Roll = 0;
	}

	if(LockPitch)
	{
		lookAt.Pitch = 0;
	}

	if(LockYaw)
	{
		lookAt.Yaw = 0;
	}

	SetWorldRotation(lookAt);
	
}

