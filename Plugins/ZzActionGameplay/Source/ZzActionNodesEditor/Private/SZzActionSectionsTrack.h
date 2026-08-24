// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Timeline/Utils/ZzActionTimelineSnap.h"
#include "Widgets/SCompoundWidget.h"


class FZzActionTrackModel_Sections;
class SZzActionSectionsTrack;


/*
 * @TODO: Support renaming with F2 and deletion with the Delete key.
 */
class SZzActionSectionLabel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SZzActionSectionLabel)
	{
	}
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, const TSharedRef<SZzActionSectionsTrack>& InOwner, const TSharedRef<FZzActionTrackModel_Sections>& InTrack, int32 InSectionInx);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void FillLinkSectionsMenu(FMenuBuilder& MenuBuilder);

	void StartRename();
	void OnTextCommitted(const FText& NewText, ETextCommit::Type InType);
	void LinkTo(FName InLinkName);
	
	FText GetSectionText() const;
	FText GetSectionIconText() const;

	int32 SectionInx;
	bool bHasSelect;
	bool bDrag;
	TWeakPtr<FZzActionTrackModel_Sections> Track;
	TWeakPtr<SZzActionSectionsTrack> OwnerWidget;

	TSharedPtr<SEditableText> EditableTextWidget;
	TSharedPtr<STextBlock> TextWidget;
};

class SZzActionSectionsTrack : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SZzActionSectionsTrack)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FZzActionTrackModel_Sections>& InTrack);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void AddNewSection(double Time);

	void RefreshSelections();

	void OnSectionSelectionChanged(int32 Inx);
	
	bool HasAnySelected() const;
	
	void ClearSelection();

private:
	FMargin GetSectionPadding(int32 Inx) const;
	
	TWeakPtr<FZzActionTrackModel_Sections> Track;
	TSharedPtr<SOverlay> Box;
	TArray<TSharedPtr<SZzActionSectionLabel>> Sections;
};