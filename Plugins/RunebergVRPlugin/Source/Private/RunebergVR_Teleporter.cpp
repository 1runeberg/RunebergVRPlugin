// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_Teleporter.h"
#include "Kismet/KismetMathLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IHeadMountedDisplay.h"

// Sets default values for this component's properties
URunebergVR_Teleporter::URunebergVR_Teleporter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Auto Activate this component
	bAutoActivate = true;

}

// Called when the game starts
void URunebergVR_Teleporter::BeginPlay()
{
	Super::BeginPlay();
	
	// Ensure target marker is not visible at start
	SetVisibility(false, true);

	// Set default object type for the teleport arc to recognize as a boundary if none was provided
	if (TeleportBoundary_ObjectTypes.Num() < 1)
	{
		TeleportBoundary_ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);	// World static objects
		//ArcTeleportBoundary_ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2);	// Dynamic objects 
	}

	// Create teleport arc spline
	ArcSpline = NewObject<USplineComponent>(this);
	ArcSpline->RegisterComponentWithWorld(GetWorld());
	ArcSpline->SetMobility(EComponentMobility::Movable);
	ArcSpline->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	if (UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().IsEqual(TEXT("OculusRift"), ENameCase::IgnoreCase, true))
	{
		PawnHeightOffset.Z = OculusHeightOffset;
	}
	else
	{
		PawnHeightOffset.Z = SteamVRHeightOffset;
	}

}

// Called every frame
void URunebergVR_Teleporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsTeleporting && bIsBeamTypeTeleport)
	{
		if (TeleportMode == 0)
		{
			DrawTeleportArc();
		}
		else if (TeleportMode == 1)
		{
			DrawTeleportRay();
		}
	}
}

// Draw Teleport Arc
void URunebergVR_Teleporter::DrawTeleportArc()
{
	// Set Teleport Arc Parameters
	FPredictProjectilePathParams Params = FPredictProjectilePathParams(
		ArcRadius,
		FVector(this->GetComponentLocation().X + BeamLocationOffset.X,
			this->GetComponentLocation().Y + BeamLocationOffset.Y,
			this->GetComponentLocation().Z + BeamLocationOffset.Z),
		this->GetForwardVector() * BeamMagnitude,
		MaxSimTime);
	Params.bTraceWithCollision = true;
	Params.bTraceComplex = false;
	Params.DrawDebugType = EDrawDebugTrace::None;
	Params.DrawDebugTime = 0.f;
	Params.SimFrequency = SimFrequency;
	Params.ObjectTypes = TeleportBoundary_ObjectTypes;
	Params.OverrideGravityZ = ArcOverrideGravity;
	Params.bTraceWithChannel = false;

	// Do the arc trace
	FPredictProjectilePathResult PredictResult;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, PredictResult);

	// Show Target Marker (if a valid teleport location)
	if (bHit)
	{
		bool bIsWithinNavBounds = GetWorld()->GetNavigationSystem()->K2_ProjectPointToNavigation(
			this,
			PredictResult.HitResult.Location,
			TargetLocation,
			(ANavigationData*)0, 0,
			BeamHitNavMeshTolerance);

		// Check if arc hit location is within the nav mesh
		if (bIsWithinNavBounds)
		{
			// Set Marker location
			TargetLocation = PredictResult.HitResult.Location;
			
			// Check marker rotation
			if (CustomMarkerRotation.Equals(FRotator::ZeroRotator))
			{
				TargetRotation = CustomMarkerRotation;
			}
			else
			{
				TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, GetOwner()->GetActorLocation());
			}
			
			// Apply marker position and orientation
			SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);

			// Set Target Marker Visibility
			SetTargetMarkerVisibility(true);
			bIsTargetLocationValid = true;
		}
		else
		{
			// Set Target Marker Visibility
			SetTargetMarkerVisibility(false);
			bIsTargetLocationValid = false;
		}
	}
	else
	{
		// Set Target Marker Visibility
		SetTargetMarkerVisibility(false);
		bIsTargetLocationValid = false;
	}


	// Set the teleport arc points
	if (ArcSpline)
	{
		// Clean-up old Spline
		ClearTeleportArc();

		// Set the point type for the curve
		ArcSpline->SetSplinePointType(ArcPoints.Num() - 1, ESplinePointType::CurveClamped, true);

		for (const FPredictProjectilePathPointData& PathPoint : PredictResult.PathData)
		{
			// Add the point to the arc spline
			ArcPoints.Add(PathPoint.Location);
			ArcSpline->AddSplinePoint(PathPoint.Location, ESplineCoordinateSpace::Local, true);
		}
	}

	// Populate arc points with meshes
	if (TeleportBeamMesh)
	{
		for (int32 i = 0; i < ArcPoints.Num() - 2; i++)
		{
			// Add the arc mesh
			USplineMeshComponent* ArcMesh = NewObject<USplineMeshComponent>(ArcSpline);
			ArcMesh->RegisterComponentWithWorld(GetWorld());
			ArcMesh->SetMobility(EComponentMobility::Movable);
			//ArcMesh->AttachToComponent(ArcSpline, FAttachmentTransformRules::KeepRelativeTransform);
			ArcMesh->SetStaticMesh(TeleportBeamMesh);
			ArcSplineMeshes.Add(ArcMesh);

			// Bend mesh to conform to arc
			ArcMesh->SetStartAndEnd(ArcPoints[i],
				ArcSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),
				ArcPoints[i + 1],
				ArcSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local),
				true);
		}
	}

}

