// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Spline/ZzActionSplineUserData.h"

#include "ZzActionBlueprint.h"
#include "ZzActionComponent.h"
#include "ZzActionInstance.h"
#include "Components/SplineComponent.h"
#include "Spline/ZzActionNiagaraSplineActor.h"


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
#endif
}

void UZzActionSplineUserData::CopyTo(USplineComponent* Spline) const
{
	if (!Spline)
	{
		return;
	}
	bHasCopyScope = true;
	
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
	
	bHasCopyScope = false;
}

void UZzActionSplineUserData::OnActionConstructed(UZzActionInstance* InActionInstance)
{
	if (InActionInstance->IsPreview())
	{
		return;
	}

	auto SplineActor = InActionInstance->GetWorld()->SpawnActor<AZzActionNiagaraSplineActor>();
	CopyTo(SplineActor->Spline);
	SplineActor->AddActionInst(InActionInstance);
	{
		FTransform SpawnTF = InActionInstance->GetActionComponent()->GetOwner()->GetActorTransform();
		SpawnTF.SetLocation(SpawnTF.GetLocation() + FVector(0, 0, -90));
		SplineActor->SetActorTransform(SpawnTF);
	}
	
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
	PreviewSplineActor = InWorld->SpawnActor<AZzActionNiagaraSplineActor>();
	PreviewSplineComponent = PreviewSplineActor->Spline;
	if (Points.Num() > 1)
	{
		CopyTo(PreviewSplineComponent);
	}
PRAGMA_DISABLE_EXPERIMENTAL_WARNINGS
	PreviewSplineComponent->GetOnSplineChanged().AddUObject(this, &UZzActionSplineUserData::HandlePreviewSplineChanged);
PRAGMA_ENABLE_BUFFER_OVERRUN_WARNING
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

void UZzActionSplineUserData::HandlePreviewSplineChanged()
{
	if (bHasCopyScope)
	{
		return;
	}
	
	Modify();
	CopyFrom(PreviewSplineComponent);
}