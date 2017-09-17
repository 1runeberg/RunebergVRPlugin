// Copyright (C) 2017 Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "RunebergVRPluginPrivatePCH.h"
#include "RunebergVR_SimpleGrabber.h"


// Sets default values for this component's properties
URunebergVR_SimpleGrabber::URunebergVR_SimpleGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Create sphere collision
	SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("GrabberSphere"));

	// Register the collision delegate function (begin overlap)
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &URunebergVR_SimpleGrabber::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &URunebergVR_SimpleGrabber::OnEndOverlap);
}


// Called when the game starts
void URunebergVR_SimpleGrabber::BeginPlay()
{
	Super::BeginPlay();

	// Set Sphere Component Defaults
	SphereComponent->SetWorldLocation(GetOwner()->GetActorLocation());
	SphereComponent->SetWorldRotation(GetOwner()->GetActorRotation());
	SphereComponent->SetSphereRadius(GrabSphereRadius);
	SphereComponent->AttachToComponent(GetAttachParent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
	SphereComponent->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->BodyInstance.SetResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
}

void URunebergVR_SimpleGrabber::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void URunebergVR_SimpleGrabber::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (isGrabbing && !GrabbedObject && !GrabbedComponent && !Cast<APawn>(OtherActor) && (int)OtherComp->GetCollisionObjectType() == ObjectTypeID) {
		GrabbedObject = OtherActor;
		GrabbedComponent = OtherComp;

		if (GrabbedObject && GrabbedComponent) {
			GrabbedObject->DisableComponentsSimulatePhysics();
			GrabbedObject->GetRootComponent()->AttachToComponent(GetAttachParent(), FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		}
	}

}

// Enable grabbing mode
void URunebergVR_SimpleGrabber::Grab(int _ObjectTypeID)
{
	ObjectTypeID = _ObjectTypeID;
	isGrabbing = true;
}

// Disable grabbing mode
void URunebergVR_SimpleGrabber::Release(bool EnablePhysics)
{
	isGrabbing = false;

	// Detach grabbed object from grabber
	if (GrabbedObject) {
		GrabbedObject->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	// Empty grabbed object holder
	GrabbedObject = nullptr;

	// Re-enable physics on component
	if (GrabbedComponent && EnablePhysics) {
		if (Cast<UStaticMeshComponent>(GrabbedComponent)) {
			Cast<UStaticMeshComponent>(GrabbedComponent)->SetSimulatePhysics(true);
		}
		else if (Cast<USkeletalMeshComponent>(GrabbedComponent)) {
			Cast<USkeletalMeshComponent>(GrabbedComponent)->SetSimulatePhysics(true);
		}
	}

	// Empty grabbed component holder
	GrabbedComponent = nullptr;
}
