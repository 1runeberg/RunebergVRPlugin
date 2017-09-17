// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_CustomGravity.h"


// Sets default values for this component's properties
URunebergVR_CustomGravity::URunebergVR_CustomGravity()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called every frame
void URunebergVR_CustomGravity::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Move this actor to the direction of gravity
	if (IsGravityActive)
	{

		switch (GravityDirection)
		{
		case EGravityDirection ::DIR_DOWN:
			GetOwner()->GetRootComponent()->AddRelativeLocation(FVector(0.0f, 0.0f, -1.0f * GravityStrength ));
			break;
		case EGravityDirection::DIR_UP:
			GetOwner()->GetRootComponent()->AddRelativeLocation(FVector(0.0f, 0.0f, 1.0f * GravityStrength));
			break;
		case EGravityDirection::DIR_LEFT:
			GetOwner()->GetRootComponent()->AddRelativeLocation(FVector(0.0f, 1.0f * GravityStrength, 0.0f));
			break;
		case EGravityDirection::DIR_RIGHT:
			GetOwner()->GetRootComponent()->AddRelativeLocation(FVector(0.0f, -1.0f * GravityStrength, 0.0f));
			break;
		case EGravityDirection::DIR_FORWARD:
			GetOwner()->GetRootComponent()->AddRelativeLocation(FVector(-1.0f * GravityStrength, 0.0f, 0.0f));
			break;
		case EGravityDirection::DIR_BACK:
			GetOwner()->GetRootComponent()->AddRelativeLocation(FVector(1.0f * GravityStrength, 0.0f, 0.0f));
			break;
		case EGravityDirection::DIR_RELATIVE:
			GetOwner()->GetRootComponent()->AddRelativeLocation((GravityOrigin - (GetOwner()->GetActorLocation()).GetSafeNormal()) * FVector(GravityStrength, GravityStrength, GravityStrength));
			break;
		default:
			break;
		}
	

	}
}

/** Stop or Start Gravity if collided component or actor has the corresponding start/stop tag */
void URunebergVR_CustomGravity::ProcessTags(AActor* OtherActor,  UPrimitiveComponent* OtherComp)
{

	// Check whether to stop gravity
	for (int32 i = 0; i < StopTags.Num(); i++)
	{
		if ((OtherActor && OtherActor->ActorHasTag(StopTags[i])) || (OtherComp && OtherComp->ComponentHasTag(StopTags[i])))
		{
			IsGravityActive = false;
			return;
		}
	}

	// Check whether to start gravity
	for (int32 i = 0; i < StartTags.Num(); i++)
	{
		if ((OtherActor && OtherActor->ActorHasTag(StartTags[i])) || (OtherComp && OtherComp->ComponentHasTag(StartTags[i])))
		{
			IsGravityActive = true;
			return;
		}
	}
}

