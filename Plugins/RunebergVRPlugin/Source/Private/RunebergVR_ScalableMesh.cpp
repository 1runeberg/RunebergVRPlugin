// Copyright (C) 2016, 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2016, 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_ScalableMesh.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
URunebergVR_ScalableMesh::URunebergVR_ScalableMesh()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void URunebergVR_ScalableMesh::BeginPlay()
{
	Super::BeginPlay();

	// Get all child skeletal & static meshes of this component
	TArray<USceneComponent*> AllChildren;
	GetChildrenComponents(false, AllChildren);

	if (AllChildren.Num() > 0)
	{
		for (int32 i = 0; i < GetNumChildrenComponents(); i++)
		{
			USkeletalMeshComponent* TrySkeletalMesh = Cast<USkeletalMeshComponent>(AllChildren[i]);
			if (TrySkeletalMesh)
			{
				MeshChildren.Add(TrySkeletalMesh);
			}
			else
			{
				UStaticMeshComponent* TryStaticMesh = Cast<UStaticMeshComponent>(AllChildren[i]);
				if (TryStaticMesh)
				{
					MeshChildren.Add(TryStaticMesh);
				}
			}
		}
	}

}


// Called every frame
void URunebergVR_ScalableMesh::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsScaling && MeshChildren.Num()> 0)
	{
		if (ScaleMode == EScaleModeEnum::SCALE_TO_MAX)
		{
			for (int32 i = 0; i < MeshChildren.Num(); i++)
			{
				// Set visibility
				MeshChildren[i]->SetVisibility(bNewVisibility);

				// Set scales for each direction as needed
				if (ScaleCache.X > 0.00001f)
				{
					MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X + RateCache.X,
						MeshChildren[i]->GetComponentScale().Y,
						MeshChildren[i]->GetComponentScale().Z));
				}

				if (ScaleCache.Y > 0.00001f)
				{
					MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
						MeshChildren[i]->GetComponentScale().Y + RateCache.Y,
						MeshChildren[i]->GetComponentScale().Z));
				}

				if (ScaleCache.Z > 0.00001f)
				{
					MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
						MeshChildren[i]->GetComponentScale().Y,
						MeshChildren[i]->GetComponentScale().Z + RateCache.Z));
				}
			}

			// Update Scales (exploded for possible future tweaking)
			ScaleCache.X -= RateCache.X;
			ScaleCache.Y -= RateCache.Y;
			ScaleCache.Z -= RateCache.Z;

			// Check if we can stop scaling now
			if (ScaleCache.X < 0.000001f && ScaleCache.Y < 0.000001f && ScaleCache.Z < 0.000001f)
			{
				IsScaling = false;
			}
		}
		else if (ScaleMode == EScaleModeEnum::SCALE_TO_MIN)
		{
			for (int32 i = 0; i < MeshChildren.Num(); i++)
			{
				// Set scales for each direction as needed
				if (ScaleCache.X > 0.00001f)
				{
					MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X - RateCache.X,
						MeshChildren[i]->GetComponentScale().Y,
						MeshChildren[i]->GetComponentScale().Z));
				}

				if (ScaleCache.Y > 0.00001f)
				{
					MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
						MeshChildren[i]->GetComponentScale().Y - RateCache.Y,
						MeshChildren[i]->GetComponentScale().Z));
				}

				if (ScaleCache.Z > 0.00001f)
				{
					MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
						MeshChildren[i]->GetComponentScale().Y,
						MeshChildren[i]->GetComponentScale().Z - RateCache.Z));
				}
			}

			// Update Scales (exploded for possible future tweaking)
			ScaleCache.X -= RateCache.X;
			ScaleCache.Y -= RateCache.Y;
			ScaleCache.Z -= RateCache.Z;

			// Check if we can stop scaling now
			if (ScaleCache.X < 0.000001f && ScaleCache.Y < 0.000001f && ScaleCache.Z < 0.000001f)
			{

				// Set New Visibility
				for (int32 i = 0; i < MeshChildren.Num(); i++)
				{
					MeshChildren[i]->SetVisibility(bNewVisibility);
				}

				// Stop Scaling
				IsScaling = false;
			}
		}
		else if (ScaleMode == EScaleModeEnum::SCALE_TO_LOC)
		{
			for (int32 i = 0; i < MeshChildren.Num(); i++)
			{
				// Set visibility
				MeshChildren[i]->SetVisibility(bNewVisibility);

				// Face mesh to target location
				MeshChildren[i]->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetAttachParent()->GetComponentLocation(), TargetLocation));

				if (NumOfTimesToScale > 0.00001f)
				{
					// We haven't reached the target location yet, set the mesh scale
					switch (ScaleDirection)
					{
					case EScaleDirectionEnum::SCALE_X:
						MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X + RateCache.X,
							MeshChildren[i]->GetComponentScale().Y,
							MeshChildren[i]->GetComponentScale().Z));
						break;

					case EScaleDirectionEnum::SCALE_Y:
						MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
							MeshChildren[i]->GetComponentScale().Y + RateCache.X,
							MeshChildren[i]->GetComponentScale().Z));
						break;

					case EScaleDirectionEnum::SCALE_Z:
						MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
							MeshChildren[i]->GetComponentScale().Y,
							MeshChildren[i]->GetComponentScale().Z + RateCache.X));
						break;

					default:
						break;
					}
				}
			}

			// Update times to scale
			NumOfTimesToScale--;

			// Check if we can stop scaling now
			if (NumOfTimesToScale < 0.00001f)
			{
				IsScaling = false;
			}

		}
		else if (ScaleMode == EScaleModeEnum::SCALE_DN_MOV)
		{
			for (int32 i = 0; i < MeshChildren.Num(); i++)
			{
				// Face pawn to target location
				//GetOwner()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation));

				if (NumOfTimesToScale > 0.00001f)
				{
					// We haven't reached the target location yet, set the mesh scale
					switch (ScaleDirection)
					{
					case EScaleDirectionEnum::SCALE_X:
						MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X - RateCache.X,
							MeshChildren[i]->GetComponentScale().Y,
							MeshChildren[i]->GetComponentScale().Z));
						break;

					case EScaleDirectionEnum::SCALE_Y:
						MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
							MeshChildren[i]->GetComponentScale().Y - RateCache.X,
							MeshChildren[i]->GetComponentScale().Z));
						break;

					case EScaleDirectionEnum::SCALE_Z:
						MeshChildren[i]->SetWorldScale3D(FVector(MeshChildren[i]->GetComponentScale().X,
							MeshChildren[i]->GetComponentScale().Y,
							MeshChildren[i]->GetComponentScale().Z - RateCache.X));
						break;

					default:
						break;
					}

					// Move Pawn towards target location
					GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + (MeshChildren[i]->GetComponentRotation().Vector() * RateCache.X));
				}
			}

			// Update times to scale
			NumOfTimesToScale--;

			// Check if we can stop scaling now
			if (NumOfTimesToScale < 0.00001f)
			{
				// Set New Visibility
				for (int32 i = 0; i < MeshChildren.Num(); i++)
				{
					MeshChildren[i]->SetVisibility(bNewVisibility);
				}

				// Turn off scaling
				IsScaling = false;
			}

		}
	}
}

