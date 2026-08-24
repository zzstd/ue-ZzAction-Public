// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "ZzActionTrackModel_Sections.h"

#include "SZzActionSectionsTrack.h"
#include "ZzActionSectionsTrack.h"
#include "Timeline/ZzActionTimelineProxy.h"
#include "Timeline/Area/SZzActionTimelineTrackLaneView.h"
#include "Timeline/Head/SActionTimelineHeadRow.h"
#include "Timeline/Utils/ZzActionTimelineEditorUtils.h"

void FZzActionTrackModel_Sections::PostConstruct()
{
	FZzActionTimelineNameableTrackModel::PostConstruct();

	if (TrackIndex != 0)
	{
		TArray<UZzActionSectionsNode*> SectionsNodes;
		GetTimelineModel()->GetTimelineChecked()->ForEachNodes<UZzActionSectionsNode>([&SectionsNodes](UZzActionSectionsNode* Node)
		{
			SectionsNodes.Add(Node);
		}, true);
		
		if (SectionsNodes.Num() == 1)
		{
			GetTimelineModel()->GetProxy()->MoveTrack(TrackIndex, 0);
		}
	}
}

UClass* FZzActionTrackModel_Sections::GetTrackClass() const
{
	return UZzActionSectionsTrack::StaticClass();
}

TSharedRef<SWidget> FZzActionTrackModel_Sections::GenerateHeadWidget()
{
	return SNew(SActionTimelineHeadRow, SharedThis(this))
		.Visibility(this, &FZzActionTimelineNameableTrackModel::GetTrackVisibility)
		.RowHeight(this, &FZzActionTimelineTrackModel::GetTrackHeight)
		.IsRowHovered(this, &FZzActionTimelineTrackModel::IsHovered);
}

TSharedRef<SWidget> FZzActionTrackModel_Sections::GenerateTrackWidget()
{
	return SNew(SZzActionTimelineTrackLaneView, GetTimelineModel()->GetViewRangeAttribute())
		
		+ SZzActionTimelineTrackLaneView::Slot()
		.NodeHandle(GetUniqueNode())
		[
			FZzActionTimelineEditorUtils::MakeNodeBox(SAssignNew(SectionsNodeWidget, SZzActionSectionsTrack, SharedThis(this)))
		];
}

TSharedPtr<FZzActionTimelineNodeModel> FZzActionTrackModel_Sections::MakeNodeModel(const FZzActionNodeHandle& InHandle)
{
	return MakeShareable(new FZzActionTimelineNodeModel_Sections(SharedThis(this)));
}

void FZzActionTrackModel_Sections::BeginSnap(class FZzActionTimelineSnap& InSnap)
{
	FZzActionTimelineNameableTrackModel::BeginSnap(InSnap);

	for (auto Sect : GetUniqueNodeObject()->GetSections(false))
	{
		InSnap.AddTime(Sect.Time, FText::FromString(TEXT("Section: ") + Sect.Name.ToString()));
	}
}

void FZzActionTrackModel_Sections::RefreshMarqueeSelectedNodes(const FSlateRect& Rect,
	FZzActionTimelineMarqueeOperation& Marquee)
{
	GetTimelineModel()->Selection.SetSelection_Node(GetUniqueNode(), false);
}

void FZzActionTrackModel_Sections::OnSectionSelectionChanged()
{
	TGuardValue SelSP(bSelectionScoped, true);
	GetTimelineModel()->Selection.SetOnlySelection_Node(GetUniqueNode());
}

bool FZzActionTrackModel_Sections::HasAnySelected() const
{
	return SectionsNodeWidget->HasAnySelected();
}

void FZzActionTrackModel_Sections::ClearSelection()
{
	return SectionsNodeWidget->ClearSelection();
}

FZzActionTimelineNodeModel_Sections::FZzActionTimelineNodeModel_Sections(
	const TSharedRef<FZzActionTrackModel_Sections>& InTrackModel)
{
	TrackModel = InTrackModel;
}

void FZzActionTimelineNodeModel_Sections::OnNotifySelectionChanged()
{
	FZzActionTimelineNodeModel::OnNotifySelectionChanged();
	
	if (!TrackModel.Pin()->bSelectionScoped)
	{
		if (!IsSelected())
		{
			TrackModel.Pin()->ClearSelection();
		}
	}
}

/*
 * TODO: Selection
bool FZzActionTimelineNodeModel_Sections::IsSelected() const
{
	return bIsSelected || TrackModel.Pin()->HasAnySelected();
}
*/
