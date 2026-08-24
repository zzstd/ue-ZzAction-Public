// Copyright 2024-2026 zz studio. All Rights Reserved.


#include "SZzActionSectionsTrack.h"

#include "EditorFontGlyphs.h"
#include "SlateOptMacros.h"
#include "ZzActionSectionsTrack.h"
#include "ZzActionTrackModel_Sections.h"
#include "Timeline/Utils/ZzActionTimelineScaleInfo.h"
#include "Utils/ZzActionWidgetUtils.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SZzActionSectionLabel::Construct(const FArguments& InArgs, const TSharedRef<SZzActionSectionsTrack>& InOwner,
									  const TSharedRef<FZzActionTrackModel_Sections>& InTrack, int32 InSectionInx)
{
	OwnerWidget = InOwner;
	Track = InTrack;
	SectionInx = InSectionInx;

	ChildSlot
	[
		SNew(SBorder)
		.Padding(4, 0)
		.BorderImage(FAppStyle::GetBrush("SpecialEditableTextImageNormal"))
		.BorderBackgroundColor_Lambda([this]()
		{
			return bHasSelect
				? FLinearColor(0.0, 0.2, 0.6, 1.0f)
				: FLinearColor(0.153, 0.153, 0.361, 1.0f);
		})
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(EditableTextWidget, SEditableText)
				.Visibility(EVisibility::Collapsed)
				.Text(this, &SZzActionSectionLabel::GetSectionText)
				.AllowContextMenu(false)
				.IsCaretMovedWhenGainFocus(false)
				.OnTextCommitted(this, &SZzActionSectionLabel::OnTextCommitted)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(TextWidget, STextBlock)
				.Visibility(EVisibility::Visible)
				.Text(this, &SZzActionSectionLabel::GetSectionText)
			]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Font(FAppStyle::Get().GetFontStyle("FontAwesome.10"))
				.Text(this, &SZzActionSectionLabel::GetSectionIconText)
			]
		]
	];
}

FReply SZzActionSectionLabel::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (SectionInx != 0)
		{
			return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton).PreventThrottling();
		}
		return FReply::Handled();
	}
	return FReply::Handled();
}

FReply SZzActionSectionLabel::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bDrag)
	{
		bDrag = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	OwnerWidget.Pin()->OnSectionSelectionChanged(SectionInx);
	
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FMenuBuilder MenuBuilder(true, nullptr);
		{
			MenuBuilder.AddMenuEntry(
				INVTEXT("Rename")
				, INVTEXT("")
				, FSlateIcon()
				, FUIAction(
					FExecuteAction::CreateSP(this, &SZzActionSectionLabel::StartRename)
				));
			
			MenuBuilder.AddMenuEntry(
				INVTEXT("Delete Section")
				, INVTEXT("")
				, FSlateIcon()
				, FUIAction(
					FExecuteAction()
					, FCanExecuteAction::CreateLambda([this](){ return SectionInx != 0; })
				));

			MenuBuilder.AddSubMenu(
				INVTEXT("Link to..."),
				INVTEXT(""),
				FNewMenuDelegate::CreateRaw(this, &SZzActionSectionLabel::FillLinkSectionsMenu));
		}
		auto MenuWidget = MenuBuilder.MakeWidget();
		return ZZ::Action::Widget::PopContextMenu(SharedThis(this), MouseEvent, MenuWidget);
	}
	
	return FReply::Handled();
}

FReply SZzActionSectionLabel::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bDrag = true;
	return FReply::Handled().CaptureMouse(SharedThis(this));
}

FReply SZzActionSectionLabel::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bDrag)
	{
		if (SectionInx != 0)
		{
			double MouseTime = FZzActionTimelineScaleInfo(Track.Pin().Get())
				.LocalPosToTime(MouseEvent.GetScreenSpacePosition(), OwnerWidget.Pin()->GetCachedGeometry());

			auto Obj = Track.Pin()->GetUniqueNodeObject();
			Obj->GetSectionRef(SectionInx).Time = MouseTime;
		}
		return FReply::Handled();
	}
	
	return SCompoundWidget::OnMouseMove(MyGeometry, MouseEvent);
}

