// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Pact/ZzActionPactUserData.h"

#include "RootMotionModifier.h"

#include "ZzActionBlueprint.h"
#include "Timeline/ZzActionTimeline.h"
#include "Nodes/ZzActionLinkTrack.h"
#include "MotionWarping/ZzNotifyNode_MotionWarping.h"
#include "Nodes/Anim/ZzActionAnimFunctionLibrary.h"


UZzActionLinkNode* FZzActionPactLinkInfo::GetLinkNode(const UZzActionTimeline* MainTimeline) const
{
	for (auto LinkNode : MainTimeline->FindNodes<UZzActionLinkNode>())
	{
		if (LinkNode->LinkGUID == LinkGUID)
		{
			return LinkNode;
		}
	}
	return nullptr;
}

UZzActionTimeline* FZzActionPactLinkInfo::GetLinkTimeline(UZzActionTimeline* MainTimeline) const
{
	const UZzActionTimeline* ConstTimeline = MainTimeline;
	return const_cast<UZzActionTimeline*>(GetLinkTimeline(ConstTimeline));
}

const UZzActionTimeline* FZzActionPactLinkInfo::GetLinkTimeline(const UZzActionTimeline* MainTimeline) const
{
	if (!LinkGUID.IsValid())
		return MainTimeline;
	
	auto LinkNode = GetLinkNode(MainTimeline);
	if (LinkNode && LinkNode->LinkActionAsset)
		return LinkNode->LinkActionAsset->GetTimeline();
	return nullptr;
}

FTransform FZzActionPactLinkInfo::GetTransformWS(const UZzActionTimeline* MainTimeline, float MainTime) const
{
	if (!LinkGUID.IsValid())
		return ZZ::Action::ExtractRootMotionWithCharacter(MainTimeline, 0, MainTime);

	UZzActionLinkNode* TargetLinkNode = GetLinkNode(MainTimeline);
	if (!TargetLinkNode)
		return FTransform::Identity;

	auto WarpBeginTime = MainTime;
	WarpBeginTime -= TargetLinkNode->GetBeginTime();

	FTransform TargetTF = FTransform::Identity;
	if (TargetLinkNode->LinkActionAsset)
		TargetTF = ZZ::Action::ExtractRootMotionWithCharacter(TargetLinkNode->LinkActionAsset->GetTimeline(), 0, WarpBeginTime);
	TargetTF *= TargetLinkNode->RelativeTransform;
	return TargetTF;
}

UZzNotifyNode_MotionWarping* FZzActionPactLinkInfo::GetMotionWarpingNode(const UZzActionTimeline* MainTimeline) const
{
	if (auto Timeline = GetLinkTimeline(MainTimeline))
	{
		UZzNotifyNode_MotionWarping* Result = nullptr;
		Timeline->ForEachNodes<UZzNotifyNode_MotionWarping>([&Result](UZzNotifyNode_MotionWarping* Node)
		{
			if (Cast<URootMotionModifier_Warp>(Node->RootMotionModifier))
			{
				Result = Node;
			}
			return Result == nullptr;
		});
		
		return Result;
	}
	return nullptr;
}

FGuid UZzActionPactUserData::GetAnchorLinkGUID() const
{
	if (ensure(Links.IsValidIndex(AnchorLinkIndex)))
		return Links[AnchorLinkIndex].LinkGUID;
	
	return FGuid();
}

const FZzActionPactLinkInfo& UZzActionPactUserData::GetAnchorLinkInfo() const
{
	if (Links.IsValidIndex(AnchorLinkIndex))
		return Links[AnchorLinkIndex];
	
	static FZzActionPactLinkInfo None;
	return None;
}

bool UZzActionPactUserData::IsOwnerAnchor() const
{
	return AnchorLinkIndex == 0;
}

bool UZzActionPactUserData::GetLinkOffset(FGuid LinkGUID, FVector& Loc, FRotator& Rot) const
{
	for (auto& a : Links)
	{
		if (a.LinkGUID == LinkGUID)
		{
			Loc = a.LocationOffset;
			Rot = a.RotationOffset;
			return true;
		}
	}
	
	return false;
}

FZzActionPactLinkInfo* UZzActionPactUserData::GetLinkInfo(FGuid LinkGUID)
{
	for (int32 i = 0; i < Links.Num(); ++i)
	{
		if (Links[i].LinkGUID == LinkGUID)
		{
			return &Links[i];
		}
	}
	
	return nullptr;
}