// Scale mesh to specified distance amount
void URunebergVR_ScalableMesh::ScaleMeshUp(FVector DistanceToScaleUpXYZ, FVector RateXYZ, bool NewVisibility)
{
	if (!IsScaling)
	{
		ScaleMode = EScaleModeEnum::SCALE_TO_MAX;
		ScaleCache = DistanceToScaleUpXYZ;
		RateCache = RateXYZ;
		bNewVisibility = NewVisibility;
		IsScaling = true;
	}
}

// Scale mesh down
void URunebergVR_ScalableMesh::ScaleMeshDown(FVector DistanceToScaleDownXYZ, FVector RateXYZ, bool VisibilityAfterScale)
{
	if (!IsScaling)
	{
		ScaleMode = EScaleModeEnum::SCALE_TO_MIN;
		ScaleCache = DistanceToScaleDownXYZ;
		RateCache = RateXYZ;
		bNewVisibility = VisibilityAfterScale;
		IsScaling = true;
	}

}

// Scale mesh to specified location
void URunebergVR_ScalableMesh::ScaleMeshToLocation(FVector Target_Location, EScaleDirectionEnum Scale_Direction, float Rate, bool NewVisibility)
{
	if (!IsScaling)
	{
		// Update variables
		ScaleMode = EScaleModeEnum::SCALE_TO_LOC;
		ScaleDirection = Scale_Direction;
		TargetLocation = Target_Location;
		RateCache.X = Rate; // Use X as magnitude for any direction

		// Calculate how much to scale
		DistanceToTarget = FVector::Distance(GetAttachParent()->GetComponentLocation(), TargetLocation);
		NumOfTimesToScale = DistanceToTarget / Rate;
		
		// Initiate mesh scale
		bNewVisibility = NewVisibility;
		IsScaling = true;
	}
}

// Scale mesh down and move pawn to target location
void URunebergVR_ScalableMesh::ScaleDownAndMove(EScaleDirectionEnum Scale_Direction, float Rate, float DistanceCorrection, bool VisibilityAfterScale)
{
	if (!IsScaling)
	{
		// Update variables
		ScaleMode = EScaleModeEnum::SCALE_DN_MOV;
		ScaleDirection = Scale_Direction;
		RateCache.X = Rate; // Use X as magnitude for any direction

		// Calculate how much to scale
		DistanceToTarget = FVector::Distance(GetAttachParent()->GetComponentLocation(), TargetLocation) - DistanceCorrection;
		NumOfTimesToScale = DistanceToTarget / Rate;

		// Initiate mesh scale
		bNewVisibility = VisibilityAfterScale;
		IsScaling = true;
	}
}
