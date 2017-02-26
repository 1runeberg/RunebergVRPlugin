// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_Teleporter.h"

// Sets default values for this component's properties
URunebergVR_Teleporter::URunebergVR_Teleporter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Auto Activate this component
	bAutoActivate = true;

}

// Called when the game starts
void URunebergVR_Teleporter::BeginPlay()
{
	Super::BeginPlay();

	// Ensure target marker is not visible at start
	SetVisibility(false, true);
	
}

// Teleport object
void URunebergVR_Teleporter::TeleportObject(AActor* ObjectToTeleport, USceneComponent* TargettingSource, bool ReSpawnMarker)
{
	// Only teleport if targetting is enabled
	if (IsTargetting && ObjectToTeleport) {
		
		// [UNCOMMENT IF NEEDED] Find closest appropriate place to teleport
		//GetWorld()->FindTeleportSpot(ObjectToTeleport, MarkerLocation, MarkerRotation);

		// Teleport object
		TargetLocation.X = MarkerLocation.X;
		TargetLocation.Y = MarkerLocation.Y;

		if (IsMarkerAtFloor) { 
			TargetLocation.Z = ZAdjustment;
			ObjectToTeleport->SetActorLocationAndRotation(TargetLocation, ObjectToTeleport->GetActorRotation());
		}
		else { 
			TargetLocation.Z = MarkerLocation.Z + ZAdjustment;
			ObjectToTeleport->SetActorLocationAndRotation(TargetLocation, ObjectToTeleport->GetActorRotation());
		}

		// Respawn marker forward
		if (ReSpawnMarker) {
			MoveMarker(TargettingSource, true, false, false, false, RespawnDistance);
		}
	}
}

// Spawn marker at given location
void URunebergVR_Teleporter::SpawnMarker(USceneComponent* TargettingSource, float Distance, bool FixedRotation, bool AtFloor, UParticleSystem* UseThisParticleSystem, UStaticMesh* UseThisStaticMesh ) {
	
	// Only spawn marker if not yet targetting 
	if (!IsTargetting && TargettingSource) {
		
		// Find start location
		MarkerLocation = TargettingSource->GetComponentTransform().GetLocation();
		
		// Set Rotation
		MarkerRotation = FRotator(TargettingSource->GetComponentTransform().GetRotation());

		// Set Marker to a fixed pitch if required
		if (FixedRotation) { 
			MarkerRotation.Roll = 0.0f;
			MarkerRotation.Pitch = 0.0f;
			IsMarkerRotationFixed = true;
		}

		// Record Distance (for re-spawns)
		RespawnDistance = Distance;

		// Calculate target location
		MarkerLocation = FVector(MarkerLocation + (MarkerRotation.Vector() * Distance));

		// Set Marker to floor if required
		if (AtFloor) {
			MarkerLocation.Z = 0.0f;
			IsMarkerAtFloor = true;
		}

		// Move to target location
		// SetWorldLocation(MarkerLocation);


		// Activate Particle System if available
		if (UseThisParticleSystem) {
			ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(this, UseThisParticleSystem, MarkerLocation, MarkerRotation);
		}


		// Show Static Mesh if available
		if (UseThisStaticMesh) {

			// Create new static mesh component and attach to actor
			StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
			StaticMeshComponent->RegisterComponentWithWorld(GetWorld());
			StaticMeshComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			
			//Set Mesh
			StaticMeshComponent->SetStaticMesh(UseThisStaticMesh);

			// Place the static mesh component in the world
			StaticMeshComponent->SetRelativeLocationAndRotation(MarkerLocation, MarkerRotation);

		}

		// Activate targetting
		IsTargetting = true;
	}
}

// Remove Marker
void URunebergVR_Teleporter::RemoveMarker()
{

	// Destroy Particle System if available
	if (ParticleSystemComponent) {
		ParticleSystemComponent->DestroyComponent();
		ParticleSystemComponent = nullptr;
	}

	// Destroy Static Mesh if available
	if (StaticMeshComponent) {
		StaticMeshComponent->DestroyComponent();
		StaticMeshComponent = nullptr;
	}

	// Disable targetting
	IsTargetting = false;
}

// Move marker
void URunebergVR_Teleporter::MoveMarker(USceneComponent* TargettingSource, bool MoveForward, bool MoveRight, bool MoveBack, bool MoveLeft, int Rate)
{
	// Only move marker if it is visible and active
	if (IsTargetting) {

		FRotator OriginalMarkerRotation = MarkerRotation;

		if (TargettingSource) {
			// Reset marker rotation
			MarkerRotation = FRotator(TargettingSource->GetRelativeTransform().GetRotation());

			// Set original marker fixedpitch values if set
			if (IsMarkerRotationFixed) {
				MarkerRotation.Roll = 0.0f;
				MarkerRotation.Pitch = 0.0f;
			}
		} else {
			
		}

		// MOVE FORWARD
		if (MoveForward) {

			// Calculate target location
			MarkerLocation = FVector(MarkerLocation + (MarkerRotation.Vector() * Rate));

			// Set Marker to floor if required
			if (IsMarkerAtFloor) {
				MarkerLocation.Z = 0.0f;
				IsMarkerAtFloor = true;
			}

			// Move visible indicators to target location if available
			if (ParticleSystemComponent) {
				ParticleSystemComponent->SetWorldLocation(MarkerLocation);
			}

			if (StaticMeshComponent) {
				StaticMeshComponent->SetWorldLocation(MarkerLocation);
			}

		}

		// MOVE BACK
		if (MoveBack) {

			// Calculate target location 
			MarkerLocation = MarkerLocation + (MarkerRotation.Vector() * -Rate);

			// Set Marker to floor if required
			if (IsMarkerAtFloor) {
				MarkerLocation.Z = 0.0f;
				IsMarkerAtFloor = true;
			}

			// Move visible indicators to target location if available
			if (ParticleSystemComponent) {
				ParticleSystemComponent->SetWorldLocation(MarkerLocation);
			}

			if (StaticMeshComponent) {
				StaticMeshComponent->SetWorldLocation(MarkerLocation);
			}

		}

		// MOVE RIGHT
		if (MoveRight) {

			// Tilt original marker location to point Eastwards
			OriginalMarkerRotation.Yaw += 90.0f;

			// Calculate target location 
			MarkerLocation = MarkerLocation + (OriginalMarkerRotation.Vector() * Rate);

			// Move visible indicators to target location if available
			if (ParticleSystemComponent) {
				ParticleSystemComponent->SetWorldLocation(MarkerLocation);
			}

			if (StaticMeshComponent) {
				StaticMeshComponent->SetWorldLocation(MarkerLocation);
			}
		}

		// MOVE LEFT
		if (MoveLeft) {

			// Tilt original marker location to point Eastwards
			OriginalMarkerRotation.Yaw += 90.0f;

			// Calculate target location 
			MarkerLocation = MarkerLocation + (OriginalMarkerRotation.Vector() * -Rate);

			// Move visible indicators to target location if available
			if (ParticleSystemComponent) {
				ParticleSystemComponent->SetWorldLocation(MarkerLocation);
			}

			if (StaticMeshComponent) {
				StaticMeshComponent->SetWorldLocation(MarkerLocation);
			}
		}

	}
}