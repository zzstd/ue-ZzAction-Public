// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzNotifyNode_DisableRootMotion.h"

#include "Timeline/ZzActionTimeline.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Nodes/Anim/ZzActionAnimNode.h"
#include "Nodes/Anim/ZzActionAnimRuntime.h"

UZzNotifyNode_DisableRootMotion::UZzNotifyNode_DisableRootMotion()
{
	Tags.AddUnique(TEXT("Gameplay"));
}

void UZzNotifyNode_DisableRootMotion::OnBegin()
{
	Super::OnBegin();
	
	if (FAnimMontageInstance* MontageInstance = GetAnimMontageInstance())
	{
		MontageInstance->PushDisableRootMotion();
	}
}

void UZzNotifyNode_DisableRootMotion::OnEnd(bool Abort)
{
	Super::OnEnd(Abort);
	
	if (FAnimMontageInstance* MontageInstance = GetAnimMontageInstance())
	{
		MontageInstance->PopDisableRootMotion();
	}
}

FAnimMontageInstance* UZzNotifyNode_DisableRootMotion::GetAnimMontageInstance() const
{
	if (auto AnimRuntime = GetNodeContext().GetAnimRuntime())
	{
		return AnimRuntime->GetMontageInstance();
	}

	return nullptr;
}