// Clear Teleport arc
void URunebergVR_Teleporter::ClearTeleportArc()
{
	// Clear Arc
	ArcPoints.Empty();
	ArcSpline->ClearSplinePoints();

	for (int32 i = 0; i < ArcSplineMeshes.Num(); i++)
	{
		if (ArcSplineMeshes[i])
		{
			ArcSplineMeshes[i]->DestroyComponent();
		}
	}

	ArcSplineMeshes.Empty();
}

// Show the teleportation arc trace
bool URunebergVR_Teleporter::ShowTeleportArc()
{
	if (!IsTeleporting)
	{
		TeleportMode = 0;
		IsTeleporting = true;
		bIsBeamTypeTeleport = true;
		SpawnTargetMarker();
		return true;
	}

	return false;
}

// Remove the teleportation arc trace
bool URunebergVR_Teleporter::HideTeleportArc()
{
	if (IsTeleporting)
	{
		// Get last teleport location status
		bool bValidTeleportLoc = bIsTargetLocationValid;

		// Clear teleport vars
		TeleportMode = -1;
		IsTeleporting = false;
		bIsBeamTypeTeleport = false;
		ClearTeleportArc();

		// Clear Target Marker
		RemoveTargetMarker();

		return bValidTeleportLoc;
	}

	return false;
}

// Remove the teleportation ray trace
bool URunebergVR_Teleporter::HideTeleportRay()
{
	if (IsTeleporting)
	{
		TeleportMode = -1;
		IsTeleporting = false;
		bIsBeamTypeTeleport = false;
		ClearTeleportRay();
		RayMeshScale = FVector(1.0f, 1.0f, 1.0f);

		// Clear Target Marker
		RemoveTargetMarker();

		return true;
	}

	return false;
}

// Clear Teleport ray
void URunebergVR_Teleporter::ClearTeleportRay()
{
	if (RayMesh)
	{
		// Remove ray mesh component
		RayMesh->DestroyComponent();
		RayMesh = nullptr;
	}
}

