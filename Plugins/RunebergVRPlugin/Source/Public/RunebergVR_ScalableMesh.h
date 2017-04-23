// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Components/SceneComponent.h"
#include "RunebergVR_ScalableMesh.generated.h"

UENUM(BlueprintType)
enum class EScaleModeEnum : uint8
{
	SCALE_TO_MAX 	UMETA(DisplayName = "Scale up to max distance"),
	SCALE_TO_MIN 	UMETA(DisplayName = "Scale down to min distance"),
	SCALE_TO_LOC 	UMETA(DisplayName = "Scale up to defined location"),
	SCALE_DN_MOV	UMETA(DisplayName = "Scale down while getting player to location")
};

UENUM(BlueprintType)
enum class EScaleDirectionEnum : uint8
{
	SCALE_X 	UMETA(DisplayName = "Scale mesh in X"),
	SCALE_Y 	UMETA(DisplayName = "Scale mesh in Y"),
	SCALE_Z 	UMETA(DisplayName = "Scale mesh in Z")
};

UCLASS( ClassGroup=(VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_ScalableMesh : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_ScalableMesh();

	// Target Location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	FVector TargetLocation = FVector::ZeroVector;

	/** Children Components -  Static Mesh and Skeletal Mesh components only, updated during runtume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	TArray<USceneComponent*> MeshChildren;


	// Check if currently scaling a mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	bool IsScaling = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Scale mesh to specified distance amount
	UFUNCTION(BlueprintCallable, Category = "VR")
	void ScaleMeshUp(FVector DistanceToScaleUpXYZ = FVector(112.f, 0.f, 0.f), FVector RateXYZ = FVector(1.5f, 0.f, 0.f), bool NewVisibility = true);

	// Scale mesh to specified distance amount
	UFUNCTION(BlueprintCallable, Category = "VR")
	void ScaleMeshDown(FVector DistanceToScaleDownXYZ = FVector(112.f, 0.f, 0.f), FVector RateXYZ = FVector(1.5f, 0.f, 0.f), bool VisibilityAfterScale = false);

	// Scale mesh to specified distance amount
	UFUNCTION(BlueprintCallable, Category = "VR")
	void ScaleMeshToLocation(FVector Target_Location = FVector(0.f, 0.f, 0.f), 
	EScaleDirectionEnum Scale_Direction = EScaleDirectionEnum::SCALE_X, float Rate = 1.f, bool NewVisibility = true);

	// Scale mesh down and move pawn to target location
	UFUNCTION(BlueprintCallable, Category = "VR")
	void ScaleDownAndMove(EScaleDirectionEnum Scale_Direction = EScaleDirectionEnum::SCALE_X, float Rate = 1.f, float DistanceCorrection=100.f, bool VisibilityAfterScale = false);

private:
	bool bNewVisibility;
	EScaleModeEnum ScaleMode;
	EScaleDirectionEnum ScaleDirection;
	FVector ScaleCache, RateCache;
	float DistanceToTarget = 0.f;
	float NumOfTimesToScale = 0.f;
};
