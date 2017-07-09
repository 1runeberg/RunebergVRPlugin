// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_Movement.h"


// Sets default values for this component's properties
URunebergVR_Movement::URunebergVR_Movement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void URunebergVR_Movement::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the Pawn
	VRPawn = GetOwner();

	// TODO: Get reference to Pawn's camera if none was provided
	//if (!UnevenTerrainVariables.Camera->IsValidLowLevel())
	//{

	//	TArray <USceneComponent*> PawnChildren;
	//	VRPawn->GetRootComponent()->GetChildrenComponents(true, PawnChildren);

	//	// Get all children component of pawn
	//	for (int32 i = 0; i < PawnChildren.Num(); i++)
	//	{
	//		UCameraComponent* TestForCamera = Cast<UCameraComponent>(PawnChildren[i]);

	//		if (TestForCamera->IsValidLowLevel())
	//		{
	//			UnevenTerrainVariables.Camera = TestForCamera;
	//			break;
	//		}
	//	}
	//}

}

// Called every frame
void URunebergVR_Movement::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (VRPawn && (IsMoving || IsBouncingBackFromVRBounds)) {

		// Check if there's a movement reference actor
		if(CurrentMovementDirectionReference) {

			// Set rotation/orientation
			TargetRotation = FRotator(CurrentMovementDirectionReference->ComponentToWorld.GetRotation());

			// Apply rotation offset
			if (OffsetRotation.Equals(FRotator::ZeroRotator))
			{
				TargetRotation = FRotator(CurrentMovementDirectionReference->ComponentToWorld.GetRotation());
			}
			else 
			{
				TargetRotation = FRotator(CurrentMovementDirectionReference->ComponentToWorld.GetRotation()).Add(OffsetRotation.Pitch, OffsetRotation.Yaw, OffsetRotation.Roll);
			}

		}
		else 
		{
			// Apply rotation offset (if any)
			if (!OffsetRotation.Equals(FRotator::ZeroRotator))
			{
				TargetRotation = TargetRotation.Add(OffsetRotation.Pitch, OffsetRotation.Yaw, OffsetRotation.Roll);
			}
		}

		// TODO: Apply Full 360 rotation offset (if any)
		//if (Full360MovementXAxis > 0.01f || Full360MovementYAxis > 0.01f)
		//{
		//	bIsDoingFull360 = true;
		//	TargetRotation = TargetRotation.Add(0.f, FMath::Atan2(Full360MovementXAxis, Full360MovementYAxis), 0.f);
		//}
		//else if (bIsDoingFull360 && Full360MovementXAxis < 0.01f && Full360MovementYAxis < 0.01f)
		//{
		//	// Stop movement if doing full 360 and no x,y axis input is registered (e.g. thumbstick)
		//	DisableVRMovement();
		//	bIsDoingFull360 = false;
		//	return;
		//}

		// Set axis locks : Pitch (Y), Yaw (Z), Roll (X)
		if (bLockPitchY)
		{
			TargetRotation = FRotator(0.f, TargetRotation.Yaw, TargetRotation.Roll);
		}

		if (bLockYawZ)
		{
			TargetRotation = FRotator(TargetRotation.Pitch, 0.f, TargetRotation.Roll);
		}

		if (bLockRollX)
		{
			TargetRotation = FRotator(TargetRotation.Pitch, TargetRotation.Yaw, 0.f);
		}

		// Set Target Location
		FVector TargetLocation;
		if (IsBouncingBackFromVRBounds)
		{
			TargetLocation = VRPawn->GetActorLocation() + (TargetRotation.Vector() * BounceBackSpeed);
		}
		else
		{
			TargetLocation = VRPawn->GetActorLocation() + (TargetRotation.Vector() * CurrentMovementSpeed);
		}
		
		// Do we need to move within Nav Mesh Bounds?
		if (bObeyNavMesh)
		{
			// Check TargetLocation if it's in the nav mesh
			FVector CheckLocation;
			bool bIsWithinNavBounds = GetWorld()->GetNavigationSystem()->K2_ProjectPointToNavigation(
				this,
				TargetLocation,
				CheckLocation,
				(ANavigationData*)0, 0,
				NavMeshTolerance);

			// Check if target location is within the nav mesh
			if (bIsWithinNavBounds)
			{
				// TODO: Check if we need to adjustZ for uneven terrain
				//if (EnableTerrainCheck && UnevenTerrainVariables.Camera->IsValidLowLevel())
				//{
				//	// Do a line trace from prior location to current location to check the Z offset
				//	FHitResult	Hit;
				//	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
				//	UWorld* World = GEngine->GetWorldFromContextObject(GetOwner());
				//	bHit = World->LineTraceSingleByChannel(Hit,
				//		VRPawn->GetActorLocation() + UnevenTerrainVariables.OriginOffset,
				//		TargetLocation,
				//		ECollisionChannel::ECC_Visibility,
				//		TraceParameters);

				//	// Check if we're allowed to step on the component
				//	if (bHit && Hit.GetComponent()->CanCharacterStepUpOn == ECanBeCharacterBase::ECB_Yes)
				//	{

				//		if (bHit)
				//		{
				//			UE_LOG(LogTemp, Warning, TEXT("Actor was hit"));
				//		}
				//		TargetLocation = Hit.Location;
				//	}
				//}

				// Move Pawn to Target Lcoation
				VRPawn->TeleportTo(TargetLocation, VRPawn->GetActorRotation());
				
			}
			else
			{
				// Stop movement
				DisableVRMovement();
			}
		}
		else 
		{
			// Move Pawn to Target Lcoation
			VRPawn->TeleportTo(TargetLocation, VRPawn->GetActorRotation());
		}



	}
}

