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
	static const FName HMDName = GEngine->HMDDevice->GetDeviceName();

	if (GEngine->HMDDevice.IsValid())
	{
		// Override height offset for Oculus Rift
		if (HMDName == FName(TEXT("OculusRift")))
		{
			this->SetActorLocation(FVector(0.f, 0.f, this->GetActorLocation().Z + 150.f));
		}
	}
}

// Override all default pawn values
void ARunebergVR_Pawn::OverridePawnValues(float PawnBaseEyeHeight, float CapsuleHalfHeight, float CapsuleRadius, 
	FVector CapsuleRelativeLocation, FVector SceneLocation, FVector LeftControllerLocation, FVector RightControllerLocation) 
{
	// Set Pawn base eye hegiht
	this->BaseEyeHeight = PawnBaseEyeHeight;

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