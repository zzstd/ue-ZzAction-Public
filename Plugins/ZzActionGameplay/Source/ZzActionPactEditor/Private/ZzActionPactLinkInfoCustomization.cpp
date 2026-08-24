// Copyright 2024-2026 zz studio. All Rights Reserved.

#include "ZzActionPactLinkInfoCustomization.h"

#include "Pact/ZzActionPactUserData.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "RootMotionModifier.h"
#include "ZzActionBlueprint.h"
#include "ZzActionPactEditorStyle.h"
#include "ZzActionTimelineUtils.h"
#include "Nodes/Anim/ZzActionAnimFunctionLibrary.h"
#include "Timeline/ZzActionTimeline.h"
#include "ZzActionNodes/Public/MotionWarping/ZzNotifyNode_MotionWarping.h"

void FZzActionPactLinkInfoCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
														 IPropertyTypeCustomizationUtils& Utils)
{
	CurrentIndex = PropertyHandle->GetIndexInArray();
	check(CurrentIndex != INDEX_NONE);
	
	TArray<UObject*> Objects;
	PropertyHandle->GetOuterObjects(Objects);
	PactAnimationUserData = Objects.IsEmpty() ? nullptr : Cast<UZzActionPactUserData>(Objects[0]);
	check(PactAnimationUserData != nullptr);

	HeaderRow.WholeRowContent()
	[
		SNew(SHorizontalBox)
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0, 0, 8, 0)
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(CurrentIndex == 0 ? INVTEXT("Owner ") : FText::FromString(FString::Printf(TEXT("Link %d"), CurrentIndex)))
			]
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				PactAnimationUserData->AnchorLinkIndex = CurrentIndex;
				PactAnimationUserData->UpdateLinks();
				return FReply::Handled();
			})
			.IsEnabled_Lambda([this]()
			{
				return PactAnimationUserData->AnchorLinkIndex != CurrentIndex;
			})
			[
				SNew(STextBlock).Text(INVTEXT("Anchor"))
			]
		]
		
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked_Lambda([this]()
			{
				if (PactAnimationUserData->DrawDebugLinks.Contains(CurrentIndex))
					PactAnimationUserData->DrawDebugLinks.Remove(CurrentIndex);
				else
					PactAnimationUserData->DrawDebugLinks.Add(CurrentIndex);
				return FReply::Handled();
			})
			[
				SNew(SImage)
				.Image(FZzActionPactEditorStyle::Get().GetBrush("PactEditor.Debug"))
				.ColorAndOpacity_Lambda([this]() -> FSlateColor
				{
					if (PactAnimationUserData->DrawDebugLinks.Contains(CurrentIndex))
						return FLinearColor::Green;
					return FLinearColor::White;
				})
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(FixBox, SBox)
		]
	];

	Update();
	
	// TODO: When hovering the mouse over the details panel options, draw the bounding box of the current preview Action
}

void FZzActionPactLinkInfoCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& Utils)
{
	uint32 NumberOfChild;
	if (PropertyHandle->GetNumChildren(NumberOfChild) == FPropertyAccess::Success)
	{
		for (uint32 Index = 0; Index < NumberOfChild; ++Index)
		{
			if (auto Child = PropertyHandle->GetChildHandle(Index))
			{
				ChildBuilder.AddProperty(Child.ToSharedRef());
			}
		}
	}
}

void FZzActionPactLinkInfoCustomization::Update()
{
	FixBox->SetContent(MakeMotionWarpUtilsWidget());
}

TSharedRef<SWidget> FZzActionPactLinkInfoCustomization::MakeMotionWarpUtilsWidget()
{
	TArray<FMsgData> Msgs;
	UpdateMsg(Msgs);
	
	if (Msgs.IsEmpty())
	{
		return SNullWidget::NullWidget;
	}
	
	auto Box = SNew(SHorizontalBox);
	
	for (auto& m : Msgs)
	{
		Box->AddSlot()
		.AutoWidth()
		[
			SNew(SButton)
			.OnClicked(m.OnClicked)
			.ToolTipText(FText::FromString(m.Msg))
			[
				SNew(STextBlock).Text(FText::FromString(m.ClickText))
			]
		];
	}
	
	return Box;
}

