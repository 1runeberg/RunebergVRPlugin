// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

#include "VR_CPP.h"
#include "VRPawnComponent_VRMovement.h"


// Sets default values for this component's properties
UVRPawnComponent_VRMovement::UVRPawnComponent_VRMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVRPawnComponent_VRMovement::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the Pawn
	VRPawn = GetOwner();
	
}


// Called every frame
void UVRPawnComponent_VRMovement::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (VRPawn && IsMoving) {

		// Check if there's a movement reference actor
		if(MovementReference) {
			TargetRotation = FRotator(MovementReference->ComponentToWorld.GetRotation());
		}

		VRPawn->TeleportTo(VRPawn->GetActorLocation() + (TargetRotation.Vector() * MovementSpeed), VRPawn->GetActorRotation());
	}
}

// Enable VR Movement
void UVRPawnComponent_VRMovement::EnableVRMovement(float PawnMovementSpeed, USceneComponent* PawnMovementReference, FRotator PawnManualRotation)
{
	if (VRPawn) {

		// Check if there's a movement reference actor
		if (PawnMovementReference) {
			MovementReference = PawnMovementReference;
		}
		else {
			MovementReference = nullptr;
			TargetRotation = PawnManualRotation;
		}

		// Set Movement speed
		if (PawnMovementSpeed != 0.0f) {
			MovementSpeed = PawnMovementSpeed;
		}

		// Set the Pawn to moving state
		IsMoving = true;
	}
}

// Disable VR Movement
void UVRPawnComponent_VRMovement::DisableVRMovement()
{
	// Set the Pawn to static state
	IsMoving = false;
}