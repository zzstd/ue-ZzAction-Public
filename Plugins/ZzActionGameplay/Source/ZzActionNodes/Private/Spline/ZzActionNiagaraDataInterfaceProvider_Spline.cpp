// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Spline/ZzActionNiagaraDataInterfaceProvider_Spline.h"

#include "NiagaraDataInterfaceSpline.h"
#include "ZzActionBlueprint.h"
#include "ZzActionInstance.h"
#include "Node/ZzActionNode.h"
#include "Spline/ZzActionNiagaraSplineActor.h"
#include "Spline/ZzActionSplineUserData.h"


void UZzActionNiagaraDataInterfaceProvider_Spline::AddSplineUserData() const
{
	if (auto Asset = GetTypedOuter<UZzActionBlueprint>())
	{
		auto NewSpline = NewObject<UZzActionSplineUserData>();
		NewSpline->SplineName = SplineName;
		Asset->AddUserData(NewSpline);
	}
}

void UZzActionNiagaraDataInterfaceProvider_Spline::CopyTo(UNiagaraComponent* Comp, UNiagaraDataInterface* InSource, UNiagaraDataInterface* InDes)
{
	InSource->CopyTo(InDes);
	if (auto SplineDI = Cast<UNiagaraDataInterfaceSpline>(InDes))
	{
		if (auto Node = GetTypedOuter<UZzActionNode>())
		{
			if (auto ActionInst = Node->GetNodeContext().GetActionInstance())
			{
				if  (auto SplineUserData = UZzActionSplineUserData::GetSplineUserData(ActionInst->GetActionAsset(), SplineName))
				{
					if (Node->GetNodeContext().IsPreview())
					{
						SplineDI->SoftSourceActor = SplineUserData->PreviewSplineActor;
					}
					else
					{
						auto SplineActorName =UZzActionSplineUserData::GetSplineDataName(SplineName);
						if (auto SplineActor = Node->GetNodeContext().GetActionData().GetObject<AZzActionNiagaraSplineActor>(SplineActorName))
						{
							SplineActor->AddNiagaraInst(Comp);
							SplineDI->SoftSourceActor = SplineActor;
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("NoFindSplineUserData: %s"), *SplineName.ToString());
						}
					}
				}
			}
		}
	}
}

bool UZzActionNiagaraDataInterfaceProvider_Spline::ShouldHiddenDIProperty(FName PropName) const
{
	return PropName == GET_MEMBER_NAME_CHECKED(UNiagaraDataInterfaceSpline, SoftSourceActor);
}

TArray<FName> UZzActionNiagaraDataInterfaceProvider_Spline::GetSplineUserDataOptions() const
{
	TArray<FName> Result;
	if (auto Asset = GetTypedOuter<UZzActionBlueprint>())
	{
		for (auto a : Asset->GetAllUserDataAsAction())
		{
			if (auto SplineUserData = Cast<UZzActionSplineUserData>(a))
			{
				Result.Add(SplineUserData->SplineName);
			}
		}
	}
	
	return Result;
}
