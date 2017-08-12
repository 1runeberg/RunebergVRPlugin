// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_Grabber.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
URunebergVR_Grabber::URunebergVR_Grabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called every frame
void URunebergVR_Grabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Pull-Push Mechanic
	if (GrabbedObject && bIsPullingOrPushing)
	{
		// Update controller location & rotation
		ControllerLocation = GetAttachParent()->GetComponentLocation();
		ControllerRotation = GetAttachParent()->GetComponentRotation();

		UpdatePullPush();
	}
	// Update grabbed object location & rotation (if any)
	else if (GrabbedObject && !bManualAttach) {

		// Update controller location & rotation
		ControllerLocation = GetAttachParent()->GetComponentLocation();
		ControllerRotation = GetAttachParent()->GetComponentRotation();

		switch (GrabType)
		{
		case EGrabTypeEnum::PRECISION_GRAB:
		case EGrabTypeEnum::SNAP_GRAB:

			// Add controller rotation offsets
			ControllerRotation.Add(StandardOffset.Pitch, StandardOffset.Yaw, StandardOffset.Roll);
			if (RotationOffset != FRotator::ZeroRotator)
			{
				ControllerRotation.Add(RotationOffset.Pitch, RotationOffset.Yaw, RotationOffset.Roll);
			}

			// Set grabbed object rotation
			GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
			GrabbedObject->SetTargetRotation(ControllerRotation);
			break;

		case EGrabTypeEnum::LOCK_GRAB:
		case EGrabTypeEnum::DANGLING_GRAB:
		case EGrabTypeEnum::PRECISION_LOCK:
			GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
			break;

		default:
			break;
		}
	}

	// Day Night cycle
	if (bIsGrabbingSun)
	{
		// Update controller location & rotation
		ControllerLocation = GetAttachParent()->GetComponentLocation();
		ControllerRotation = GetAttachParent()->GetComponentRotation();

		// Update Day-Night Cycle
		UpdateDayNight();
	}
}

// Ray-Cast and grab an Actor
AActor* URunebergVR_Grabber::Grab(float Reach, bool ScanOnlyWillManuallyAttach, EGrabTypeEnum GrabMode, FName TagName, FRotator Rotation_Offset, bool RetainObjectRotation, bool RetainDistance, bool ShowDebugLine)
{
	// Set component vars
	GrabType = GrabMode;
	DistanceFromController = Reach;
	RotationOffset = Rotation_Offset;
	bManualAttach = ScanOnlyWillManuallyAttach;

	// Update controller location & rotation
	ControllerLocation = GetAttachParent()->GetComponentLocation();
	ControllerRotation = GetAttachParent()->GetComponentRotation();

	// Calculate Standard Offset - invert Roll to ensure rotation of grabbed objects are retained
	if (ControllerRotation.Roll < 0)
	{
		StandardOffset = FRotator(0.f, 0.f, FMath::Abs(ControllerRotation.Roll));
	}
	else if (ControllerRotation.Roll > 0) 
	{
		StandardOffset = FRotator(0.f, 0.f, ControllerRotation.Roll * -1.f);
	}
	

	// Show Debug line (helpful for a visual indicator during testing)
	if (ShowDebugLine) {
		// Draw Debug Line Trace
		DrawDebugLine(
			GetWorld(),
			this->GetComponentLocation(),
			this->GetComponentLocation() + (this->GetComponentRotation().Vector() * Reach),
			FColor(255, 0, 0),
			false, -1, 0,
			12.0f
		);
	}

	// Line trace
	AActor* ActorHit = GetHit(this->GetComponentLocation(), this->GetComponentLocation() + (this->GetComponentRotation().Vector() * Reach), RetainDistance, ShowDebugLine);

	// Check if there's a valid object to grab
	if (ActorHit)
	{
		// Only grab an object with a Physics Handle
		GrabbedObject = ActorHit->FindComponentByClass<UPhysicsHandleComponent>();
		//UE_LOG(LogTemp, Warning, TEXT("GRABBER - I grabbed : %s"), *ActorHit->GetName());

		// Automatic Attachment - Attach to Physics Handle
		if (GrabbedObject)
		{
			// Check for actor tag
			if (!TagName.IsNone())
			{
				if (!GrabbedObject->ComponentHasTag(TagName))
				{
					//UE_LOG(LogTemp, Warning, TEXT("GRABBER - Couldn't find %s tag in this physics handle."), *TagName.ToString());
					return nullptr;
				}
			}

			// Do a Physics Handle Grab if automatic attachment is selected
			if (!bManualAttach)
			{
				// Physics Handle found! Attempt to Grab Object
				UPrimitiveComponent* ComponentToGrab = Cast<UPrimitiveComponent>(ActorHit->GetRootComponent());

				// Make object face controller
				//FRotator TempRotator = UKismetMathLibrary::FindLookAtRotation(GrabbedObject->GetOwner()->GetActorLocation(), GetAttachParent()->GetComponentLocation());
				//GrabbedObject->SetTargetRotation(TempRotator);

				// Check for precision grab
				if (GrabType == EGrabTypeEnum::PRECISION_GRAB)
				{
					// Grab
					GrabbedObject->GrabComponentAtLocationWithRotation(
						ComponentToGrab,
						NAME_None,
						NewGrabbedLocation, // NewGrabbedLocation holds the impact point of the line trace
						RetainObjectRotation ? ActorHit->GetActorRotation() : ControllerRotation
					);
					
					// Set transform
					GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
					GrabbedObject->SetTargetRotation(ControllerRotation);

				}
				else if (GrabType == EGrabTypeEnum::DANGLING_GRAB)
				{
					// Grab
					GrabbedObject->GrabComponentAtLocation(
						ComponentToGrab,
						NAME_None,
						NewGrabbedLocation // NewGrabbedLocation holds the impact point of the line trace
					);

					// Set transform
					GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
					GrabbedObject->SetTargetRotation(ControllerRotation);

				}
				else if (GrabType == EGrabTypeEnum::PRECISION_LOCK)
				{
					// Grab
					GrabbedObject->GrabComponentAtLocationWithRotation(
						ComponentToGrab,
						NAME_None,
						NewGrabbedLocation, // NewGrabbedLocation holds the impact point of the line trace
						RetainObjectRotation ? ActorHit->GetActorRotation() : ControllerRotation
					);

					// Set transform
					GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
				}
				else
				{
					// Grab
					GrabbedObject->GrabComponentAtLocationWithRotation(
						ComponentToGrab,
						NAME_None,
						ActorHit->GetActorLocation(),
						RetainObjectRotation ? ActorHit->GetActorRotation() : FRotator::ZeroRotator
					);

					// Set transform
					GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
					if (GrabType == EGrabTypeEnum::SNAP_GRAB)
					{
						GrabbedObject->SetTargetRotation(ControllerRotation);
					}

				}
			}

			// UE_LOG(LogTemp, Warning, TEXT("GRABBER - Returning Actor %s."), *ActorHit->GetName());
			return ActorHit;
		}

	}

	return nullptr;
}

