// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_Climb.h"


// Sets default values for this component's properties
URunebergVR_Climb::URunebergVR_Climb()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void URunebergVR_Climb::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Climbing motion
	if (IsActive() && IsClimbing && GetAttachParent())
	{
		// Disable custom gravity if defined
		if (CustomGravity)
		{
			CustomGravity->IsGravityActive = false;
		}

		GetOwner()->GetRootComponent()->AddRelativeLocation((InitialLocation - GetAttachParent()->GetComponentLocation()).GetSafeNormal());
	}
}


/** Enable climb mode */
void URunebergVR_Climb::Climb() 
{
	if (IsActive() && !IsClimbing)
	{
		// Get parent attachment
		USceneComponent* ClimbingReference = GetAttachParent();

		// If no climb tags have been specified, free climb
		if (ClimbTags.Num() <= 0)
		{
			StartClimb(ClimbingReference);
			return;
		}

		// Climb tags have been specified, check for overlapping components. Parent must be of Primitive type
		UPrimitiveComponent* TryPrimitive = Cast<UMeshComponent>(ClimbingReference);
		if (TryPrimitive)
		{
			TArray<UPrimitiveComponent*> OverlappingComponents;
			TryPrimitive->GetOverlappingComponents(OverlappingComponents);

			for (int32 i = 0; i < OverlappingComponents.Num(); i++)
			{
				if (OverlappingComponents[i])
				{
					// Check this component for defined climb tags
					for (int32 j = 0; j < ClimbTags.Num(); j++)
					{
						if (OverlappingComponents[j]->ComponentHasTag(ClimbTags[j]))
						{
							StartClimb(ClimbingReference);
							return;
						}
					}
				}
			}

			
		}
	}
}

void URunebergVR_Climb::StartClimb(USceneComponent* ClimbingReference)
{
	// Set Climbing mode to active and initialize starting location
	InitialLocation = ClimbingReference->GetComponentLocation();
	IsClimbing = true;

	// Stop custom gravity if defined
	if (CustomGravity)
	{
		CustomGravity->IsGravityActive = false;
	}
}


/** Deactivate climb mode */
void URunebergVR_Climb::LetGo()
{
	// Deactivate climbing mode
	IsClimbing = false;

	// Activate custom gravity if defined
	if (CustomGravity)
	{
		CustomGravity->IsGravityActive = true;
	}
}