// Draw Teleport Ray
void URunebergVR_Teleporter::DrawTeleportRay()
{

	// Setup ray trace
	FCollisionQueryParams Ray_TraceParams(FName(TEXT("Ray_Trace")), true, this->GetOwner());
	Ray_TraceParams.bTraceComplex = true;
	Ray_TraceParams.bTraceAsyncScene = true;
	Ray_TraceParams.bReturnPhysicalMaterial = false;

	// Initialize Hit Result var
	FHitResult Ray_Hit(ForceInit);

	// Get Target Location
	TargetLocation = FVector(this->GetComponentLocation().X + BeamLocationOffset.X,
		this->GetComponentLocation().Y + BeamLocationOffset.Y,
		this->GetComponentLocation().Z + BeamLocationOffset.Z) +
		(this->GetComponentRotation().Vector() * BeamMagnitude);

	// Do the ray trace
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Ray_Hit,
		this->GetComponentLocation(),
		FVector(this->GetComponentLocation().X + BeamLocationOffset.X,
			this->GetComponentLocation().Y + BeamLocationOffset.Y,
			this->GetComponentLocation().Z + BeamLocationOffset.Z) +
			(this->GetComponentRotation().Vector() * BeamMagnitude),
		TeleportBoundary_ObjectTypes,
		Ray_TraceParams
	);


	// Reset Target Marker
	SetTargetMarkerVisibility(false);
	bIsTargetLocationValid = false;

	// Check if we hit a possible location to teleport to
	if (bHit)
	{
		// Check if target location is within the nav mesh
		FVector tempTargetLocation;
		bool bIsWithinNavBounds = GetWorld()->GetNavigationSystem()->K2_ProjectPointToNavigation(
			this,
			Ray_Hit.ImpactPoint,
			tempTargetLocation,
			(ANavigationData*)0, 0,
			BeamHitNavMeshTolerance);

		if (bIsWithinNavBounds)
		{
			// Set Target Marker Visibility
			TargetLocation = Ray_Hit.ImpactPoint;
			
			// Check marker rotation
			if (CustomMarkerRotation.Equals(FRotator::ZeroRotator))
			{
				TargetRotation = CustomMarkerRotation;
			}
			else
			{
				TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, GetOwner()->GetActorLocation());
			}

			SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);
			SetTargetMarkerVisibility(true);
			bIsTargetLocationValid = true;
		}
	}

	// Draw ray mesh
	ClearTeleportRay();
	if (TeleportBeamMesh)
	{
		// Spawn the beam mesh
		RayMesh = NewObject<UStaticMeshComponent>(this);
		RayMesh->RegisterComponentWithWorld(GetWorld());
		RayMesh->SetMobility(EComponentMobility::Movable);
		RayMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		RayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RayMesh->SetStaticMesh(TeleportBeamMesh);
		RayMesh->AddLocalOffset(BeamLocationOffset);
		RayMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(FVector(this->GetComponentLocation().X + BeamLocationOffset.X,
			this->GetComponentLocation().Y + BeamLocationOffset.Y,
			this->GetComponentLocation().Z + BeamLocationOffset.Z), TargetLocation));

		// Scale the beam mesh
		if (RayInstantScale)
		{
			// Calculate how long the beam should be using RayScaleRate as the base unit
			RayMeshScale = FVector(FVector::Distance(GetComponentLocation(), TargetLocation) * RayScaleRate, 1.f, 1.f);
			RayMesh->SetWorldScale3D(RayMeshScale);
		}
		else
		{
			// Scale beam mesh gradually until it reaches the target location
			RayDistanceToTarget = FVector::Distance(GetComponentLocation(), TargetLocation);
			RayNumOfTimesToScale = RayDistanceToTarget;
			if (RayNumOfTimesToScale_Actual < RayNumOfTimesToScale)
			{
				// We haven't reached the target location yet, set the mesh scale
				RayMesh->SetWorldScale3D(RayMeshScale);
				RayMeshScale.X = RayMeshScale.X + RayScaleRate;

				// Update temp scale variables
				RayMeshScale_Max = RayMeshScale;
				RayNumOfTimesToScale_Actual += RayScaleRate;
			}
			else
			{
				// Scale mesh to max possible size to hit target location
				RayMesh->SetWorldScale3D(RayMeshScale_Max);
			}
		}
	}
}

// Show the teleportation ray trace
bool URunebergVR_Teleporter::ShowTeleportRay()
{
	if (!IsTeleporting)
	{
		TeleportMode = 1;
		IsTeleporting = true;
		bIsBeamTypeTeleport = true;
		SpawnTargetMarker();
		RayNumOfTimesToScale_Actual = 0.f;

		return true;
	}

	return false;
}

