// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_Movement.h"


// Sets default values for this component's properties
URunebergVR_Movement::URunebergVR_Movement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URunebergVR_Movement::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the Pawn
	VRPawn = GetOwner();
	
}


// Called every frame
void URunebergVR_Movement::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
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
void URunebergVR_Movement::EnableVRMovement(float PawnMovementSpeed, USceneComponent* PawnMovementReference, FRotator PawnManualRotation)
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
void URunebergVR_Movement::DisableVRMovement()
{
	// Set the Pawn to static state
	IsMoving = false;
}