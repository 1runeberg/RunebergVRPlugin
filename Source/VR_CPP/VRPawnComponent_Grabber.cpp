// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "VR_CPP.h"
#include "VRPawnComponent_Grabber.h"


// Sets default values for this component's properties
UVRPawnComponent_Grabber::UVRPawnComponent_Grabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVRPawnComponent_Grabber::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UVRPawnComponent_Grabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetAttachParent()) {
		// Update controller location
		ControllerLocation = GetAttachParent()->GetComponentTransform().GetLocation();
		ControllerRotation = FRotator(GetAttachParent()->GetComponentTransform().GetRotation());

		// Update Grabbed Object Location and Controller location	
		UpdateGrabbedObjectLocation(GetAttachParent()->GetComponentTransform().GetLocation(), FRotator(GetAttachParent()->GetComponentTransform().GetRotation()));
	}
	
}

// Ray-Cast and grab an Actor
void UVRPawnComponent_Grabber::Grab(float Reach, bool ShowDebugLine, bool SetLocationManually, FVector _ControllerLocation, FRotator _ControllerRotation)
{

	// Set Motion Controller Location & Rotation
	if (SetLocationManually) {
		_ControllerLocation = ControllerLocation;
		_ControllerRotation = ControllerRotation;
	}

	// Set Line Trace (Ray-Cast) endpoints
	FVector LineTraceStart = ControllerLocation;
	FVector LineTraceEnd = ControllerLocation + (ControllerRotation.Vector() * Reach);
	
	// Attempt Grab
	AttemptGrab(LineTraceStart, LineTraceEnd, ShowDebugLine);
}

// Attempt to grab object
void UVRPawnComponent_Grabber::AttemptGrab(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine)
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


			PhysicsHandle->GrabComponent(
				ComponentToGrab,
				NAME_None,
				ActorHit->GetActorLocation(),
				true		// allow for rotation
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
void UVRPawnComponent_Grabber::SetDistanceFromController(float NewDistance)
{
	if (NewDistance > MinDistanceFromController && NewDistance < MaxDistanceFromController) { 
			DistanceFromController = NewDistance; 
	}
}

// Raycast and get any object hit by the line trace
AActor* UVRPawnComponent_Grabber::GetHit(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine)
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

// Update grabbed object location & render
void UVRPawnComponent_Grabber::UpdateGrabbedObjectLocation(FVector _ControllerLocation, FRotator _ControllerRotation)
{
	// Set Left Controller Location & Rotation
	_ControllerLocation = ControllerLocation;
	_ControllerRotation = ControllerRotation;

	// Update Location of Grabbed Actor(s) if present
	if (GrabbedObject) {
		GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
		GrabbedObject->SetTargetRotation(ControllerRotation);
	}
}

// Release hold of object
void UVRPawnComponent_Grabber::Release()
{
	if (GrabbedObject) {

		// Player has latched on to something, release it
		GrabbedObject->ReleaseComponent();
		GrabbedObject = nullptr;
	}
}

// Pull grabbed object
void UVRPawnComponent_Grabber::PullGrabbedObject(int Speed)
{
	if (GrabbedObject) { 
		SetDistanceFromController(DistanceFromController - Speed);
	}
}

// Push grabbed object
void UVRPawnComponent_Grabber::PushGrabbedObject(int Speed)
{
	if (GrabbedObject) {
		SetDistanceFromController(DistanceFromController + Speed);
	}
}