// Raycast and get any object hit by the line trace
AActor* URunebergVR_Grabber::GetHit(FVector LineTraceStart, FVector LineTraceEnd, bool RetainDistance, bool bShowDebugLine)
{
	// Do line trace / ray-cast
	FHitResult	Hit;
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

		// Update Distance with hit distance
		if (!RetainDistance)
		{
			DistanceFromController = Hit.Distance;
		}

		// Set Grabbed transform for precision grabs
		NewGrabbedLocation = Hit.ImpactPoint;

		// Send back actor that was hit by the line trace
		return ActorHit; 
	}
	else { 
		return nullptr; 
	}
}

// Release hold of object
AActor* URunebergVR_Grabber::Release()
{
	bIsGrabbingSun = false;

	if (GrabbedObject) {


		// Check if we're currently pulling or pushing the grabbed object
		if (bIsPullingOrPushing)
		{
			StopPull();
			StopPush();
		}

		// Save the currently attached object
		AActor* CurrentlyGrabbed = GrabbedObject->GetOwner();

		if (!bManualAttach)
		{
			// Player has latched on to something, release it
			GrabbedObject->ReleaseComponent();
		}

		GrabbedObject = nullptr;
		return CurrentlyGrabbed;
	}

	return nullptr;
}

// Set distance from controller
void URunebergVR_Grabber::SetDistanceFromController(float NewDistance, float MinDistance, float MaxDistance)
{
	// Set specified bounds
	MinDistanceFromController = MinDistance;
	MaxDistanceFromController = MaxDistance;

	//UE_LOG(LogTemp, Warning, TEXT("GRABBER - MinDistance: %f   MaxDistance: %f"), MinDistanceFromController, MaxDistanceFromController);
	//UE_LOG(LogTemp, Warning, TEXT("GRABBER - CurrentDistance: %f   Speed: %f"), DistanceFromController, Speed);

	// Check if we're pulling
	if (Speed < 0.f)
	{
		if (NewDistance > MinDistanceFromController) {
			DistanceFromController = NewDistance;
			if (GrabbedObject)
			{
				// Update controller location & rotation
				//UE_LOG(LogTemp, Warning, TEXT("GRABBER - PULLING..."));
				ControllerLocation = GetAttachParent()->GetComponentLocation();
				ControllerRotation = GetAttachParent()->GetComponentRotation();
				GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));

			}
		}
	}
	// Check if we're pushing
	else if (Speed > 0.f)
	{
		if (NewDistance < MaxDistanceFromController) {
			DistanceFromController = NewDistance;
			if (GrabbedObject)
			{
				// Update controller location & rotation
				//UE_LOG(LogTemp, Warning, TEXT("GRABBER - PUSHING..."));
				ControllerLocation = GetAttachParent()->GetComponentLocation();
				ControllerRotation = GetAttachParent()->GetComponentRotation();
				GrabbedObject->SetTargetLocation(ControllerLocation + (ControllerRotation.Vector() * DistanceFromController));
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("GRABBER - NewDistance: %f   Speed: %f"),NewDistance, Speed);

}

// Pull grabbed object
void URunebergVR_Grabber::PullGrabbedObject(float PullSpeed, float MinDistance, float MaxDistance)
{
	// Update controller location & rotation
	ControllerLocation = GetAttachParent()->GetComponentLocation();
	ControllerRotation = GetAttachParent()->GetComponentRotation();

	if (GrabbedObject) { 
		// Set variables and begin pull
		Speed = FMath::Abs(PullSpeed) * -1.f;
		MinDistanceFromController = MinDistance;
		MaxDistanceFromController = MaxDistance;
		bIsPullingOrPushing = true;
	}
}

// Push grabbed object
void URunebergVR_Grabber::PushGrabbedObject(float PushSpeed, float MinDistance, float MaxDistance)
{
	// Update controller location & rotation
	ControllerLocation = GetAttachParent()->GetComponentLocation();
	ControllerRotation = GetAttachParent()->GetComponentRotation();

	if (GrabbedObject) {
		// Set variables and begin pull
		Speed = FMath::Abs(PushSpeed);
		MinDistanceFromController = MinDistance;
		MaxDistanceFromController = MaxDistance;
		bIsPullingOrPushing = true;
	}
}

// Update Pulled-Pushed Object
void URunebergVR_Grabber::UpdatePullPush()
{
	// Get the distance from the controller
	DistanceFromController = FVector::Distance(ControllerLocation, GrabbedObject->GetOwner()->GetActorLocation());

	// Try to set the new distance
	SetDistanceFromController(DistanceFromController + Speed, MinDistanceFromController, MaxDistanceFromController);
}

// Stop Pull
AActor* URunebergVR_Grabber::StopPull()
{
	if (Speed < 0.f)
	{
		// Stop Pull
		bIsPullingOrPushing = false;
	}
	else 
	{
		return nullptr;
	}

	if (GrabbedObject)
	{
		// Save the currently attached object
		AActor* CurrentlyGrabbed = GrabbedObject->GetOwner();
		return CurrentlyGrabbed;
	}

	return nullptr;
}


// Stop ush
AActor* URunebergVR_Grabber::StopPush()
{
	if (Speed > 0.f)
	{
		// Stop Push
		bIsPullingOrPushing = false;
	}
	else
	{
		return nullptr;
	}

	if (GrabbedObject)
	{
		// Save the currently attached object
		AActor* CurrentlyGrabbed = GrabbedObject->GetOwner();
		return CurrentlyGrabbed;
	}

	return nullptr;
}

// Cycle World Day/Night
bool URunebergVR_Grabber::GrabSun(AActor* Sky_Sphere, float SunCycleRate)
{
	// Check for given sky sphere
	if (Sky_Sphere)
	{
		// Check if this is a valid Skysphere
		SkySphere = Sky_Sphere;
		UObjectPropertyBase* ObjectProp = FindField<UObjectPropertyBase>(Sky_Sphere->GetClass(), FName("Directional Light Actor"));

		if (ObjectProp) 
		{
			// Valid Skysphere - get the directional light actor
			void* ObjectPtr = ObjectProp->GetObjectPropertyValue_InContainer(SkySphere);
			if (ObjectPtr) {
				SunDirectionalLightActor = static_cast<ADirectionalLight*>(ObjectPtr);

				if (SunDirectionalLightActor)
				{
					SunDirectionalLightComponent = SunDirectionalLightActor->GetLightComponent();
				}
				else 
				{
					// No directional light for this sphere
					return false;
				}
				
				if (!SunDirectionalLightComponent)
				{
					// Sun directional light component not found - possible invalid skysphere
					return false;
				}

			}
		}
	}
	else 
	{
		// Not a valid sky sphere
		return false;
	}


	// Check for valid given params
	if (SunCycleRate > 0.1f)
	{
		// Update controller location & rotation
		ControllerLocation = GetAttachParent()->GetComponentLocation();
		ControllerRotation = GetAttachParent()->GetComponentRotation();

		// Set global params
		RotationDuringGrab = GetAttachParent()->RelativeRotation;
		CycleRate = SunCycleRate;

		// Calculate the Distance from the Sun Reference Point (in case we are grabbing the sun for the day/night cycle mechanic
		DistanceFromSun = FVector::Distance(FVector(ControllerLocation.X, 0.f, 0.f), SunReferencePoint);

		// Ensure Light Actor is moveable
		SunDirectionalLightActor->SetMobility(EComponentMobility::Movable);

		// Initiate update on tick
		bIsGrabbingSun = true;
		return true;
	}

	return false;
}

// Update Day/Night Cycle
void URunebergVR_Grabber::UpdateDayNight() 
{
	if (SkySphere && SunDirectionalLightActor)
	{
		// Get Current Distance From Sun
		float CurrentDistanceFromSun = FVector::Distance(FVector(ControllerLocation.X, 0.f, 0.f), SunReferencePoint);

		// Check direction of controller movement
		float NewPitch = 0.f;
		FRotator DeltaRotation = FRotator::ZeroRotator;
		//UE_LOG(LogTemp, Warning, TEXT("GRABBER - CurrentDist: %f   Prev Dist: %f"),CurrentDistanceFromSun, DistanceFromSun);


		// WIP: Check for instances of the "reverse dawn" issue and compensate
		if (GetAttachParent()->RelativeRotation.Pitch < -22.5f && SunDirectionalLightActor->GetActorRotation().Pitch > HorizonPitch)
		{
			// Do not move beyond controller twist 
			DeltaRotation = FRotator::ZeroRotator;
		}
		else if ((bMoveEast && CurrentDistanceFromSun > DistanceFromSun) || (!bMoveEast && CurrentDistanceFromSun < DistanceFromSun))
		{
			// Retain current direction boolean (no change) and calculate delta rotation
			DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetAttachParent()->RelativeRotation, RotationDuringGrab);
		}
		else 
		{
			// Check Direction
			if (CurrentDistanceFromSun < DistanceFromSun)
			{
				// Check for a negative pitch rotation for the controller
				if (GetAttachParent()->RelativeRotation.Pitch <= 0.f)
				{
					// Negative rotation pitch -- invert direction (keep moving in the same direction to avoid the sun moving back after the controller hits a negative pitch)
					DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetAttachParent()->RelativeRotation, RotationDuringGrab);
					bMoveEast = true;
					//UE_LOG(LogTemp, Warning, TEXT("GRABBER - 1"));
				}
				else
				{
					// Positive rotation pitch -- move in the direction of the controller (towards or away from the westmost reference point)
					DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(RotationDuringGrab, GetAttachParent()->RelativeRotation);
					bMoveEast = false;
					//UE_LOG(LogTemp, Warning, TEXT("GRABBER - 2"));
				}
			}
			else
			{
				// Check for a negative pitch rotation for the controller
				if (GetAttachParent()->RelativeRotation.Pitch <= 0.f)
				{
					// Positive rotation pitch -- move in the direction of the controller (towards or away from the westmost reference point)
					DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(RotationDuringGrab, GetAttachParent()->RelativeRotation);
					bMoveEast = false;
					//UE_LOG(LogTemp, Warning, TEXT("GRABBER - 3"));
				}
				else
				{
					// Negative rotation pitch -- invert direction (keep moving in the same direction to avoid the sun moving back after the controller hits a negative pitch)
					DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetAttachParent()->RelativeRotation, RotationDuringGrab);
					bMoveEast = true;
					//UE_LOG(LogTemp, Warning, TEXT("GRABBER - 4"));
				}
			}

		}


		// Update light rotation
		if (bMoveEast)
		{
			NewPitch = FMath::Abs(DeltaRotation.Pitch) * -1.f * CycleRate; // ensure negative delta pitch
		}
		else 
		{
			NewPitch = FMath::Abs(DeltaRotation.Pitch) * CycleRate; // ensure positive delta pitch
		}

		SunDirectionalLightActor->AddActorLocalRotation(FRotator(NewPitch, 0.f, 0.f));

		// Update day-night cycle mechanic variables
		DistanceFromSun = CurrentDistanceFromSun;
		RotationDuringGrab = GetAttachParent()->RelativeRotation;

		// Update the sun's brightness
		UFloatProperty* FloatProp = FindField<UFloatProperty>(SkySphere->GetClass(), FName("Sun Brightness"));
		if (FloatProp)
		{
			void* ValuePtr = FloatProp->ContainerPtrToValuePtr<void>(SkySphere);
			if (SunDirectionalLightActor->GetActorRotation().Pitch > HorizonPitch)
			{
				FloatProp->SetFloatingPointPropertyValue(ValuePtr, 0.f);
			}
			else
			{
				FloatProp->SetFloatingPointPropertyValue(ValuePtr, SunBrightness);
			}
		}

		// Update the sun direction
		FOutputDeviceNull ar;
		SkySphere->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), ar, NULL, true);
	}
}