// Teleport object
bool URunebergVR_Teleporter::TeleportNow(FWorldFadeSettings FadeOutOptions, FWorldFadeSettings FadeInOptions, bool ForceTeleport, bool TeleportPhysics)
{
	// Only teleport if targetting is enabled
	if (ForceTeleport || (IsTeleporting && bIsTargetLocationValid)) {

		// Get HMD Position & Orientation
		FRotator HMDRotation;
		FVector HMDLocation;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);

		// Calculate target location
		if (TargetParticleSystemComponent)
		{
			TeleportTargetLoc = TargetParticleSystemComponent->GetComponentTransform().GetLocation();
		}

		if (TeleportTargetMesh)
		{
			TeleportTargetLoc = TargetStaticMeshComponent->GetComponentTransform().GetLocation();
		}

		// Setup camera offsets
		FVector TeleportCameraOffset = HMDLocation;
		APawn* RootPawn = Cast<APawn>(GetOwner());
		UCameraComponent* PawnCamera = nullptr;

		// Use camera vs pawn location as an accurate offset over the HMD location
		if (RootPawn && RootPawn->IsValidLowLevel())
		{
			PawnCamera = RootPawn->FindComponentByClass<UCameraComponent>();

			if (PawnCamera)
			{
				TeleportCameraOffset = PawnCamera->GetComponentLocation() - GetOwner()->GetActorLocation();
			}
		}
		
		// Location Troubleshooting
		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Current Pawn Location (pre-teleport): %s"), *GetOwner()->GetActorLocation().ToString());
		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Current HMD Location: %s"), *HMDLocation.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Current Camera World Location: %s"), *PawnCamera->GetComponentLocation().ToString());
		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Current Camera Relative Location: %s"), *PawnCamera->GetRelativeTransform().GetLocation().ToString());
		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Unadjusted Target Location is: %s"), *TeleportTargetLoc.ToString());

		// Set target location adjustments
		TeleportTargetLoc = FVector(TeleportTargetLoc.X - TeleportCameraOffset.X, TeleportTargetLoc.Y - TeleportCameraOffset.Y, TeleportTargetLoc.Z);
		TeleportTargetLoc = FVector(TeleportTargetLoc.X + PawnHeightOffset.X + TeleportTargetPawnSpawnOffset.X,
			TeleportTargetLoc.Y + PawnHeightOffset.Y + TeleportTargetPawnSpawnOffset.Y,
			TeleportTargetLoc.Z + PawnHeightOffset.Z + TeleportTargetPawnSpawnOffset.Z);

		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Adjusted Target Location is: %s"), *TeleportTargetLoc.ToString());

		// Apply Custom Rotation if needed
		if (bFaceMarkerRotation)
		{
			TeleportTargetLoc = CustomMarkerRotation.RotateVector(TeleportTargetLoc);
		}
		
		// Set Target Physics
		TeleportTargetPhys = TeleportPhysics;

		// Check if we need to fade out
		if (FadeOutOptions.bDoWorldFade && FadeOutOptions.FadeDuration > 0.001f)
		{
			// Teleport pawn
			if (FadeOutTeleportOffset > 0.f && FadeOutOptions.FadeDuration > FadeOutTeleportOffset)
			{
				// Apply teleport offset time
				GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &URunebergVR_Teleporter::OnTeleport, FadeOutOptions.FadeDuration - FadeOutTeleportOffset, false);
			}
			else if (FadeOutTeleportOffset < 0.f)
			{
				// Immediately teleport
				this->GetOwner()->SetActorLocation(TeleportTargetLoc, false, nullptr, TeleportPhysics ? ETeleportType::TeleportPhysics : ETeleportType::None);

			}
			else
			{
				// Wait for fade out before teleporting
				GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &URunebergVR_Teleporter::OnTeleport, FadeOutOptions.FadeDuration, false);
			}

			// Start Fade out - hold fade if there's an active fade in effect
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(FadeOutOptions.FromOpacity, FadeOutOptions.ToOpacity, FadeOutOptions.FadeDuration,
				FadeOutOptions.FadeColor, FadeOutOptions.bShouldFadeAudio, FadeInOptions.bDoWorldFade);
		}
		else 
		{
			// No effective fade out, instantly teleport pawn
			this->GetOwner()->SetActorLocation(TeleportTargetLoc, false, nullptr, TeleportPhysics ? ETeleportType::TeleportPhysics : ETeleportType::None);
		}

		// Check if we need to fade in
		if (FadeInOptions.bDoWorldFade)
		{
			// Start fade in
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(FadeInOptions.FromOpacity, FadeInOptions.ToOpacity, FadeInOptions.FadeDuration,
				FadeInOptions.FadeColor, FadeInOptions.bShouldFadeAudio, false);
		}

		// Remove teleport artifacts
		switch (TeleportMode)
		{
		case 0:
			HideTeleportArc();
			break;

		case 1:
			HideTeleportRay();
			break;

		case 2:
			HideMarker();
			break;

		default:
			break;
		}

		// Reset Teleport mode
		TeleportMode = -1;

		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Current Pawn Location (post-teleport): %s"), *GetOwner()->GetActorLocation().ToString());

		return true;

	}

	return false;
}