void SZzActionSectionLabel::FillLinkSectionsMenu(FMenuBuilder& MenuBuilder)
{
	for (int32 i = 0; i < Track.Pin()->GetUniqueNodeObject()->GetSectionsNum(); i++)
	{
		auto SectData = Track.Pin()->GetUniqueNodeObject()->GetSections()[i];
		
		MenuBuilder.AddMenuEntry(
			FText::FromName(SectData.Name)
			, INVTEXT("")
			, FSlateIcon()
			, FUIAction(FExecuteAction::CreateSP(this, &SZzActionSectionLabel::LinkTo, SectData.Name)));
	}
}

FText SZzActionSectionLabel::GetSectionText() const
{
	if (Track.IsValid())
	{
		auto SectData = Track.Pin()->GetUniqueNodeObject()->GetSectionRef(SectionInx);

		if (SectionInx != 0)
		{
			auto& Line = Track.Pin()->GetTimelineModel()->TimeLinesVisualization.GetInfo(FName(FString::Printf(TEXT("Section_%d"), SectionInx)));
			Line.bActive = true;
			Line.Thickness = 1;
			Line.Color = bHasSelect
					? FLinearColor(0.0, 0.2, 0.6, 1.0f)
					: FLinearColor(0.153, 0.153, 0.361, 1.0f);
			Line.Time = SectData.Time;
		}
		return FText::FromName(SectData.Name);
	}

	return FText();
}

FText SZzActionSectionLabel::GetSectionIconText() const
{
	if (Track.IsValid())
	{
		int32 NextSectionIndex = Track.Pin()->GetUniqueNodeObject()->GetNextSectionIndex(SectionInx);

		FText IconText;
		if(NextSectionIndex == INDEX_NONE)
		{
			IconText = FText::GetEmpty();
		}
		else if(NextSectionIndex == SectionInx)
		{
			IconText = FEditorFontGlyphs::Undo;
		}
		else if(NextSectionIndex > SectionInx)
		{
			IconText = FEditorFontGlyphs::Arrow_Right;
		}
		else if(NextSectionIndex < SectionInx)
		{
			IconText = FEditorFontGlyphs::Arrow_Left;
		}
		
		return IconText;
	}

	return FText();
}


void SZzActionSectionLabel::StartRename()
{
	EditableTextWidget->SetVisibility(EVisibility::Visible);
	TextWidget->SetVisibility(EVisibility::Collapsed);
	FSlateApplication::Get().SetKeyboardFocus(EditableTextWidget);
	EditableTextWidget->SelectAllText();
}

void SZzActionSectionLabel::OnTextCommitted(const FText& NewText, ETextCommit::Type InType)
{
	EditableTextWidget->SetVisibility(EVisibility::Collapsed);
	TextWidget->SetVisibility(EVisibility::Visible);

	auto Obj = Track.Pin()->GetUniqueNodeObject();
	Obj->Modify();
	Obj->RenameSection(Obj->GetSectionRef(SectionInx).Name, FName(NewText.ToString()));
}

void SZzActionSectionLabel::LinkTo(FName InLinkName)
{
	auto Obj = Track.Pin()->GetUniqueNodeObject();
	Obj->Modify();
	Obj->GetSectionRef(SectionInx).LinkSection = InLinkName;
}

void SZzActionSectionsTrack::Construct(const FArguments& InArgs, const TSharedRef<FZzActionTrackModel_Sections>& InTrack)
{
	Track = InTrack;
	ChildSlot[SAssignNew(Box, SOverlay)];
	RefreshSelections();
}

FReply SZzActionSectionsTrack::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