// Disable VR Bounds Bounce Back Movement
void URunebergVR_Movement::MoveVRPawn(float MovementSpeed, USceneComponent* MovementDirectionReference,  
	bool LockPitchAngle, bool LockYawAngle, bool LockRollAngle, FRotator CustomDirection)
{
	// Check if there's a movement reference actor
	if (MovementDirectionReference) {
		CurrentMovementDirectionReference = MovementDirectionReference;

		// Set axis locks
		this->bLockPitchY = LockPitchAngle;
		this->bLockRollX = LockRollAngle;
		this->bLockYawZ = LockYawAngle;

		// Set Custom Direction as rotation offset
		OffsetRotation = CustomDirection;
	}
	else {
		CurrentMovementDirectionReference = nullptr;
		TargetRotation = CustomDirection;
	}

	// Set Movement speed
	if (MovementSpeed > 0.0001f || MovementSpeed < -0.0001f) {
		CurrentMovementSpeed = MovementSpeed;
	}

	// Set the Pawn to moving state
	IsMoving = true;
}

// Enable VR Movement
void URunebergVR_Movement::EnableVRMovement(float MovementSpeed, USceneComponent* MovementDirectionReference,
	bool ObeyNavMesh, bool LockPitch, bool LockYaw, bool LockRoll, 
	float Full360Movement_XAxis, float Full360Movement_YAxis, FRotator CustomDirection)
{
	Full360MovementXAxis = Full360Movement_XAxis;
	Full360MovementYAxis = Full360Movement_YAxis;

	// Check if we need to respect Nav Mesh Bounds
	bObeyNavMesh = ObeyNavMesh;

	if (VRPawn && !IsBouncingBackFromVRBounds) 
	{
		MoveVRPawn(MovementSpeed, MovementDirectionReference, LockPitch, LockYaw, LockRoll, CustomDirection);
	}
}

// Disable VR Movement
void URunebergVR_Movement::DisableVRMovement()
{
	// Set the Pawn to static state
	IsMoving = false;

	// Reset Offset
	OffsetRotation = FRotator::ZeroRotator;
	Full360MovementXAxis = 0.f;
	Full360MovementYAxis = 0.f;
}

// Apply acceleration multiplier to current movement speed - can be used for smooth acceleration / deceleration
void URunebergVR_Movement::ApplySpeedMultiplier(float SpeedMultiplier, float BaseSpeed, bool UseCurrentSpeedAsBase)
{
	if (UseCurrentSpeedAsBase)
	{
		CurrentMovementSpeed *= SpeedMultiplier;
	}
	else 
	{
		CurrentMovementSpeed = SpeedMultiplier * BaseSpeed;
	}
}

// Disable VR Bounds Bounce Back Movement
void URunebergVR_Movement::DisableVRBounceBack()
{
	if (bResetMovementStateAfterBounce)
	{
		// Bring back movement state of pawn when we started the bounce back
		IsMoving = bIsMovingCache;
	}
	else 
	{
		// Set the Pawn to static state
		IsMoving = false;
	}

	IsBouncingBackFromVRBounds = false;
}

// Timed movement - move pawn for a specified amount of time
void URunebergVR_Movement::TimedMovement(float MovementDuration, float MovementSpeed, USceneComponent* MovementDirectionReference, 
	bool LockPitchY, bool LockYawZ, bool LockRollX, FRotator CustomDirection, bool ObeyNavMesh)
{
	// Start movement
	EnableVRMovement(MovementSpeed, MovementDirectionReference, ObeyNavMesh, LockPitchY, LockYawZ, LockRollX, 0.f, 0.f, CustomDirection);

	// End movement via timer
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &URunebergVR_Movement::DisableVRMovement, MovementDuration, false);
}

// Dash move (timed)  - dash into a predefined direction and time
void URunebergVR_Movement::TimedDashMove(float MovementDuration, float MovementSpeed, FRotator MovementDirection, bool ObeyNavMesh)
{
	// Start movement
	EnableVRMovement(MovementSpeed, nullptr, ObeyNavMesh, false, false, false, 0.f, 0.f, MovementDirection);

	// End movement via timer
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &URunebergVR_Movement::DisableVRMovement, MovementDuration, false);
}

// Bounce back from VR bounds
void  URunebergVR_Movement::BounceBackFromVRBounds(float MovementSpeed, float MovementDuration, bool ResetMovementStateAfterBounce)
{
	// Set wether we want ot reinstate movement state of pawn after the bounce back
	bResetMovementStateAfterBounce = ResetMovementStateAfterBounce;

	if (ResetMovementStateAfterBounce)
	{
		bIsMovingCache = IsMoving;
	}

	// Disable standard VR Movement
	DisableVRMovement();

	// Start movement
	BounceBackSpeed = FMath::Abs(MovementSpeed) * (-1.f);
	IsBouncingBackFromVRBounds = true;

	// End movement via timer
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &URunebergVR_Movement::DisableVRBounceBack, MovementDuration, false);
}