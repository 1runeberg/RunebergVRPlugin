// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/* 
GNU GPLv3 - http://www.gnu.org/licenses/gpl.txt

VR Pawn with C++ & Blueprint common interface functions
Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

UCLASS(Blueprintable)
class VR_CPP_API AVRPawn : public APawn
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	AVRPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Set the location of the motion controller(s)
	UFUNCTION(BlueprintCallable, Category = "VR")
	void UpdateGrabbedObjectLocation(FVector LeftControllerLocation, FRotator LeftControllerRotation, FVector RightControllerLocation, FRotator RightControllerRotation);

	// Grab something within ray-cast range of controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Grab(bool Left, bool Right, float Reach, bool ShowDebugLine, bool SetLocationManually, FVector LeftControllerLocation, FRotator LeftControllerRotation, FVector RightControllerLocation, FRotator RightControllerRotation);

	// Release grabbed object
	UFUNCTION(BlueprintCallable, Category = "VR")
	void Release(bool Left, bool Right);

	// Pull grabbed object(s) 
	UFUNCTION(BlueprintCallable, Category = "VR")
	void PullGrabbedObjects(bool ObjectInLeftController, bool ObjectInRightController, int Speed = 1);

	// Pull grabbed object(s) 
	UFUNCTION(BlueprintCallable, Category = "VR")
	void PushGrabbedObjects(bool ObjectInLeftController, bool ObjectInRightController, int Speed = 1);

	// Set distance from left controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	void SetDistanceFromLeftController(float NewDistance);

	// Set distance from right controller
	UFUNCTION(BlueprintCallable, Category = "VR")
	void SetDistanceFromRightController(float NewDistance);

	/// Current Distance of Grabbed items from their respective controllers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	float DistanceFromControllerLeft = MinDistanceFromLeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	float DistanceFromControllerRight = MinDistanceFromRightController;

	/// Original distance difference of grabbed actors from controller during grabbing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MinDistanceFromLeftController = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MinDistanceFromRightController = 10.0f;

	/// Original distance difference of grabbed actors from controller during grabbing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MaxDistanceFromLeftController = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MaxDistanceFromRightController = 250.0f;

private:
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;
	
	/// Motion Controller Location
	FVector ControllerLocationLeft;
	FRotator ControllerRotationLeft;
	FVector ControllerLocationRight;
	FRotator ControllerRotationRight;

	/// Grabbed Actors
	UPhysicsHandleComponent* GrabbedLeft = nullptr;
	UPhysicsHandleComponent* GrabbedRight = nullptr;

	AActor* GetHit(FVector& LineTraceStart, FVector& LineTraceEnd, bool bShowDebugLine);	// Get Actor hit by Ray-Cast
	void AttemptGrab(FVector& LineTraceStart, FVector& LineTraceEnd, bool bGrabLeft, bool bShowDebugLine);	// Attempt to Grab Actor
};
