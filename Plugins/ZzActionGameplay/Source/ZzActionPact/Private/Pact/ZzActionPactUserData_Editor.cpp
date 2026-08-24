// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "Pact/ZzActionPactUserData.h"

#include "ZzActionBlueprint.h"
#include "ZzActionHelperUtils.h"
#include "ZzActionUtils.h"
#include "MotionWarping/ZzNotifyNode_MotionWarping.h"
#include "Nodes/ZzActionLinkTrack.h"
#include "Nodes/Anim/ZzActionAnimFunctionLibrary.h"
#include "Timeline/ZzActionTimeline.h"


void UZzActionPactUserData::UpdateLinks()
{
	Links.Empty();
	{
		// ensure index 0 is owner;
		Links.AddDefaulted();
		Links[0].LinkGUID = FGuid();
	}
	
	auto ActionAsset = Cast<UZzActionBlueprint>(GetOuter());
	if (!ActionAsset)
		return;
	
	for (auto LinkNode : ActionAsset->GetTimeline()->FindNodes<UZzActionLinkNode>())
	{
		ensureAlways(LinkNode->LinkGUID.IsValid());
		auto& Info = Links.AddDefaulted_GetRef();
		Info.LinkGUID = LinkNode->LinkGUID;
	}
	
	AnchorRelativeTransform = GetAnchorLinkInfo().GetTransformWS(ActionAsset->GetTimeline(), 0);
	UpdateLinksOffset();
}

void UZzActionPactUserData::UpdateLinksOffset()
{
	auto ActionAsset = Cast<UZzActionBlueprint>(GetOuter());
	auto MainTimeline = ActionAsset ? ActionAsset->GetTimeline() : nullptr;
	if (!MainTimeline)
		return;
	
	if (!Links.IsValidIndex(AnchorLinkIndex))
		return;
	
	for (int32 i = 0; i < Links.Num(); ++i)
	{
		auto& LinkInfo = Links[i];
		LinkInfo.bHasMotionWarpNode = false;
		LinkInfo.LocationOffset = FVector::ZeroVector;
		LinkInfo.RotationOffset = FRotator::ZeroRotator;
		
		if (AnchorLinkIndex == i)
			continue;
		
		auto MotionWarpNode = LinkInfo.GetMotionWarpingNode(MainTimeline);
		if (!MotionWarpNode)
			continue;
		
		LinkInfo.bHasMotionWarpNode = true;
		
		const FTransform WarpBeginTargetWP = GetAnchorLinkInfo().GetTransformWS(MainTimeline, MotionWarpNode->GetBeginTime());
		const FTransform WarpEndWP = LinkInfo.GetTransformWS(MainTimeline, MotionWarpNode->GetEndTime());
		
		LinkInfo.LocationOffset = WarpBeginTargetWP.InverseTransformPosition(WarpEndWP.GetLocation());
		LinkInfo.RotationOffset = WarpBeginTargetWP.InverseTransformRotation(WarpEndWP.GetRotation()).Rotator();
	}
}

void UZzActionPactUserData::Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const
{
	UZzActionTimeline* MainTimeline = Cast<UZzActionBlueprint>(GetOuter())->GetTimeline();
	check(MainTimeline);
	
	for (int32 i = 0; i < Links.Num(); ++i)
	{
		if (!DrawDebugLinks.Contains(i))
			continue;
		
		auto& LinkInfo = Links[i];
		const UZzActionTimeline* LinkTimeline = LinkInfo.GetLinkTimeline(MainTimeline);
		if (!LinkTimeline)
			continue;
		
		FTransform LocalToWorld;
		if (!ZZ::Action::Helper::GetPreviewStartWorldTransform(LinkTimeline, false, LocalToWorld))
			continue;
		
		if (auto MotionWarpNode = LinkInfo.GetMotionWarpingNode(MainTimeline))
		{
			check(LinkTimeline == MotionWarpNode->GetTimelineAsset());
			
			const float WarpBeginTime = MotionWarpNode->GetBeginTime();
			const float WarpEndTime = MotionWarpNode->GetEndTime();
			
			FTransform WarpStartTF = ZZ::Action::ExtractRootMotionWithCharacter(LinkTimeline, 0, WarpBeginTime);
			FTransform WarpEndTF = ZZ::Action::ExtractRootMotionWithCharacter(LinkTimeline, 0, WarpEndTime);
			
			const FTransform TargetOffset(LinkInfo.RotationOffset.Quaternion(), LinkInfo.LocationOffset);
			FTransform TargetTF = TargetOffset.Inverse() * WarpEndTF;

			auto WorldStart = WarpStartTF * LocalToWorld;
			auto WorldEnd = WarpEndTF * LocalToWorld;
			auto WorldTarget =TargetTF * LocalToWorld;

			DrawWireBox(PDI, FBox(WorldEnd.GetLocation(), WorldTarget.GetLocation()), FLinearColor::White, SDPG_Foreground);
			
			PDI->DrawPoint(WorldStart.GetLocation(), FLinearColor::Green, 7, SDPG_Foreground);
			PDI->DrawPoint(WorldEnd.GetLocation(), FLinearColor::Blue, 7, SDPG_Foreground);
			PDI->DrawPoint(WorldTarget.GetLocation(), FLinearColor::Red, 7, SDPG_Foreground);
			ZZ::Action::Helper::DrawCoordinateSystem(PDI, WorldStart, 5, 10.0f, 2, true, 200);
			ZZ::Action::Helper::DrawCoordinateSystem(PDI, WorldEnd, 5, 10.0f, 2, true, 200);
			ZZ::Action::Helper::DrawCoordinateSystem(PDI, WorldTarget, 5, 10.0f, 2, true, 200);

			constexpr float DrawTimeStep = 1.0 / 30.0;
			FVector p0 = WorldStart.GetLocation();
			for (float t = WarpBeginTime + DrawTimeStep; t < WarpEndTime; t += DrawTimeStep)
			{
				auto r = ZZ::Action::ExtractRootMotionWithCharacter(LinkTimeline, 0, t);
				auto p1 = LocalToWorld.TransformPosition(r.GetLocation());
				PDI->DrawTranslucentLine(p0, p1, FColor::Orange, SDPG_World, 1.0f, 2, true);
				p0 = p1;
			}
			PDI->DrawTranslucentLine(p0, WorldEnd.GetLocation(), FColor::Orange, SDPG_World, 1.0f, 2, true);
		}
	}
}

void UZzActionPactUserData::PostEditChangeOwner(const FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateLinksOffset();
}