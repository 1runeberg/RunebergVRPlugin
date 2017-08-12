// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVRPlugin.h"
#include "RunebergVR_Pawn.h"
#include "IHeadMountedDisplay.h"

// Sets default values
ARunebergVR_Pawn::ARunebergVR_Pawn(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Base Eye Height
	this->BaseEyeHeight = 0.f;

	// Set root scene component - use static mesh to ensure collisions
	RootComponent = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("SceneRoot"));

	// Add Scene component (for headset positioning), set to -110 to ensure headset starts at floor
	Scene = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("VRBaseScene"));
	Scene->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
	Scene->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Add camera
	Camera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	Camera->AttachToComponent(Scene, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->SetFieldOfView(110.f);

	// Add Capsule Collission, set default VR half height and radius values
	CapsuleCollision = PCIP.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleCollision"));
	CapsuleCollision->SetCapsuleHalfHeight(96.f);
	CapsuleCollision->SetCapsuleRadius(22.f);
	CapsuleCollision->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
	CapsuleCollision->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);

	// Add Motion Controllers
	MotionController_Left = PCIP.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("MotionController_Left"));
	MotionController_Left->Hand = EControllerHand::Left;
	MotionController_Left->AttachToComponent(Scene, FAttachmentTransformRules::KeepRelativeTransform);
	MotionController_Left->SetRelativeLocation(FVector(0.f, 0.f, 110.f));

	MotionController_Right = PCIP.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("MotionController_Right"));
	MotionController_Right->Hand = EControllerHand::Right;
	MotionController_Right->AttachToComponent(Scene, FAttachmentTransformRules::KeepRelativeTransform);
	MotionController_Right->SetRelativeLocation(FVector(0.f, 0.f, 110.f));

}

// Called when the game starts
void ARunebergVR_Pawn::BeginPlay()
{
	Super::BeginPlay();

	// Adjust pawn spawn target offset based on HMD
	if (GEngine->HMDDevice.IsValid())
	{

		// Override height offset for Oculus Rift
		switch (GEngine->HMDDevice->GetHMDDeviceType())
		{
		case EHMDDeviceType::DT_OculusRift:
			HMDLocationOffset = OculusLocationOffset;   // This ensure we use the Oculus location offset for uneven terrain calculations
			this->SetActorLocation(this->GetActorLocation() + OculusLocationOffset);
			GEngine->HMDDevice->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
			break;
		default:
			break;
		}

		// Set tracking origin (Oculus & Vive)
		GEngine->HMDDevice->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}

	// Set Current Gravity Strength
	CurrentGravityStrength = GravityVariables.GravityStrength;
}

// Called every frame
void ARunebergVR_Pawn::Tick(float DeltaTime)
{ 

	Super::Tick(DeltaTime);

	// Get the current position of the Camera
	FVector CameraPosition = Camera->GetComponentTransform().GetLocation();

	// Apply gravity if enabled and camera is positioned at the head of the player
	if (EnableGravity && Camera->IsValidLowLevel() && CameraPosition.Z > this->GetActorLocation().Z)
	{
		// Set line trace for gravity variables
		FHitResult RayHit(EForceInit::ForceInit);
		FCollisionQueryParams RayTraceParams(FName(TEXT("GravityRayTrace")), true, this->GetOwner());

		// Initialize Gravity Trace Hit Result var
		RayTraceParams.bTraceComplex = true;
		RayTraceParams.bTraceAsyncScene = true;
		RayTraceParams.bReturnPhysicalMaterial = false;
		
		// Do a line trace and check for a component that can be stepped on
		bHit = GetWorld()->LineTraceSingleByChannel(RayHit, CameraPosition, CameraPosition + FVector(0.f, 0.f, FMath::Abs(GravityVariables.FloorTraceRange) * -1.f),
			ECollisionChannel::ECC_Visibility, RayTraceParams);

		// Check if we need to float the Pawn over uneven terrain
		if (GravityVariables.RespondToUnevenTerrain
			&& bHit && RayHit.GetComponent()->CanCharacterStepUpOn == ECanBeCharacterBase::ECB_Yes
			&& (RayHit.Distance + GravityVariables.FloorTraceTolerance) < GravityVariables.FloorTraceRange)
		{
			int Steps = FMath::Round(GravityVariables.FloorTraceTolerance / StepUpRate);
			for (int32 i = Steps; i > 1; i--)
			{
				this->TeleportTo(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, RayHit.Location.Z - (StepUpRate * i) + HMDLocationOffset.Z), this->GetActorRotation());
			}
		} 
		
		// Apply gravity
		if (!bHit || RayHit.GetComponent()->CanCharacterStepUpOn != ECanBeCharacterBase::ECB_Yes)
		{
			// Calculate gravity with acceleration and apply to the pawn
			CurrentGravityStrength = CurrentGravityStrength * GravityVariables.Acceleration;
			this->TeleportTo(this->GetActorLocation() + (GravityVariables.GravityDirection * CurrentGravityStrength), this->GetActorRotation());
		}
		else 
		{
			// Reset current gravity
			CurrentGravityStrength = GravityVariables.GravityStrength;
		}

	}
}

// Override all default pawn values
void ARunebergVR_Pawn::OverridePawnValues(float PawnBaseEyeHeight, float FOV, float CapsuleHalfHeight, float CapsuleRadius, 
	FVector CapsuleRelativeLocation, FVector SceneLocation, FVector LeftControllerLocation, FVector RightControllerLocation) 
{
	// Set Pawn base eye hegiht
	this->BaseEyeHeight = PawnBaseEyeHeight;

	// Set Camera Field of View
	Camera->SetFieldOfView(FOV);

	// Set capsule collision settings
	CapsuleCollision->SetCapsuleHalfHeight(CapsuleHalfHeight);
	CapsuleCollision->SetCapsuleRadius(CapsuleRadius);
	CapsuleCollision->SetRelativeLocation(CapsuleRelativeLocation);

	// Set scene location
	Scene->SetRelativeLocation(SceneLocation);

	// Set motion controller location
	MotionController_Left->SetRelativeLocation(LeftControllerLocation);
	MotionController_Right->SetRelativeLocation(RightControllerLocation);
}

// Check if the HMD is worn
bool ARunebergVR_Pawn::IsHMDWorn()
{
	if (GEngine->HMDDevice.IsValid())
	{
		if (GEngine->HMDDevice->GetHMDWornState() == EHMDWornState::Worn)
		{
			return true;
		}
	}

	return false;
}


// Pawn Rotation - usefull for static mouse rotations during development
void ARunebergVR_Pawn::RotatePawn(float RotationRate, float XAxisInput, float YAxisInput)
{
	if (XAxisInput != 0.f)
	{
		this->AddActorLocalRotation(FRotator(0.f, XAxisInput * RotationRate, 0.f));
	}

	if (YAxisInput != 0.f)
	{
		this->AddActorLocalRotation(FRotator(0.f, YAxisInput * RotationRate, 0.f));
	}
}

// Print debug message
void ARunebergVR_Pawn::PrintDebugMessage(FString Message, bool OverwriteExisting, float Duration, FColor Color)
{
	int32 Key;

	if (OverwriteExisting)
	{
		Key = 0;
	}
	else
	{
		Key = -1;
	}

	GEngine->AddOnScreenDebugMessage(Key, Duration, Color, Message);
}