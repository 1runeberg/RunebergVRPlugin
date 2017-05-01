// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "Algo/Reverse.h"
#include "RunebergVRPlugin.h"
#include "RunebergVR_Gestures.h"


// Sets default values for this component's properties
URunebergVR_Gestures::URunebergVR_Gestures()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void URunebergVR_Gestures::BeginPlay()
{
	Super::BeginPlay();

	// Initialize Gesture
	VRGesture.GestureName = UNKNOWN_GESTURE;
	VRGesture.GesturePattern.Empty();

}


// Called every frame
void URunebergVR_Gestures::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsRecording)
	{
		// Add the time since last tick to the delta time
		DeltaSeconds += DeltaTime;

		// Check if the right amount of time has passed
		if (DeltaSeconds > Interval)
		{
			// Record current location of the controller and add to the existing pattern
			VRGesture.GesturePattern.Add(GetAttachParent()->GetRelativeTransform().GetLocation());

			// Reset timer
			DeltaSeconds = 0.f;
		}
	}
}


// Start recording VR Gesture
void URunebergVR_Gestures::StartRecordingGesture(FString GestureName, float RecordingInterval)
{
	// Name the gesture
	if (GestureName.IsEmpty())
	{
		// Create a default name if none was provided
		GestureName = FGuid::NewGuid().ToString();
	}
	VRGesture.GestureName = GestureName;

	// Clear the pattern array
	VRGesture.GesturePattern.Empty();

	// Add the first vector in the pattern
	VRGesture.GesturePattern.Add(GetAttachParent()->GetRelativeTransform().GetLocation());

	// Clear the timer
	DeltaSeconds = 0.f;

	// Set the recording interval
	Interval = RecordingInterval;

	// Start recording
	IsRecording = true;

}

// Stop recording VR Gesture
FVRGesture URunebergVR_Gestures::StopRecordingGesture(bool SaveToDB)
{
	// Add the final vector in the pattern
	VRGesture.GesturePattern.Add(GetAttachParent()->GetRelativeTransform().GetLocation());

	// Stop recording
	IsRecording = false;

	// Set the pattern to local space
	FVector OriginVector = FVector(VRGesture.GesturePattern[0].X, VRGesture.GesturePattern[0].Y, VRGesture.GesturePattern[0].Z);
	for (int32 i = 0; i < VRGesture.GesturePattern.Num(); i++)
	{
		VRGesture.GesturePattern[i] -= OriginVector;
	}

	// Save to database
	if (SaveToDB)
	{
		SaveGestureToDB();
	}

	// Return currently stored VR Gesture
	return VRGesture;
}

// Convenience function to save a gesture to DB
bool URunebergVR_Gestures::SaveGestureToDB()
{
	if (KnownGesturesDB)
	{
		// Add the normalized gesture into the DB
		KnownGesturesDB->VRGesturePatterns.Add(VRGesture);
		return true;
	}
	return false;

}

// Convenience function to empty a Gesture DB
bool URunebergVR_Gestures::EmptyKnownGestures()
{
	if (KnownGesturesDB)
	{
		KnownGesturesDB->VRGesturePatterns.Empty();
		return true;
	}
	return false;
}

// Draw stored gesture
void URunebergVR_Gestures::DrawVRGesture(FVRGesture VR_Gesture, FColor LineColor, FVector OriginLocation, FRotator OriginRotation, 
	float OffSetDistance, float Lifetime, float LineThickness)
{
	// Set Offset location
	FVector OffsetLocation = OriginLocation + (OriginRotation.Vector() * OffSetDistance);

	if (VR_Gesture.GesturePattern.Num() > 1)
	{
		for (int32 i = 0; i < VR_Gesture.GesturePattern.Num() - 1; i++)
		{
			DrawDebugLine(
				GetOwner()->GetWorld(),
				VR_Gesture.GesturePattern[i] + OffsetLocation,
				VR_Gesture.GesturePattern[i + 1] + OffsetLocation,
				LineColor,
				false,
				Lifetime,
				0,
				LineThickness
			);
		}
	}

}

// Calculate the Manhattan Distance between two points
float URunebergVR_Gestures::CalculateManhattanDistance(FVector PointA, FVector PointB)
{
	float ManhattanDistance = 0.f;

	for (int32 i = 0; i < VectorScale; i++)
	{
		ManhattanDistance += FMath::Abs(PointA[i] - PointB[i]);
	}
	return ManhattanDistance;
}

// Calculate the Euclidian distance between two points
float URunebergVR_Gestures::CalculateEuclidianDistance(FVector PointA, FVector PointB)
{
	float EuclidianDistance = 0.f;

	for (int i = 0; i < VectorScale; i++)
	{
		EuclidianDistance += FMath::Pow(PointA[i] - PointB[i], 2);
	}

	return FMath::Sqrt(EuclidianDistance);
}