FReply SZzActionSectionsTrack::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FMenuBuilder MenuBuilder(true, nullptr);
		{
			double RightClickTime = FZzActionTimelineScaleInfo(Track.Pin().Get())
				.LocalPosToTime(MouseEvent.GetScreenSpacePosition(), GetCachedGeometry());

			MenuBuilder.AddMenuEntry(
				INVTEXT("Add Section")
				, INVTEXT("")
				, FSlateIcon()
				, FUIAction(FExecuteAction::CreateSP(this, &SZzActionSectionsTrack::AddNewSection, RightClickTime)));
		}
		auto MenuWidget = MenuBuilder.MakeWidget();
		return ZZ::Action::Widget::PopContextMenu(SharedThis(this), MouseEvent, MenuWidget);
	}

	return FReply::Unhandled();
}

void SZzActionSectionsTrack::AddNewSection(double Time)
{
	auto Obj = Track.Pin()->GetUniqueNodeObject();
	Obj->Modify();
	
	FString UniqueSectionName = TEXT("NewSection");
	{
		int32 i = 0;
		while (Obj->GetSection(*UniqueSectionName))
		{
			UniqueSectionName = TEXT("NewSection") + FString::FromInt(++i);
		}
	}

	FActionNodeSectionData Info;
	Info.Name = FName(UniqueSectionName);
	Info.Time = Time;
	
	auto StInx = Obj->GetSectionIndexByTime(Time);
	check(StInx != INDEX_NONE)
	Info.LinkSection = Obj->GetSectionRef(StInx).LinkSection;
	Obj->GetSectionRef(StInx).LinkSection = Info.Name;

	int32 NewIndex = Obj->Sections.Insert(Info, StInx) + 1;
	
	RefreshSelections();
	Sections[NewIndex]->StartRename();
}

void SZzActionSectionsTrack::RefreshSelections()
{
	Box->ClearChildren();
	Sections.Empty();
	for (int32 i = 0; i < Track.Pin()->GetUniqueNodeObject()->GetSectionsNum(); i++)
	{
		TSharedRef<SZzActionSectionLabel> NewItem = SNew(SZzActionSectionLabel, SharedThis(this), Track.Pin().ToSharedRef(), i);
		Sections.Add(NewItem.ToSharedPtr());
			
		Box->AddSlot()
		.VAlign(VAlign_Center)
		.Padding(MakeAttributeSP(this, &SZzActionSectionsTrack::GetSectionPadding, i))
		[
			NewItem
		];
	}
}

void SZzActionSectionsTrack::OnSectionSelectionChanged(int32 Inx)
{
	for (int32 i = 0; i < Sections.Num(); i++)
	{
		Sections[i]->bHasSelect = i == Inx;
	}
	Track.Pin()->OnSectionSelectionChanged();
}

bool SZzActionSectionsTrack::HasAnySelected() const
{
	for (int32 i = 0; i < Sections.Num(); i++)
	{
		if (Sections[i]->bHasSelect)
		{
			return true;
		}
	}

	return false;
}

void SZzActionSectionsTrack::ClearSelection()
{
	for (int32 i = 0; i < Sections.Num(); i++)
	{
		Sections[i]->bHasSelect = false;
	}
}

FMargin SZzActionSectionsTrack::GetSectionPadding(int32 Inx) const
{
	if (auto StrongTrackModel = Track.Pin())
	{
		const float TrackSizeX = GetCachedGeometry().GetLocalSize().X;
			
		const float LocalTime = FZzActionTimelineScaleInfo(StrongTrackModel.Get())
			.TimeToLocal(Track.Pin()->GetUniqueNodeObject()->GetSections()[Inx].Time, TrackSizeX);
			
		const float WidgetLength = Sections[Inx]->GetDesiredSize().X;
		const float Offset = 0.f;
			
		return FMargin(LocalTime + Offset, 0, TrackSizeX - (LocalTime  + Offset + WidgetLength), 0);
	}
	return FMargin();
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
