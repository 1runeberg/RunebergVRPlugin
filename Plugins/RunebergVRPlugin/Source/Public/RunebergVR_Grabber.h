// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Components/ActorComponent.h"
#include "RunebergVR_Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_Grabber : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_Grabber();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Motion Controller Location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	FVector ControllerLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	FRotator ControllerRotation = FRotator::ZeroRotator;
	
	// Current Distance of grabbed items from their respective controllers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	float DistanceFromController = 10.0f;

	// Min & Max Distance for Controller for grabbed objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MinDistanceFromController = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MaxDistanceFromController = 250.0f;


	// Grab something within line trace range of controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Grab(float Reach, bool ShowDebugLine, bool SetLocationManually, FVector _ControllerLocation, FRotator _ControllerRotation);

	// Set distance from controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	void SetDistanceFromController(float NewDistance);

	// Set the location of the motion controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	void UpdateGrabbedObjectLocation(FVector _ControllerLocation, FRotator _ControllerRotation);

	// Release grabbed object
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Release();

	// Pull grabbed object 
	UFUNCTION(BlueprintCallable, Category = "VR")
	void PullGrabbedObject(int Speed = 1);

	// Pull grabbed object(s) 
	UFUNCTION(BlueprintCallable, Category = "VR")
	void PushGrabbedObject(int Speed = 1);

private:
	UInputComponent* InputComponent = nullptr;

	// Grabbed Actor
	UPhysicsHandleComponent* GrabbedObject = nullptr;

	AActor* GetHit(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine);	// Get Actor hit by line trace
	void AttemptGrab(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine);	// Attempt to Grab Object in line trace		
};