void FZzActionPactLinkInfoCustomization::UpdateMsg(TArray<FMsgData>& List)
{
	int32 MotionWarpNum = 0;
	TWeakObjectPtr<UZzActionTimeline> WeakLinkTimeline;
	TWeakObjectPtr<UZzNotifyNode_MotionWarping> WeakMWNode;
	TWeakObjectPtr<URootMotionModifier_Warp> WeakWarpModifier;
	{
		auto ActionAsset = Cast<UZzActionBlueprint>(PactAnimationUserData->GetOuter());
		auto MainTimeline = ActionAsset ? ActionAsset->GetTimeline() : nullptr;

		if (auto Timeline = PactAnimationUserData->Links[CurrentIndex].GetLinkTimeline(MainTimeline))
		{
			WeakLinkTimeline = Timeline;
			Timeline->ForEachNodes<UZzNotifyNode_MotionWarping>(
			[&MotionWarpNum, &WeakWarpModifier, &WeakMWNode](UZzNotifyNode_MotionWarping* Node)
			{
				if (auto wm = Cast<URootMotionModifier_Warp>(Node->RootMotionModifier))
				{
					MotionWarpNum++;
					WeakMWNode = Node;
					WeakWarpModifier = wm;
				}
			});
		}
	}
	
	if (MotionWarpNum == 0)
	{
		auto& m = List.AddDefaulted_GetRef();
		m.Msg = TEXT("Click to quickly create a MotionWarpNode");
		m.Level = 0;
		m.ClickText = TEXT("Add Motion Warp");
		m.OnClicked = FOnClicked::CreateLambda([WeakLinkTimeline, this]()
		{
			if (auto StrongTimeline = WeakLinkTimeline.Get())
			{
				FScopedTransaction Transaction(INVTEXT("Quick Add Motion Warp Node"));
				
				auto NewTrack = ZZ::Action::Timeline::CreateNotifyTrack(StrongTimeline);
				auto NewMWNode = ZZ::Action::Timeline::CreateNotifyNode<UZzNotifyNode_MotionWarping>(NewTrack, 0, 1);
				
				if (auto WarpModifier = Cast<URootMotionModifier_Warp>(NewMWNode->RootMotionModifier))
				{
					WarpModifier->WarpTargetName = TEXT("WarpTarget");
				}
			}
			Update();
			return FReply::Handled();
		});
	}
	else if (MotionWarpNum == 1)
	{
		if (WeakWarpModifier.IsValid())
		{
			// @TODO: Warn when the Motion Warping node has no valid target name.
			if (WeakWarpModifier->WarpTargetName.IsNone())
			{
				auto& m = List.AddDefaulted_GetRef();
				m.Level = 1;
				m.Msg = TEXT("Warp Target Name not set");
				m.ClickText = TEXT("Fix TargetName");
				m.OnClicked = FOnClicked::CreateLambda([WeakWarpModifier, this](){
					FScopedTransaction Transaction(INVTEXT("Fix Warp Target Name"));
					
					WeakWarpModifier->Modify();
					WeakWarpModifier->WarpTargetName = TEXT("WarpTarget");
					Update();
					return FReply::Handled();
				});
			}
		}
		
		if (WeakLinkTimeline.IsValid() && WeakMWNode.IsValid() && WeakWarpModifier.IsValid())
		{
			auto RootMotionTF = ZZ::Action::ExtractRootMotionFromTimeline(WeakLinkTimeline.Get(), WeakMWNode->GetBeginTime(), WeakMWNode->GetEndTime());
			if (WeakWarpModifier.Get()->bWarpTranslation)
			{
				if (RootMotionTF.GetLocation().IsNearlyZero())
				{
					auto& m = List.AddDefaulted_GetRef();
					m.Level = 1;
					m.Msg = TEXT("Motion Warping does not motion");
					m.ClickText = TEXT("Err RootMotion");
				}
				
				if (WeakWarpModifier->bIgnoreZAxis)
				{
					if (FMath::Abs(RootMotionTF.GetLocation().Z) > 10)
					{
						auto& m = List.AddDefaulted_GetRef();
						m.Level = 1;
						m.Msg = TEXT("Including Z motion, but Motion Warping ignores the z-axis");
						m.ClickText = TEXT("Err RootMotion");
					}
				}
			}
		}
	}
	else if (MotionWarpNum > 1)
	{
		auto& m = List.AddDefaulted_GetRef();
		m.Level = 2;
		m.Msg = TEXT("Not supporting multiple MotionWarpNodes");
		m.ClickText = TEXT("Err");
	}
}
