// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "MotionWarping/ZzNotifyNode_MotionWarping.h"

#include "GameFramework/Actor.h"
#include "Timeline/ZzActionTimeline.h"
#include "RootMotionModifier_SkewWarp.h"
#include "MotionWarpingComponent.h"
#include "ZzActionBlueprint.h"
#include "ZzActionEditorBlueprintFunctionLibrary.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Misc/UObjectToken.h"
#include "MotionWarping/ZzRootMotionModifierInterface.h"
#include "Nodes/Anim/ZzActionAnimData.h"
#include "Nodes/Anim/ZzActionAnimFunctionLibrary.h"
#include "Nodes/Anim/ZzActionAnimNode.h"
#include "Nodes/Anim/ZzActionAnimRuntime.h"

UZzNotifyNode_MotionWarping::UZzNotifyNode_MotionWarping(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootMotionModifier = ObjectInitializer.CreateDefaultSubobject<URootMotionModifier_SkewWarp>(this, TEXT("RootMotionModifier_SkewWarp"));
	
	Tags.AddUnique(TEXT("Gameplay"));
}

void UZzNotifyNode_MotionWarping::OnInit()
{
	Super::OnInit();

	if (auto AnimRuntime = GetNodeContext().GetAnimRuntime())
    {
        AnimRuntime->WaitMontagePlay(FSimpleDelegate::CreateLambda([this, AnimRuntime]()
        {
        	if (auto Actor = GetNodeContext().GetOwnedActor())
            {
                if (auto MotionWarpingComp = Actor->FindComponentByClass<UMotionWarpingComponent>())
                {
         			OnBecomeRelevant(MotionWarpingComp, AnimRuntime->GetAnimMontage(), GetBeginTime(), GetEndTime());	
				}
			}
        }));
    }
}

FText UZzNotifyNode_MotionWarping::GetNotifyName_Implementation() const
{
#if WITH_EDITOR
	if (RootMotionModifier)
	{
		return RootMotionModifier.GetClass()->GetDisplayNameText();
	}
#endif
	return Super::GetNotifyName_Implementation();
}

FLinearColor UZzNotifyNode_MotionWarping::GetNotifyColor_Implementation() const
{
#if WITH_EDITOR
	if (RootMotionModifier)
	{
		if (RootMotionModifier.GetClass()->IsChildOf<URootMotionModifier_Warp>())
		{
			return FColor::Orange;
		}
	}
#endif
	return FColor::Turquoise;
}

void UZzNotifyNode_MotionWarping::OnBecomeRelevant(UMotionWarpingComponent* MotionWarpingComp, const UAnimSequenceBase* Animation, float InStartTime, float InEndTime)
{
	if (URootMotionModifier* RootMotionModifierNew = AddRootMotionModifier(MotionWarpingComp, Animation, InStartTime, InEndTime))
	{
		ModifierInstance = RootMotionModifierNew;
		if (!RootMotionModifierNew->OnActivateDelegate.IsBound())
		{
			RootMotionModifierNew->OnActivateDelegate.BindDynamic(this, &UZzNotifyNode_MotionWarping::OnRootMotionModifierActivate);
		}

		if (!RootMotionModifierNew->OnUpdateDelegate.IsBound())
		{
			RootMotionModifierNew->OnUpdateDelegate.BindDynamic(this, &UZzNotifyNode_MotionWarping::OnRootMotionModifierUpdate);
		}

		if (!RootMotionModifierNew->OnDeactivateDelegate.IsBound())
		{
			RootMotionModifierNew->OnDeactivateDelegate.BindDynamic(this, &UZzNotifyNode_MotionWarping::OnRootMotionModifierDeactivate);
		}
	}
}

URootMotionModifier* UZzNotifyNode_MotionWarping::AddRootMotionModifier_Implementation(UMotionWarpingComponent* MotionWarpingComp, const UAnimSequenceBase* Animation, float InStartTime, float InEndTime) const
{
	if (MotionWarpingComp && RootMotionModifier)
	{
		URootMotionModifier* NewModifier = MotionWarpingComp->AddModifierFromTemplate(RootMotionModifier, Animation, InStartTime, InEndTime);
		
		if (IZzRootMotionModifierInterface* ModifierInterface = Cast<IZzRootMotionModifierInterface>(NewModifier))
		{
			ModifierInterface->SetOwnerNode(const_cast<UZzNotifyNode_MotionWarping*>(this));
		}
		
		return NewModifier;
	}

	return nullptr;
}

void UZzNotifyNode_MotionWarping::OnRootMotionModifierActivate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier)
{
	OnWarpBegin(MotionWarpingComp, Modifier);
	for (auto Extender : Extensions)
	{
		if (auto MWExtender = Cast<UZzActionNodeExtender_MotionWarping>(Extender))
		{
			MWExtender->OnWarpBegin(MotionWarpingComp, Modifier);
		}
	}
}

