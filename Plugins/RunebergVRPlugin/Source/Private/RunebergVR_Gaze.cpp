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
#include "RunebergVR_Gaze.h"


// Sets default values for this component's properties
URunebergVR_Gaze::URunebergVR_Gaze()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called every frame
void URunebergVR_Gaze::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check if we are gazing
	if (IsGazing)
	{
		// Do line trace / ray-cast
		FHitResult	Hit;
		FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
		UWorld* World = GEngine->GetWorldFromContextObject(GetOwner());
		bool const bHit = World->LineTraceSingleByChannel(Hit, 
			GetAttachParent()->GetComponentLocation(), 
			GetAttachParent()->GetComponentRotation().Vector() + (GetAttachParent()->GetComponentRotation().Vector() * GazeRange),
			TargetCollisionType,
			TraceParameters);

		//if (bHit && Hit.GetActor())
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Actor hit is %s"), *Hit.GetActor()->GetName());
		//}

		// Draw Debug line
		if (bDrawDebugLine)
		{
			DrawDebugLine(
				GetWorld(),
				FVector(GetAttachParent()->GetComponentLocation().X, GetAttachParent()->GetComponentLocation().Y, GetAttachParent()->GetComponentLocation().Z + 25),
				GetAttachParent()->GetComponentRotation().Vector() + (GetAttachParent()->GetComponentRotation().Vector() * GazeRange),
				FColor(255, 0, 0),
				false, -1, 0,
				12.333
			);
		}


		// Return any hits
		if (bHit && Hit.GetActor() && Hit.GetActor()->ActorHasTag(TargetTag))
		{

			// Add deltatime to current duration
			GazeCurrentDuration += DeltaTime;

			// Report hit
			GazeHasHit = true;
			OnGazeHit.Broadcast(Hit, GazeCurrentDuration / GazeTargetDuration);

			// Check if sufficient time has elapsed for gaze to be considered a hit
			if (GazeCurrentDuration >= GazeTargetDuration)
			{
				OnGazeActivate.Broadcast(Hit);

				if (StopGazeAfterHit)
				{
					EndGaze();
				}
			}	
		}
		//else if (bHit && Hit.GetActor() && !Hit.GetActor()->ActorHasTag(TargetTag))
		//{
		//	// No valid actor in gaze range, reset gaze duration
		//	GazeHasHit = false;
		//	GazeCurrentDuration = 0.f;
		//	OnGazeLost.Broadcast(Hit);
		//}
		else
		{
			// No hit
			GazeHasHit = false;
			GazeCurrentDuration = 0.f;
			OnGazeLost.Broadcast(Hit);
		}
	}
}

// Start gaze
void URunebergVR_Gaze::StartGaze(float Gaze_Range, float Gaze_TargetDuration, bool DrawDebugLine)
{
	GazeHasHit = false;
	GazeCurrentDuration = 0.f;
	GazeRange = Gaze_Range;
	GazeTargetDuration = Gaze_TargetDuration;
	IsGazing = true;

	if (DrawDebugLine)
	{
		bDrawDebugLine = true;
	}
	else
	{
		bDrawDebugLine = false;
	}

}

// End Gaze
void URunebergVR_Gaze::EndGaze()
{
	GazeHasHit = false;
	GazeCurrentDuration = 0.f;
	IsGazing = false;
	bDrawDebugLine = false;
}

