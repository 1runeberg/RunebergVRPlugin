// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "RunebergVR_Pawn.generated.h"

// Gravity settings
USTRUCT(BlueprintType)
struct FGravityVariables
{
	GENERATED_USTRUCT_BODY()

	/** Respond to uneven terrain - Gravity MUST also be enabled */
	UPROPERTY(EditAnywhere, Category = "VR")
	bool RespondToUnevenTerrain = false;

	/** How fast this VR Pawn will fall with gravity */
	UPROPERTY(EditAnywhere, Category = "VR")
	float GravityStrength = 3.f;

	/** How far should the check for a floor be */
	UPROPERTY(EditAnywhere, Category = "VR")
	float FloorTraceRange = 150.f;

	/* Minimum Z offset before terrain is considered uneven */
	UPROPERTY(EditAnywhere, Category = "VR")
	float FloorTraceTolerance = 3.f;

	/** Direction where this VR Pawn will "fall" */
	UPROPERTY(EditAnywhere, Category = "VR")
	FVector GravityDirection = FVector(0.f, 0.f, -1.f);

};


UCLASS()
class RUNEBERGVRPLUGIN_API ARunebergVR_Pawn : public APawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Sets default values for this pawn's properties
	ARunebergVR_Pawn(const class FObjectInitializer& PCIP);

	/** Enable gravity for this pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool EnableGravity = false;

	/** Gravity variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FGravityVariables GravityVariables;

	/** Oculus HMD Location Offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FVector OculusLocationOffset = FVector(0.f, 0.f, 150.f);

	// Capsule Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCapsuleComponent* CapsuleCollision;

	// Scene Component (for headset posiitoning)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	USceneComponent* Scene;

	// Pawn camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCameraComponent* Camera;

	// VR Motion Controllers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	class UMotionControllerComponent* MotionController_Left;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	class UMotionControllerComponent* MotionController_Right;

	// Override default pawn vr values
	UFUNCTION(BlueprintCallable, Category = "VR")
	void OverridePawnValues(float PawnBaseEyeHeight = 0.f, float FOV = 110.f, float CapsuleHalfHeight = 96.f, float CapsuleRadius = 22.f, 
		FVector CapsuleRelativeLocation = FVector(0.f, 0.f, -110.f),
		FVector SceneLocation = FVector(0.f, 0.f, -110.f), FVector LeftControllerLocation = FVector(0.f, 0.f, 110.f), 
		FVector RightControllerLocation = FVector(0.f, 0.f, 110.f));

	// Pawn Rotation - usefull for static mouse rotation during development
	UFUNCTION(BlueprintCallable, Category = "VR")
	void RotatePawn(float RotationRate = 1.f, float XAxisInput = 0.f, float YAxisInput = 0.f);

	// Check if HMD is worn
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool IsHMDWorn();

	// Print debug message
	UFUNCTION(BlueprintCallable, Category = "VR")
	void PrintDebugMessage(FString Message, bool OverwriteExisting = false, float Duration = 5.f, FColor Color = FColor::Red);

private:
	// Whether we have hit something with the line trace that can cause this pawn to stop falling if gravity is enabled
	bool bHit = false;
};
