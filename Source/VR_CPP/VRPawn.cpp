// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
GNU GPLv3 - http://www.gnu.org/licenses/gpl.txt

VR Pawn with C++ & Blueprint common interface functions
Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "VR_CPP.h"
#include "VRPawn.h"

// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player (First Person)
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	// Set Base Eye Height to 0 (SteamVR/Vive)
	BaseEyeHeight = 0.f;

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}


// Ray-Cast and grab an Actor
void AVRPawn::Grab(bool Left, bool Right, float Reach, bool ShowDebugLine, bool SetLocationManually, FVector LeftControllerLocation, FRotator LeftControllerRotation, FVector RightControllerLocation, FRotator RightControllerRotation)
{
	FVector ControllerLocation;
	FVector ControllerRotation;
	FVector LineTraceStart;
	FVector LineTraceEnd;

	// Set default Reach
	if (Reach < 10.0f) { Reach = 10.0f;  }

	// Set Motion Controller Location & Rotation
	if (!SetLocationManually) { 
		LeftControllerLocation = ControllerLocationLeft; 
		LeftControllerRotation = ControllerRotationLeft;

		RightControllerLocation = ControllerLocationRight;
		RightControllerRotation = ControllerRotationRight;
	}

	if (Left) {

		// Set Line Trace (Ray-Cast) endpoints
		LineTraceStart = LeftControllerLocation; 
		LineTraceEnd = LeftControllerLocation + (LeftControllerRotation.Vector() * Reach);

		// Attempt Grab
		AttemptGrab(LineTraceStart, LineTraceEnd, true, ShowDebugLine);
	}

	if (Right) {
		// Set Line Trace (Ray-Cast) endpoints
		LineTraceStart = RightControllerLocation;
		LineTraceEnd = RightControllerLocation + (RightControllerRotation.Vector() * Reach);

		// Attempt Grab
		AttemptGrab(LineTraceStart, LineTraceEnd, false, ShowDebugLine);
	}
	
}


// Attempt to grab Actor
void AVRPawn::AttemptGrab(FVector& LineTraceStart, FVector& LineTraceEnd, bool bGrabLeft, bool bShowDebugLine)
{
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	if(bShowDebugLine) {
		// Draw Debug Line Trace
		DrawDebugLine(
			GetWorld(),
			LineTraceStart,
			LineTraceEnd,
			FColor(255, 0, 0),
			false, -1, 0,
			12.0f
		);
	}
	
	// Line trace
	AActor* ActorHit = GetHit(LineTraceStart, LineTraceEnd, bShowDebugLine);

	// Check if there's a valid object to grab
	if (ActorHit)
	{
		// Only grab an item with a Physics Handle
		PhysicsHandle = ActorHit->FindComponentByClass<UPhysicsHandleComponent>();

		if (PhysicsHandle)
		{
			// Physics Handle found! Attempt to Grab Actor
			UPrimitiveComponent* ComponentToGrab = Cast<UPrimitiveComponent>(ActorHit->GetRootComponent());

			PhysicsHandle->GrabComponent(
				ComponentToGrab,
				NAME_None,
				ActorHit->GetActorLocation(),
				true		// allow for rotation
			);

			// If Actor is successfully grabbed, move Actor with Controller
			if (PhysicsHandle->GrabbedComponent)
			{
				PhysicsHandle->SetTargetLocation(LineTraceEnd);
				if (bGrabLeft) { 
					GrabbedLeft = PhysicsHandle; 
					SetDistanceFromLeftController(FVector::Dist(ActorHit->GetActorLocation(), LineTraceStart));
				}
				else { 
					GrabbedRight = PhysicsHandle; 
					SetDistanceFromRightController(FVector::Dist(ActorHit->GetActorLocation(), LineTraceStart));
				}				
			}

		}
	}
}


// Ray cast and get any Actor hit by the line trace
AActor* AVRPawn::GetHit(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine)
{
	FHitResult	Hit;

	// Do line trace / ray-cast
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		LineTraceStart,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	// See what we hit
	AActor* ActorHit = Hit.GetActor();

	// Return any hits
	if (ActorHit) { return ActorHit; }
	else { return nullptr; }
}


// Release hold of Actor
void AVRPawn::Release(bool Left, bool Right)
{
	if (Left && GrabbedLeft) {
		// Player has latched on to something, release it
		GrabbedLeft->ReleaseComponent();
		GrabbedLeft = nullptr;
	}
	
	if (Right && GrabbedRight) {
		// Player has latched on to something, release it
		GrabbedRight->ReleaseComponent();
		GrabbedRight = nullptr;
	}

}


void AVRPawn::UpdateGrabbedObjectLocation(FVector LeftControllerLocation, FRotator LeftControllerRotation, FVector RightControllerLocation, FRotator RightControllerRotation)
{
	// Set Left Controller Location & Rotation
	ControllerLocationLeft = LeftControllerLocation;
	ControllerRotationLeft = LeftControllerRotation;

	// Set Right Controller Location & Rotation
	ControllerLocationRight = RightControllerLocation;
	ControllerRotationRight = RightControllerRotation;
	
	// Update Location of Grabbed Actor(s) if present
	if (GrabbedLeft) { 
		GrabbedLeft->SetTargetLocation(LeftControllerLocation + (LeftControllerRotation.Vector() * DistanceFromControllerLeft)); 
		GrabbedLeft->SetTargetRotation(LeftControllerRotation); 
	}

	if (GrabbedRight) { 
		GrabbedRight->SetTargetLocation(RightControllerLocation + (RightControllerRotation.Vector() * DistanceFromControllerRight));
		GrabbedRight->SetTargetRotation(RightControllerRotation);
	}
}


void AVRPawn::PullGrabbedObjects(bool ObjectInLeftController, bool ObjectInRightController, int Speed) 
{
	if (ObjectInLeftController) { SetDistanceFromLeftController(DistanceFromControllerLeft - Speed); }
	if (ObjectInRightController) { SetDistanceFromRightController(DistanceFromControllerRight - Speed); }
}


void AVRPawn::PushGrabbedObjects(bool ObjectInLeftController, bool ObjectInRightController, int Speed)
{
	if (ObjectInLeftController) { SetDistanceFromLeftController(DistanceFromControllerLeft + Speed); }
	if (ObjectInRightController) { SetDistanceFromRightController(DistanceFromControllerRight + Speed); }
}

void AVRPawn::SetDistanceFromLeftController(float NewDistance)
{
	if(NewDistance > MinDistanceFromLeftController && NewDistance < MaxDistanceFromLeftController) { DistanceFromControllerLeft = NewDistance; }
}


void AVRPawn::SetDistanceFromRightController(float NewDistance) 
{
	if(NewDistance > MinDistanceFromRightController && NewDistance < MaxDistanceFromRightController) { DistanceFromControllerRight = NewDistance; }
}