void URunebergVR_Teleporter::OnTeleport() 
{
	// Teleport Pawn
	this->GetOwner()->SetActorLocation(TeleportTargetLoc, false, nullptr, TeleportTargetPhys ? ETeleportType::TeleportPhysics : ETeleportType::None);
	
	// Clear Fade Timer Handle
	GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
}

// Show the teleport target marker
bool URunebergVR_Teleporter::ShowMarker()
{
	if (!IsTeleporting)
	{
		// Calculate Target Location
		TargetLocation = this->GetComponentLocation() + (this->GetComponentRotation().Vector() * BeamMagnitude);

		// Check if target location is within the nav mesh
		FVector tempTargetLocation;
		bool bIsWithinNavBounds = GetWorld()->GetNavigationSystem()->K2_ProjectPointToNavigation(
			this,
			TargetLocation,
			tempTargetLocation,
			(ANavigationData*)0, 0,
			BeamHitNavMeshTolerance);

		if (bIsWithinNavBounds)
		{
			// Set Target Marker Visibility
			TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, GetOwner()->GetActorLocation());
			SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);
			SetTargetMarkerVisibility(true);
			bIsTargetLocationValid = true;
		}
		else
		{
			return false;
		}

		// Set teleport parameters
		TeleportMode = 2;
		IsTeleporting = true;
		bIsBeamTypeTeleport = false;
		bIsTargetLocationValid = true;

		// Show target marker
		SpawnTargetMarker();
		TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetLocation, GetOwner()->GetActorLocation());
		TargetLocation.Z = FloorIsAtZ;

		// Calculate Rotation of marker to face player and set the new transform
		SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);

		// Make target marker visible
		SetTargetMarkerVisibility(true);

		return true;
	}

	return false;
}

// Remove Marker
bool URunebergVR_Teleporter::HideMarker()
{
	if (IsTeleporting)
	{
		TeleportMode = -1;
		IsTeleporting = false;
		bIsBeamTypeTeleport = false;
		bIsTargetLocationValid = false;

		// Clear Target Marker
		RemoveTargetMarker();

		return true;
	}

	return false;
}

// Move marker
bool URunebergVR_Teleporter::MoveMarker(EMoveDirectionEnum MarkerDirection, int Rate, FRotator CustomDirection)
{
	// Only move marker if it is visible and active
	if (IsTeleporting) {

		switch (MarkerDirection)
		{
		case  EMoveDirectionEnum::MOVE_FORWARD:
			TargetLocation = FVector(TargetLocation + (TargetRotation.Vector() * Rate));
			TargetLocation.Z = FloorIsAtZ;
			SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);
			break;

		case  EMoveDirectionEnum::MOVE_BACKWARD:
			TargetLocation = TargetLocation + (TargetRotation.Vector() * -Rate);
			TargetLocation.Z = FloorIsAtZ;
			SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);
			break;

		case  EMoveDirectionEnum::MOVE_LEFT:
			// Tilt original marker location to point Westwards
			CustomDirection = TargetRotation;
			CustomDirection.Yaw += 90.0f;

			// Calculate target location 
			TargetLocation = TargetLocation + (CustomDirection.Vector() * Rate);
			TargetLocation.Z = FloorIsAtZ;
			SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);
			break;

		case  EMoveDirectionEnum::MOVE_RIGHT:
			// Tilt original marker location to point Eastwards
			CustomDirection = TargetRotation;
			CustomDirection.Yaw += 90.0f;

			// Calculate target location 
			TargetLocation = TargetLocation + (CustomDirection.Vector() * -Rate);
			TargetLocation.Z = FloorIsAtZ;
			SetTargetMarkerLocationAndRotation(TargetLocation, FRotator::ZeroRotator);
			break;

		case  EMoveDirectionEnum::MOVE_CUSTOM:
			TargetLocation = FVector(TargetLocation + (CustomDirection.Vector() * Rate));
			TargetLocation.Z = FloorIsAtZ;
			SetTargetMarkerLocationAndRotation(TargetLocation, TargetRotation);
			break;

		default:
			break;
		}

		return true;
	}

	return false;
}

