// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

#include "VR_CPP.h"
#include "VRPawnComponent_Targetter.h"

// Sets default values for this component's properties
UVRPawnComponent_Targetter::UVRPawnComponent_Targetter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;

	// Auto Activate this component
	bAutoActivate = true;

}


// Called when the game starts
void UVRPawnComponent_Targetter::BeginPlay()
{
	Super::BeginPlay();

	// Ensure target marker is not visible at start
	SetVisibility(false, true);
	
}


// Called every frame
void UVRPawnComponent_Targetter::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

// Teleport object
void UVRPawnComponent_Targetter::TeleportObject(AActor* ObjectToTeleport, USceneComponent* TargettingSource, bool ReSpawnMarker)
{
	// Only teleport if targetting is enabled
	if (IsTargetting && ObjectToTeleport) {
		
		// [UNCOMMENT IF NEEDED] Find closest appropriate place to teleport
		//GetWorld()->FindTeleportSpot(ObjectToTeleport, MarkerLocation, MarkerRotation);

		// Teleport object
		if (IsMarkerAtFloor) { 
			MarkerLocation.Z = 0.0f;
			ObjectToTeleport->SetActorLocationAndRotation(MarkerLocation, ObjectToTeleport->GetActorRotation()); 
		}
		else { 
			ObjectToTeleport->SetActorLocationAndRotation(MarkerLocation, ObjectToTeleport->GetActorRotation()); 
		}

		// Respawn marker forward
		if (ReSpawnMarker) {
			MoveMarker(TargettingSource, true, false, false, false, RespawnDistance);
		}
	}
}

// Spawn marker at given location
void UVRPawnComponent_Targetter::SpawnMarker(USceneComponent* TargettingSource, float Distance, bool FixedRotation, bool AtFloor, UParticleSystem* UseThisParticleSystem, UStaticMesh* UseThisStaticMesh ) {
	
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
			StaticMeshComponent->AttachTo(GetOwner()->GetRootComponent());
			
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
void UVRPawnComponent_Targetter::RemoveMarker() 
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
void UVRPawnComponent_Targetter::MoveMarker(USceneComponent* TargettingSource, bool MoveForward, bool MoveRight, bool MoveBack, bool MoveLeft, int Rate)
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