// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_Grabber.h"


// Sets default values for this component's properties
URunebergVR_Grabber::URunebergVR_Grabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Set defaults
	isLockGrab = false;

}


// Called every frame
void URunebergVR_Grabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetAttachParent()) {
		// Update controller location & rotation
		ControllerLocation = GetAttachParent()->GetComponentTransform().GetLocation();
		ControllerRotation = FRotator(GetAttachParent()->GetComponentTransform().GetRotation());

		// Update grabbed object location & rotation (if any)
		if (GrabbedObject) {
			if (isLockGrab) {
				// Get grabbed object & controller transforms and place into cache (save on future calls)
				transformCache1 = GrabbedObject->GetOwner()->GetRootComponent()->GetComponentTransform(); // Grabbed object's original transform
				transformCache2 = GetAttachParent()->GetComponentTransform();							  // Controller's transform

				newGrabbedLocation = ControllerLocation + (ControllerRotation.Vector() * DistanceFromController);
				diffGrabbedRotation = transformCache1.GetRotation() - transformCache2.GetRotation();
				
				if (isJustGrabbed) {
					diffGrabbedLocation = GrabbedObject->GetOwner()->GetActorLocation() - newGrabbedLocation;	
					isJustGrabbed = false;
				}

				// Move grabbed object to target location and add the original offset
				GrabbedObject->SetTargetLocation(newGrabbedLocation + diffGrabbedLocation);
				transformCache1.SetRotation(transformCache2.GetRotation() + diffGrabbedRotation);

			}
			else {
				// Do a regular ranged grab (snaps to 0,0,0 of grabbed object)
				GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
				GrabbedObject->SetTargetRotation(ControllerRotation);
				
			}

		}
	}
	
}

// Ray-Cast and grab an Actor
void URunebergVR_Grabber::Grab(float Reach, bool LockGrab, bool ShowDebugLine, bool SetLocationManually, FVector _ControllerLocation, FRotator _ControllerRotation)
{
	// Set if this grab is a precision grab
	if (LockGrab) {
		isLockGrab = true;
		isJustGrabbed = true;
	}
	else {
		isLockGrab = false;
		isJustGrabbed = false;
	}


	// Set Motion Controller Location & Rotation
	if (SetLocationManually) {
		ControllerLocation = _ControllerLocation;
		ControllerRotation = _ControllerRotation;
	}

	// Set Line Trace (Ray-Cast) endpoints
	FVector LineTraceStart = ControllerLocation;
	FVector LineTraceEnd = ControllerLocation + (ControllerRotation.Vector() * Reach);
	
	// Attempt Grab
	AttemptGrab(LineTraceStart, LineTraceEnd, ShowDebugLine);

}


// Attempt to grab object
void URunebergVR_Grabber::AttemptGrab(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine)
{
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	// Show Debug line (helpful for a visual indicator during testing)
	if (bShowDebugLine) {
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
		// Only grab an object with a Physics Handle
		PhysicsHandle = ActorHit->FindComponentByClass<UPhysicsHandleComponent>();
		// UE_LOG(LogTemp, Warning, TEXT("I grabbed : %s"), *ActorHit->GetName());

		if (PhysicsHandle)
		{
			// Physics Handle found! Attempt to Grab Object
			UPrimitiveComponent* ComponentToGrab = Cast<UPrimitiveComponent>(ActorHit->GetRootComponent());

			PhysicsHandle->GrabComponentAtLocationWithRotation(
				ComponentToGrab,
				NAME_None,
				ActorHit->GetActorLocation(),
				FRotator(0.0f, 0.0f, 0.0f)
			);


			// If object is successfully grabbed, move object with Controller
			if (PhysicsHandle->GrabbedComponent)
			{
				PhysicsHandle->SetTargetLocation(LineTraceEnd);
				GrabbedObject = PhysicsHandle;
				SetDistanceFromController(FVector::Dist(ActorHit->GetActorLocation(), LineTraceStart));
			}

		}
	}
}

// Set distance from controller
void URunebergVR_Grabber::SetDistanceFromController(float NewDistance)
{
	if (NewDistance > MinDistanceFromController && NewDistance < MaxDistanceFromController) { 
			DistanceFromController = NewDistance; 
	}
}

// Raycast and get any object hit by the line trace
AActor* URunebergVR_Grabber::GetHit(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine)
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
	auto ActorHit = Hit.GetActor();

	// Return any hits
	if (ActorHit) { 
		return ActorHit; 
	}
	else { 
		return nullptr; 
	}
}

// Release hold of object
void URunebergVR_Grabber::Release()
{
	if (GrabbedObject) {

		// Player has latched on to something, release it
		GrabbedObject->ReleaseComponent();
		GrabbedObject = nullptr;
	}
}

// Pull grabbed object
void URunebergVR_Grabber::PullGrabbedObject(int Speed)
{
	if (GrabbedObject) { 
		SetDistanceFromController(DistanceFromController - Speed);
	}
}

// Push grabbed object
void URunebergVR_Grabber::PushGrabbedObject(int Speed)
{
	if (GrabbedObject) {
		SetDistanceFromController(DistanceFromController + Speed);
	}
}