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
#include "MotionControllerComponent.h"
#include "RunebergVR_Pawn.generated.h"

UCLASS()
class RUNEBERGVRPLUGIN_API ARunebergVR_Pawn : public APawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this pawn's properties
	ARunebergVR_Pawn(const class FObjectInitializer& PCIP);

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
	void OverridePawnValues(float PawnBaseEyeHeight = 0.f, float CapsuleHalfHeight = 96.f, float CapsuleRadius = 22.f, 
		FVector CapsuleRelativeLocation = FVector(0.f, 0.f, -110.f),
		FVector SceneLocation = FVector(0.f, 0.f, -110.f), FVector LeftControllerLocation = FVector(0.f, 0.f, 110.f), 
		FVector RightControllerLocation = FVector(0.f, 0.f, 110.f));

};