// Calculate the minimum Dynamic Time Warping (DTW) distance between Gesture2 and every possible ending of Gesture1
float URunebergVR_Gestures::CalculateDTW(TArray<FVector> Gesture1, TArray<FVector> Gesture2)
{
	// Initialize reversed arrays (copy & reverse)
	TArray<FVector> Gesture1_Reversed = TArray<FVector>(Gesture1);
	Algo::Reverse(Gesture1_Reversed);

	TArray<FVector> Gesture2_Reversed = TArray<FVector>(Gesture2);
	Algo::Reverse(Gesture2_Reversed);

	// Create DTW Matrix
	TArray<FFloatArray> MatrixTab;
	TArray<FIntArray> SlopeI;
	TArray<FIntArray> SlopeJ;

	for (int i = 0; i < Gesture1_Reversed.Num()+1; i++)
	{
		FFloatArray NewFloatArray;
		MatrixTab.Add(NewFloatArray);

		FIntArray NewIntArray1, NewIntArray2;
		SlopeI.Add(NewIntArray1);
		SlopeJ.Add(NewIntArray2);

		for (int j = 0; j < Gesture2_Reversed.Num()+1; j++)
		{
			MatrixTab[i].FloatValue.Add(FLT_MAX);
			SlopeI[i].IntValue.Add(0);
			SlopeJ[i].IntValue.Add(0);
		}
	}

	MatrixTab[0].FloatValue[0] = 0.f;

	// Dynamic computation of the DTW matrix.
	for (int i = 1; i < Gesture1_Reversed.Num()+1; i++)
	{
		for (int j = 1; j < Gesture2_Reversed.Num()+1; j++)
		{
			if (MatrixTab[i].FloatValue[j - 1] < MatrixTab[i - 1].FloatValue[j - 1] && MatrixTab[i].FloatValue[j - 1] < MatrixTab[i - 1].FloatValue[j] && SlopeI[i].IntValue[j - 1] < MaxSlope)
			{
				MatrixTab[i].FloatValue[j] = CalculateEuclidianDistance(Gesture1_Reversed[i - 1], Gesture2_Reversed[j - 1]) + MatrixTab[i].FloatValue[j - 1];
				SlopeI[i].IntValue[j] = SlopeJ[i].IntValue[j - 1] + 1; ;
				SlopeJ[i].IntValue[j] = 0;
			}
			else if (MatrixTab[i - 1].FloatValue[j] < MatrixTab[i - 1].FloatValue[j - 1] && MatrixTab[i - 1].FloatValue[j] < MatrixTab[i].FloatValue[j - 1] && SlopeJ[i - 1].IntValue[j] < MaxSlope)
			{
				MatrixTab[i].FloatValue[j] = CalculateEuclidianDistance(Gesture1_Reversed[i - 1], Gesture2_Reversed[j - 1]) + MatrixTab[i - 1].FloatValue[j];
				SlopeI[i].IntValue[j] = 0;
				SlopeJ[i].IntValue[j] = SlopeJ[i - 1].IntValue[j] + 1;
			}
			else
			{
				MatrixTab[i].FloatValue[j] = CalculateEuclidianDistance(Gesture1_Reversed[i - 1], Gesture2_Reversed[j - 1]) + MatrixTab[i - 1].FloatValue[j - 1];
				SlopeI[i].IntValue[j] = 0;
				SlopeJ[i].IntValue[j] = 0;
			}
		}
	}

	// Find the best match between Gesture2 and an ending of Gesture1
	float BestMatch = FLT_MAX;
	for (int i = 1; i < Gesture1_Reversed.Num() + 1; i++)
	{
		if (MatrixTab[i].FloatValue[Gesture2_Reversed.Num()] < BestMatch)
			BestMatch = MatrixTab[i].FloatValue[Gesture2_Reversed.Num()];
	}
	return BestMatch;
}

// Find Gesture in the Known Gestures Database
FString URunebergVR_Gestures::FindGesture()
{
	if (KnownGesturesDB)
	{
		float MinDist = FLT_MAX;
		FString FoundGesture;

		for (int i = 0; i < KnownGesturesDB->VRGesturePatterns.Num(); i++)
		{
			TArray<FVector> GestureSample = KnownGesturesDB->VRGesturePatterns[i].GesturePattern;
			if (CalculateEuclidianDistance(VRGesture.GesturePattern[VRGesture.GesturePattern.Num() - 1],
				GestureSample[GestureSample.Num() - 1]) < VectorThreshold)
			{
				float DTW = CalculateDTW(VRGesture.GesturePattern, GestureSample) / (GestureSample.Num());
				if (DTW < MinDist)
				{
					MinDist = DTW;
					FoundGesture = KnownGesturesDB->VRGesturePatterns[i].GestureName;
				}
			}
		}

		if (MinDist < GlobalThreshold)
		{
			return FoundGesture;
		}
		else 
		{
			return UNKNOWN_GESTURE;
		}
	}

	return UNKNOWN_GESTURE;
}