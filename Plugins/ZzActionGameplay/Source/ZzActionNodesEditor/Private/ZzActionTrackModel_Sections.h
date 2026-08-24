// Copyright 2024-2026 zz studio. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Timeline/ZzActionTimelineNodeModel.h"
#include "Timeline/ZzActionTimelineTrackModel.h"
#include "Timeline/ZzActionTimelineUniqueNodeProvider.h"

class UZzActionSectionsNode;

class FZzActionTrackModel_Sections
	: public FZzActionTimelineNameableTrackModel
	, public TZzActionTimelineUniqueNodeProvider<FZzActionTrackModel_Sections, UZzActionSectionsNode>
{
public:
	static TSharedRef<FZzActionTimelineTrackModel> MakeInstance() 
	{
		return MakeShareable(new FZzActionTrackModel_Sections());
	}

	virtual void PostConstruct() override;
	virtual UClass* GetTrackClass() const override;
	virtual TSharedRef<SWidget> GenerateHeadWidget() override;
	virtual TSharedRef<SWidget> GenerateTrackWidget() override;
	virtual TSharedPtr<FZzActionTimelineNodeModel> MakeNodeModel(const FZzActionNodeHandle& InHandle) override;
	virtual void BeginSnap(class FZzActionTimelineSnap& InSnap) override;
	virtual void RefreshMarqueeSelectedNodes(const FSlateRect& Rect, FZzActionTimelineMarqueeOperation& Marquee) override;

	void OnSectionSelectionChanged();
	bool bSelectionScoped = false;
	bool HasAnySelected() const;
	void ClearSelection();
private:
	TSharedPtr<class SZzActionSectionsTrack> SectionsNodeWidget;
};

class FZzActionTimelineNodeModel_Sections : public FZzActionTimelineNodeModel
{
public:
	virtual ~FZzActionTimelineNodeModel_Sections() override {}

	FZzActionTimelineNodeModel_Sections(const TSharedRef<FZzActionTrackModel_Sections>& InTrackModel);

protected:
	virtual void OnNotifySelectionChanged() override;
	
	TWeakPtr<FZzActionTrackModel_Sections> TrackModel;
};