// Show target location marker
void URunebergVR_Teleporter::SpawnTargetMarker(FVector MarkerLocation, FRotator MarkerRotation)
{
	// Activate Particle System if available
	if (TeleportTargetParticle) {
		TargetParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(this, TeleportTargetParticle, MarkerLocation, MarkerRotation);
		TargetParticleSystemComponent->SetWorldScale3D(TeleportTargetParticleScale);
		TargetParticleSystemComponent->SetVisibility(false);
		TargetParticleSystemComponent->SetMobility(EComponentMobility::Movable);

		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Current Particle Marker Transform is: %s"), *TargetParticleSystemComponent->GetComponentTransform().ToString());
	}

	// Show Static Mesh if available
	if (TeleportTargetMesh) {
		// Create new static mesh component and attach to actor
		TargetStaticMeshComponent = NewObject<UStaticMeshComponent>(this);
		TargetStaticMeshComponent->RegisterComponentWithWorld(GetWorld());
		TargetStaticMeshComponent->SetSimulatePhysics(false);
		TargetStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TargetStaticMeshComponent->SetMobility(EComponentMobility::Movable);
		TargetStaticMeshComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		//Set Mesh
		TargetStaticMeshComponent->SetVisibility(false);
		TargetStaticMeshComponent->SetWorldScale3D(TeleportTargetMeshScale);
		TargetStaticMeshComponent->SetStaticMesh(TeleportTargetMesh);

		//UE_LOG(LogTemp, Warning, TEXT("[TELEPORT] Current Static Mesh Marker Transform is: %s"), *TargetStaticMeshComponent->GetComponentTransform().ToString());
	}
}

// Remove target location marker
void URunebergVR_Teleporter::RemoveTargetMarker()
{
	// Destroy Particle System if available
	if (TargetParticleSystemComponent) {
		TargetParticleSystemComponent->DestroyComponent();
		TargetParticleSystemComponent = nullptr;
	}

	// Destroy Static Mesh if available
	if (TargetStaticMeshComponent) {
		TargetStaticMeshComponent->DestroyComponent();
		TargetStaticMeshComponent = nullptr;
	}

	bIsTargetLocationValid = false;
	CustomMarkerRotation = FRotator::ZeroRotator;
}

// Show target location marker
void  URunebergVR_Teleporter::SetTargetMarkerVisibility(bool MakeVisible)
{
	// Activate Particle System if available
	if (TargetParticleSystemComponent) {
		TargetParticleSystemComponent->SetVisibility(MakeVisible);
	}

	// Show Static Mesh if available
	if (TargetStaticMeshComponent) {
		TargetStaticMeshComponent->SetVisibility(MakeVisible);
	}
}

// Move target location marker
void  URunebergVR_Teleporter::SetTargetMarkerLocationAndRotation(FVector MarkerLocation, FRotator MarkerRotation)
{
	// Activate Particle System if available
	if (TargetParticleSystemComponent) {
		TargetParticleSystemComponent->SetWorldLocation(MarkerLocation + TeleportTargetParticleSpawnOffset);
		TargetParticleSystemComponent->SetWorldRotation(MarkerRotation);
	}

	// Show Static Mesh if available
	if (TargetStaticMeshComponent) {
		TargetStaticMeshComponent->SetWorldLocation(MarkerLocation + TeleportTargetMeshSpawnOffset);
		TargetStaticMeshComponent->SetWorldRotation(MarkerRotation);
	}
}
