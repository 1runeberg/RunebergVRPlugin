// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Components/SceneComponent.h"
#include "RunebergVR_SimpleGrabber.generated.h"


UCLASS( ClassGroup=(VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_SimpleGrabber : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_SimpleGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public: 
	// Object Type that'll be "grabbable" (PhysicsBody)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float GrabSphereRadius = 8.f;

	// Enable grab
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Grab(int _ObjectTypeID = 5);

	// Release grabbed object
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Release(bool EnablePhysics = true);

	// Delegate function that'll be called during begin overlap 
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Delegate function that'll be called during end overlap 
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	bool isGrabbing;			  				        // Wether or not grabbing is enabled
	USphereComponent* SphereComponent;					// Sphere collision for simple grabs
	AActor* GrabbedObject = nullptr;					// Grabbed Actor
	UPrimitiveComponent* GrabbedComponent = nullptr;	// Grabbed Actor
	int ObjectTypeID = 5;								// EChannelCollision ID of "Grabbable Objects" default is 5 for PhaysicsBody
};
