// Copyright (C) 2016  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

#pragma once

#include "Components/ActorComponent.h"
#include "VRPawnComponent_VRMovement.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VR_CPP_API UVRPawnComponent_VRMovement : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVRPawnComponent_VRMovement();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Movement Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float MovementSpeed = 0.0f;

	// Target Rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FRotator TargetRotation = FRotator::ZeroRotator;

	// Movement Reference - Object that dictates the direction/rotation of the Pawn's movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	USceneComponent* MovementReference = nullptr;

	// Indicator if the pawn is moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool IsMoving = false;	

	// Enable VR Movement
	UFUNCTION(BlueprintCallable, Category = "VR")
	void EnableVRMovement(float PawnMovementSpeed, USceneComponent* PawnMovementReference = nullptr, FRotator PawnManualRotation = FRotator::ZeroRotator);

	// Disable VR Movement
	UFUNCTION(BlueprintCallable, Category = "VR")
	void DisableVRMovement();

private:
	AActor* VRPawn = nullptr;	// The VR Pawn that this component is attached to
};
