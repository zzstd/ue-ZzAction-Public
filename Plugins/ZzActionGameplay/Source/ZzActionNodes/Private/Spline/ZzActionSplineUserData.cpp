// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Spline/ZzActionSplineUserData.h"

#include "ZzActionBlueprint.h"
#include "ZzActionComponent.h"
#include "ZzActionInstance.h"
#include "Components/SplineComponent.h"
#include "Spline/ZzActionSplineActor.h"
#include "Misc/EngineVersionComparison.h"


void UZzActionSplineUserData::CopyFrom(const USplineComponent* Spline)
{
	if (!Spline)
	{
		return;
	}
	
	Points.Reset();
	const int32 NumPoints = Spline->GetNumberOfSplinePoints();
	Points.Reserve(NumPoints);
	for (int32 Index = 0; Index < NumPoints; ++Index)
	{
		Points.Add(Spline->GetSplinePointAt(Index, ESplineCoordinateSpace::Local));
	}

	bClosedLoop = Spline->IsClosedLoop();
	DefaultUpVector = Spline->DefaultUpVector;
	Duration = Spline->Duration;
	ReparamStepsPerSegment = Spline->ReparamStepsPerSegment;
	bStationaryEndpoints = Spline->bStationaryEndpoints;
#if WITH_EDITOR
	bAllowDiscontinuousSpline = Spline->bAllowDiscontinuousSpline;
	
	if (PreviewSplineComponent && PreviewSplineComponent != Spline)
	{
		TGuardValue bGuard(bPreviewSplineModifyScoped, true);
		CopyTo(PreviewSplineComponent);
	}
#endif
}

void UZzActionSplineUserData::CopyTo(USplineComponent* Spline) const
{
	if (!Spline)
	{
		return;
	}
	
#if WITH_EDITOR
	Spline->bAllowDiscontinuousSpline = bAllowDiscontinuousSpline;
#endif
	Spline->bStationaryEndpoints = bStationaryEndpoints;
	Spline->DefaultUpVector = DefaultUpVector;
	Spline->Duration = Duration;
	Spline->ReparamStepsPerSegment = ReparamStepsPerSegment;

	Spline->SetClosedLoop(bClosedLoop, false);
	{
		Spline->ClearSplinePoints(false);
		Spline->AddPoints(Points, false);
	}
	
	Spline->UpdateSpline();
	Spline->MarkRenderStateDirty();
}

void UZzActionSplineUserData::OnActionConstructed(UZzActionInstance* InActionInstance)
{
	if (InActionInstance->IsPreview())
	{
		return;
	}

	FTransform ActorTF = InActionInstance->GetActionComponent()->GetOwner()->GetActorTransform();
	ActorTF *= FTransform(FVector(0, 0, -90));
	
	auto SplineActor = SpawnSplineActor(InActionInstance->GetWorld(), ActorTF, true);
	SplineActor->AddActionInst(InActionInstance);
	
	auto SplineActorName = GetSplineDataName(SplineName);
	InActionInstance->ActionData.Objects.Add(SplineActorName, SplineActor);
}

FName UZzActionSplineUserData::GetSplineDataName(FName SplineName)
{
	return FName(TEXT("SplineUserData_") + SplineName.ToString());
}

UZzActionSplineUserData* UZzActionSplineUserData::GetSplineUserData(const UZzActionBlueprint* Asset, FName SplineName)
{
	if (Asset)
	{
		for (auto a : Asset->GetAllUserDataAsAction())
		{
			if (auto SplineUserData = Cast<UZzActionSplineUserData>(a))
			{
				if (SplineUserData->SplineName == SplineName)
				{
					return SplineUserData;
				}
			}
		}
	}
	
	return nullptr;
}

void UZzActionSplineUserData::OnPreviewEnter(UWorld* InWorld, TArray<AActor*>& OutActors)
{
	TGuardValue bGuard(bPreviewSplineModifyScoped, true);
	
	const bool bValidPoints = Points.Num() > 1;
	PreviewSplineActor = SpawnSplineActor(InWorld, FTransform::Identity, bValidPoints);
	PreviewSplineComponent = PreviewSplineActor->Spline;
	
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 7, 0)
PRAGMA_DISABLE_EXPERIMENTAL_WARNINGS
	PreviewSplineComponent->GetOnSplineChanged().AddUObject(this, &UZzActionSplineUserData::HandlePreviewSplineChanged);
PRAGMA_ENABLE_BUFFER_OVERRUN_WARNING
#endif
	OutActors.Add(PreviewSplineActor);
}

void UZzActionSplineUserData::OnPreviewExit(UWorld* InWorld)
{
	if (PreviewSplineActor)
	{
		CopyFrom(PreviewSplineComponent);
		PreviewSplineActor->Destroy();
	}
}

AZzActionSplineActor* UZzActionSplineUserData::SpawnSplineActor(UWorld* InWorld, const FTransform& SpawnTF, bool bApplyPoint) const
{
	check(InWorld);
	
	auto SplineActor = InWorld->SpawnActor<AZzActionSplineActor>();
	SplineActor->SetActorTransform(RelativeTransform * SpawnTF);
	
	if (bApplyPoint)
	{
		CopyTo(SplineActor->Spline);
	}
	
	return SplineActor;
}

void UZzActionSplineUserData::HandlePreviewSplineChanged()
{
	if (bPreviewSplineModifyScoped)
	{
		return;
	}
	
	Modify();
	CopyFrom(PreviewSplineComponent);
}
