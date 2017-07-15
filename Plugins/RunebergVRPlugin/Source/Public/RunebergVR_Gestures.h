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
#include "RunebergVR_Gestures_Database.h"
#include "RunebergVR_Gestures.generated.h"


USTRUCT(BlueprintType)
struct FFloatArray
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "VR")
	TArray<float> FloatValue;

};

USTRUCT(BlueprintType)
struct FIntArray
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "VR")
	TArray<int32> IntValue;

};

USTRUCT(BlueprintType)
struct FDrawnGestures
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "VR")
	USplineComponent* SplineComponent;
	
	UPROPERTY(EditAnywhere, Category = "VR")
	TArray<USplineMeshComponent*> SplineMesh;

	FDrawnGestures(USplineComponent* Spline_Component, TArray<USplineMeshComponent*> Spline_Mesh) : SplineComponent(Spline_Component), SplineMesh(Spline_Mesh) {}
	FDrawnGestures() {}

};

UCLASS(ClassGroup = (VR), meta=(BlueprintSpawnableComponent) )
class RUNEBERGVRPLUGIN_API URunebergVR_Gestures : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URunebergVR_Gestures();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Known gestures database to be used by this component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	URunebergVR_Gestures_Database* KnownGesturesDB;

	/** How different two gesture patterns can be for testing similarity measured in DTW distance units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float GlobalThreshold = 25.f;

	/** How far apart each point can be for the DTW similarity test measured in DTW distance units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	float VectorThreshold = 35.f;

	/** Maximum number of vertical or horizontal steps in a row for the DTW similarity test */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
	int MaxSlope = 100;

	/** Latest recorded VR Gesture */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR - Read Only")
	FVRGesture VRGesture;

	/** Wether or not this Gestures Component is currently recording */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR - Read Only")
	bool IsRecording = false;

	// Start recording VR Gesture
	UFUNCTION(BlueprintCallable, Category = "VR")
	void StartRecordingGesture(float RecordingInterval, FString GestureName,
		bool DrawLine, UStaticMesh* LineMesh, UMaterial* LineMaterial,
		FVector LineOffset, FRotator RotationOffset);

	// Stop recording VR Gesture
	UFUNCTION(BlueprintCallable, Category = "VR")
	FVRGesture StopRecordingGesture(bool SaveToDB = false);

	// Convenience function to save a gesture to DB
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool SaveGestureToDB();

	// Convenience function to empty a Gesture DB
	UFUNCTION(BlueprintCallable, Category = "VR")
	bool EmptyKnownGestures();

	// Draw stored gesture
	UFUNCTION(BlueprintCallable, Category = "VR")
	void DrawVRGesture(FVRGesture VR_Gesture, UStaticMesh* LineMesh, UMaterial* LineMaterial,
		FVector OriginLocation = FVector::ZeroVector, FRotator OriginRotation = FRotator::ZeroRotator, 
		FVector OffsetLocation = FVector::ZeroVector, float OffsetDistance = 100.f, float Lifetime = 3.f);

	// Find Gesture in the Known Gestures Database
	UFUNCTION(BlueprintCallable, Category = "VR")
	FString FindGesture();

private:
	// Internal variables
	const FString UNKNOWN_GESTURE = FString(TEXT("UNKNOWN_GESTURE"));
	float Interval = 0.5f;
	float DeltaSeconds = 0.f;
	int32 VectorScale = 3;

	// Calculate the Manhattan Distance between two points
	float CalculateManhattanDistance(FVector PointA, FVector PointB);

	// Calculate the Euclidian distance between two points
	float CalculateEuclidianDistance(FVector PointA, FVector PointB);

	// Calculate the minimum Dynamic Time Warping (DTW) distance between Gesture2 and every possible ending of Gesture1
	float CalculateDTW(TArray<FVector> Gesture1, TArray<FVector> Gesture2);

	// Remove Gesture
	void RemoveGesture(TArray<FDrawnGestures> Drawn_Gestures, int GestureIndex = 0);

	// Real-time gesture drawing
	//TODO: Refactor with clear drawn gestures
	bool bDrawRTLine = false;
	FVector RT_PriorVector = FVector::ZeroVector;
	UStaticMesh* RT_LineMesh;
	UMaterial* RT_LineMaterial;
	FVector RT_LineOffset = FVector::ZeroVector;
	FRotator RT_RotationOffset = FRotator::ZeroRotator;
	USplineComponent* CurrentSpline = nullptr;
	TArray<USplineMeshComponent*> RTSplineMeshArray;

	// Clear drawn gesture & variables
	TArray<FDrawnGestures> DrawnGestures;
	void ClearDrawnGesture();
};