void UZzNotifyNode_MotionWarping::OnRootMotionModifierUpdate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier)
{
	OnWarpUpdate(MotionWarpingComp, Modifier);
	for (auto Extender : Extensions)
	{
		if (auto MWExtender = Cast<UZzActionNodeExtender_MotionWarping>(Extender))
		{
			MWExtender->OnWarpUpdate(MotionWarpingComp, Modifier);
		}
	}
}

void UZzNotifyNode_MotionWarping::OnRootMotionModifierDeactivate(UMotionWarpingComponent* MotionWarpingComp, URootMotionModifier* Modifier)
{
	OnWarpEnd(MotionWarpingComp, Modifier);
	for (auto Extender : Extensions)
	{
		if (auto MWExtender = Cast<UZzActionNodeExtender_MotionWarping>(Extender))
		{
			MWExtender->OnWarpEnd(MotionWarpingComp, Modifier);
		}
	}
}

#if WITH_EDITOR
#include "Animation/DebugSkelMeshComponent.h"
namespace ZZ
{
	struct FFakedNotifyEvent : public FAnimNotifyEvent
	{
		FFakedNotifyEvent(float InBegin, float InEnd, FColor InColor)
		{
			TriggerTimeOffset = InBegin;
			SetDuration(InEnd - InBegin);
			NotifyColor = InColor;
			NotifyStateClass = GetMutableDefault<UAnimNotifyState>();
		}
	};

	class UDebugSkelMeshComp_GetRootRefTF : public UDebugSkelMeshComponent
	{
	public:
		FTransform& GetRootRefTF()
		{
			return RootMotionReferenceTransform;
		}
	};
}

void UZzNotifyNode_MotionWarping::OnPreviewEditorDraw(FPrimitiveDrawInterface* PDI) const
{
	/*if (RootMotionModifier)
	{
		if (auto AnimData = UZzActionAnimData::GetAnimData(GetNodeContext().GetTimeline()))
		{
			UAnimSequenceBase* Animation = AnimData->GetDefaultMontage();
			USkeletalMeshComponent* MeshComp = GetNodeContext().TryGetSkeletonComponent();
			if (Animation && MeshComp)
			{
				FTransform ActorTF = FTransform::Identity;
				ZzActionUtils::GetStartWorldTransform(MeshComp, true, ActorTF);
				TGuardValue MeshRot(static_cast<ZZ::UDebugSkelMeshComp_GetRootRefTF*>(MeshComp)->GetRootRefTF(), ActorTF);
				
				FAnimNotifyEvent FakedNotifyEvent = (FAnimNotifyEvent)ZZ::FFakedNotifyEvent(GetBeginTime(), GetEndTime(), FColor::Orange);

				// Necessary for FCompactPose (likely used by most RootMotionModifiers) that uses a FAnimStackAllocator (TMemStackAllocator) which allocates from FMemStack.
				// When allocating memory from FMemStack we need to explicitly use FMemMark to ensure items are freed when the scope exits. 
				FMemMark Mark(FMemStack::Get());
				RootMotionModifier->DrawInEditor(PDI, MeshComp, Animation, FakedNotifyEvent);
			}
		}
	}*/
}

void UZzNotifyNode_MotionWarping::OnPreviewEditorDrawCanvas(class FCanvas& Canvas, FSceneView& View) const
{
	/*if (RootMotionModifier)
	{
		if (auto AnimData = UZzActionAnimData::GetAnimData(GetNodeContext().GetTimeline()))
		{
			UAnimSequenceBase* Animation = AnimData->GetDefaultMontage();
			USkeletalMeshComponent* MeshComp = GetNodeContext().TryGetSkeletonComponent();
			if (Animation && MeshComp)
			{
				FTransform ActorTF = FTransform::Identity;
				ZzActionUtils::GetStartWorldTransform(MeshComp, true, ActorTF);
				TGuardValue MeshRot(static_cast<ZZ::UDebugSkelMeshComp_GetRootRefTF*>(MeshComp)->GetRootRefTF(), ActorTF);
				
				FAnimNotifyEvent FakedNotifyEvent = (FAnimNotifyEvent)ZZ::FFakedNotifyEvent(GetBeginTime(), GetEndTime(), FColor::Orange);

				// Necessary for FCompactPose (likely used by most RootMotionModifiers) that uses a FAnimStackAllocator (TMemStackAllocator) which allocates from FMemStack.
				// When allocating memory from FMemStack we need to explicitly use FMemMark to ensure items are freed when the scope exits. 
				FMemMark Mark(FMemStack::Get());
				RootMotionModifier->DrawCanvasInEditor(Canvas, View, MeshComp, Animation, FakedNotifyEvent);
			}
		}
	}*/
}
#endif
