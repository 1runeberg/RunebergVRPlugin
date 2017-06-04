// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RunebergVR_Teleporter.generated.h"


UENUM(BlueprintType)
enum class EMoveDirectionEnum : uint8
{
	MOVE_FORWARD 	UMETA(DisplayName = "Towards Player"),
	MOVE_BACKWARD 	UMETA(DisplayName = "Away from Player"),
	MOVE_LEFT		UMETA(DisplayName = "Left of Player"),
	MOVE_RIGHT		UMETA(DisplayName = "Right of Player"),
	MOVE_CUSTOM		UMETA(DisplayName = "Use a Custom Rotation for Direction")
};

UCLASS( ClassGroup=(VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_Teleporter : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_Teleporter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// The teleport beam's mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
	class UStaticMesh* TeleportBeamMesh = nullptr;

	/** The teleport beam's Launch Velocity Magnitude - higher number increases range of teleport */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
	float BeamMagnitude = 500.f;

	/** A location offset from the parent mesh origin where the teleport beam will start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
	FVector BeamLocationOffset = FVector::ZeroVector;

	/** For ray type beam, ensure the lenth of the beam reaches target location instantenously. Uses RayScaleRate as base length unit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
	bool RayInstantScale = true;

	/** How much the ray will scale up until it reaches target location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
	float RayScaleRate = 1.f;

	/** The teleport beam's navigation mesh tolerance - fine tune to fit your nav mesh bounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
	FVector BeamHitNavMeshTolerance = FVector(10.f, 10.f, 10.f);

	/** The teleport beam's custom gravity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
	float ArcOverrideGravity = 0.f;

	// The teleport target stuff

	/** Additional offset of pawn (internal offsets are Steam: 112, Rift: 250) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	FVector TeleportTargetPawnSpawnOffset = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	float FloorIsAtZ = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	class UStaticMesh* TeleportTargetMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	FVector TeleportTargetMeshScale = FVector(1.f, 1.f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	FVector TeleportTargetMeshSpawnOffset = FVector(0.f, 0.f, 5.f);

	/** Custom marker rotation (applied per frame) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	FRotator CustomMarkerRotation = FRotator::ZeroRotator;

	/** If player should face marker rotation (use with Custom Marker Rotation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	bool bFaceMarkerRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	class UParticleSystem* TeleportTargetParticle = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	FVector TeleportTargetParticleScale = FVector(1.f, 1.f, 1.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
	FVector TeleportTargetParticleSpawnOffset = FVector(0.f, 0.f, 0.f);

	/** Check to see if an active teleport mode is turned on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR - Read Only")
	bool IsTeleporting = false;

	// Show the teleportation arc trace
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool ShowTeleportArc();

	// Show the teleportation ray trace
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool ShowTeleportRay();

	// Remove the teleportation arc trace
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool HideTeleportArc();

	// Remove the teleportation ray trace
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool HideTeleportRay();

	// Show marker in the world
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool ShowMarker();

	// Move Marker
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool MoveMarker(EMoveDirectionEnum MarkerDirection = EMoveDirectionEnum::MOVE_FORWARD, int Rate = 25, FRotator CustomDirection = FRotator::ZeroRotator);

	// Remove marker
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool HideMarker();

	// Teleport
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool TeleportNow();

private:
	// Teleport target height offset - defaults to SteamVR
	FVector PawnHeightOffset = FVector(0.f, 0.f, 112.f);

	// Teleport targetting mode
	int TeleportMode = -1;

	// Teleport Arc constants
	const float ArcRadius = 0.f;
	const float MaxSimTime = 2.f;
	const float SimFrequency = 30.f;

	// Teleport Arc spline parameters
	USplineComponent* ArcSpline = nullptr;
	TArray<FVector> ArcPoints;
	TArray<USplineMeshComponent*> ArcSplineMeshes;
	TArray<TEnumAsByte<EObjectTypeQuery> > ArcObjectTypesToIgnore;
	FVector RayMeshScale = FVector(1.0f, 1.0f, 1.0f);
	FVector RayMeshScale_Max = FVector(1.0f, 1.0f, 1.0f);
	bool bIsBeamTypeTeleport = false;
	float RayNumOfTimesToScale = 0.f;
	float RayNumOfTimesToScale_Actual = 0.f;
	float RayDistanceToTarget = 0.f;
	

	// TeleportRay mesh
	UStaticMeshComponent* RayMesh = nullptr;

	// Teleport target location
	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;
	bool bIsTargetLocationValid = false;

	// Spawned visible components for targetting marker
	UParticleSystemComponent* TargetParticleSystemComponent = nullptr;
	UStaticMeshComponent* TargetStaticMeshComponent = nullptr;

	// Draw teleport arc
	void DrawTeleportArc();

	// Clear teleport arc spline
	void ClearTeleportArc();

	// Draw teleport ray
	void DrawTeleportRay();

	// Clear teleport arc spline
	void ClearTeleportRay();

	// Spawn target location marker
	void SpawnTargetMarker(FVector MarkerLocation = FVector::ZeroVector, FRotator MarkerRotation = FRotator::ZeroRotator);

	// Show target location marker
	void SetTargetMarkerVisibility(bool MakeVisible=false);

	// Show target location marker
	void SetTargetMarkerLocationAndRotation(FVector MarkerLocation = FVector::ZeroVector, FRotator MarkerRotation = FRotator::ZeroRotator);

	// Remove target location marker
	void RemoveTargetMarker();
};
