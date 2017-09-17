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
#include "RunebergVR_Movement.generated.h"


UCLASS( ClassGroup=(VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_Movement : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_Movement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Movement Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float CurrentMovementSpeed = 3.f;

	/** Current Target Rotation (current direction movement component is facing) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FRotator TargetRotation = FRotator::ZeroRotator;

	/** Offset Rotation (applied per frame) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FRotator OffsetRotation = FRotator::ZeroRotator;

	/** Movement Reference - Object that dictates the direction/rotation of the Pawn's movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	USceneComponent* CurrentMovementDirectionReference = nullptr;

	/** Navigation mesh tolerance (when used) - fine tune to fit your nav mesh bounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FVector NavMeshTolerance = FVector(10.f, 10.f, 10.f);

	/** Indicator if the pawn is moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool IsMoving = false;	

	/** Indicator if the pawn is bouncing back from a VR bounds collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool IsBouncingBackFromVRBounds = false;

	/** Whether this component will check for uneven terrain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool EnableTerrainCheck = false;

	// Enable VR Movement
	UFUNCTION(BlueprintCallable, Category = "VR")
	void EnableVRMovement(float MovementSpeed = 3.f, USceneComponent* MovementDirectionReference = nullptr, bool ObeyNavMesh = false,
		bool LockPitch = false, bool LockYaw = false, bool LockRoll = false, FRotator CustomDirection = FRotator::ZeroRotator);

	// Full 360 Movement
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Enable360Movement(USceneComponent* MovementDirectionReference = nullptr, bool LockPitch = false, bool LockYaw = false, bool LockRoll = false, float MovementSpeed = 3.f, float XAxisInput = 0.f, float YAxisInput = 0.f);

	// Disable VR Movement
	UFUNCTION(BlueprintCallable, Category = "VR")
	void DisableVRMovement();

	// Apply acceleration multiplier to current movement speed - can be used for smooth acceleration / deceleration
	UFUNCTION(BlueprintCallable, Category = "VR")
	void ApplySpeedMultiplier(float SpeedMultiplier = 2.f, float BaseSpeed = 3.f, bool UseCurrentSpeedAsBase = false);

	// Timed movement - move pawn for a specified amount of time
	UFUNCTION(BlueprintCallable, Category = "VR")
	void TimedMovement(float MovementDuration = 5.f, float MovementSpeed = 3.f, USceneComponent* MovementDirectionReference = nullptr, 
		bool LockPitchY = false, bool LockYawZ = false, bool LockRollX = false, FRotator CustomDirection = FRotator::ZeroRotator, bool ObeyNavMesh = false);

	// Dash move (timed)  - dash into a predefined direction and time
	UFUNCTION(BlueprintCallable, Category = "VR")
	void TimedDashMove(float MovementDuration = 5.f, float MovementSpeed = 3.f, FRotator MovementDirection = FRotator::ZeroRotator, bool ObeyNavMesh = false);

	// Bounce back from VR bounds
	UFUNCTION(BlueprintCallable, Category = "VR")
	void BounceBackFromVRBounds(float MovementSpeed = 3.f, float MovementDuration = 0.5f, bool ResetMovementStateAfterBounce=false);

private:
	AActor* VRPawn = nullptr;	// The VR Pawn that this component is attached to
	float BounceBackSpeed = 0.f;
	bool bObeyNavMesh = false;
	bool bLockPitchY = false;
	bool bLockYawZ = false;
	bool bLockRollX = false;
	bool bResetMovementStateAfterBounce = false;
	bool bIsMovingCache = false;

	// Move VR Pawn
	void MoveVRPawn(float MovementSpeed, USceneComponent* MovementDirectionReference, bool LockXAxis, bool LockYAxis, bool LockZAxis, FRotator CustomDirection);

	// Disable VR Bounds Bounce Back Movement
	void DisableVRBounceBack();

	// Whether we have hit something with the line trace that can cause this pawn to stop falling if gravity is enabled
	bool bHit = false;
};
