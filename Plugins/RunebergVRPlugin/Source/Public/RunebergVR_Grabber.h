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
#include "RunebergVR_Grabber.generated.h"

UENUM(BlueprintType)		
enum class EGrabTypeEnum : uint8
{
	PRECISION_GRAB 	UMETA(DisplayName = "Precision Grab"),
	SNAP_GRAB 		UMETA(DisplayName = "Snap to Mesh Origin Grab"),
	LOCK_GRAB		UMETA(DisplayName = "Locked Rotation Grab"),
	DANGLING_GRAB	UMETA(DisplayName = "Precision Grab and Dangle"),
	PRECISION_LOCK	UMETA(DisplayName = "Precision Grab with Locked Rotation")

};

UCLASS( ClassGroup=(VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_Grabber : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URunebergVR_Grabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Current Distance of grabbed items from their respective controllers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	float DistanceFromController = 10.0f;

	/** Min Distance for Controller for grabbed objects  - Customize Push & Pull functions mid action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MinDistanceFromController = 1.0f;

	/** Min & Max Distance for Controller for grabbed objects  - Customize Push & Pull functions mid action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MaxDistanceFromController = 20.0f;

	/** The westmost point of this world for the day-night cycle mechanic -leave as default if using built-in skysphere */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FVector SunReferencePoint = FVector(-25000.f, 0.f, -25000.f);

	/** The horizon pitch - moving beyond this point we should turn off the sun brightness for the day-night cycle mechanic -leave as default if using built-in skysphere */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float HorizonPitch = 65.f;

	/** The normal sun brightness - leave as default if using built-in skysphere */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float SunBrightness = 75.f;

	// Grab something within line trace range of controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	AActor* Grab(float Reach = 5.f, bool ScanOnlyWillManuallyAttach= false, EGrabTypeEnum GrabMode = EGrabTypeEnum::PRECISION_GRAB, FName TagName = FName(TEXT("")), FRotator Rotation_Offset = FRotator::ZeroRotator, bool RetainObjectRotation = true, bool RetainDistance = false, bool ShowDebugLine = false);

	// Set distance from controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	void SetDistanceFromController(float NewDistance, float MinDistance, float MaxDistance);

	// Release grabbed object
	UFUNCTION(BlueprintCallable, Category = "VR")
	AActor* Release();

	// Pull grabbed object 
	UFUNCTION(BlueprintCallable, Category = "VR")
	void PullGrabbedObject(float PullSpeed = 1.f, float MinDistance = 1.f, float MaxDistance = 20.f);

	// Push grabbed object(s) 
	UFUNCTION(BlueprintCallable, Category = "VR")
	void PushGrabbedObject(float PushSpeed = 1.f, float MinDistance = 1.f, float MaxDistance = 20.f);

	// Stop Pull
	UFUNCTION(BlueprintCallable, Category = "VR")
	AActor* StopPull();

	// Stop Push
	UFUNCTION(BlueprintCallable, Category = "VR")
	AActor* StopPush();

	// Cycle World Day/Night
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool GrabSun(AActor* Sky_Sphere, float SunCycleRate = 2.f);

private:
	// Motion Controller Transform
	FVector ControllerLocation = FVector::ZeroVector;
	FRotator ControllerRotation = FRotator::ZeroRotator;

	// Temp Variables
	UPhysicsHandleComponent* GrabbedObject = nullptr;
	EGrabTypeEnum GrabType = EGrabTypeEnum::PRECISION_GRAB;
	FVector NewGrabbedLocation = FVector::ZeroVector;
	FRotator StandardOffset = FRotator::ZeroRotator;
	FRotator RotationOffset = FRotator::ZeroRotator;

	// Get Actor hit by line trace
	AActor* GetHit(FVector LineTraceStart, FVector LineTraceEnd, bool RetainDistance, bool bShowDebugLine);
	bool bManualAttach = false;

	// Pull-Push Mechanic
	bool bIsPullingOrPushing = false;
	float Speed = 1.f;
	// Update Pulled-Pushed Object
	void UpdatePullPush();

	// Cycle Day Night Mechanic
	bool bIsGrabbingSun = false;
	bool bMoveEast = true;
	float DistanceFromSun = 0.f;
	float CycleRate = 2.f;
	AActor* SkySphere = nullptr;
	ADirectionalLight* SunDirectionalLightActor = nullptr;
	ULightComponent* SunDirectionalLightComponent = nullptr;
	FRotator RotationDuringGrab = FRotator::ZeroRotator;
	void UpdateDayNight();
};
