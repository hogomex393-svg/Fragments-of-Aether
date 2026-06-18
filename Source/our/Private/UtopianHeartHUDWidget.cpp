#include "UtopianHeartHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateColor.h"

void UUtopianHeartHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    BuildDefaultWidgetTree();
}

void UUtopianHeartHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BuildDefaultWidgetTree();
    SetHeartProgress(0, 15);
}

void UUtopianHeartHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (PickupMessageRemainingSeconds <= 0.0f)
    {
        return;
    }

    PickupMessageRemainingSeconds = FMath::Max(0.0f, PickupMessageRemainingSeconds - InDeltaTime);
    if (PickupMessageRemainingSeconds <= 0.0f && PickupPanel)
    {
        PickupPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UUtopianHeartHUDWidget::SetHeartProgress(int32 CollectedCount, int32 TargetCount)
{
    BuildDefaultWidgetTree();

    if (ProgressLabel)
    {
        ProgressLabel->SetText(FText::Format(
            ProgressFormat,
            FText::AsNumber(CollectedCount),
            FText::AsNumber(TargetCount)));
    }
}

void UUtopianHeartHUDWidget::ShowCompletionMessage()
{
    BuildDefaultWidgetTree();

    if (ProgressLabel)
    {
        ProgressLabel->SetText(CompletionText);
    }
}

void UUtopianHeartHUDWidget::ShowPickupMessage(int32 CollectedCount, int32 TargetCount)
{
    BuildDefaultWidgetTree();

    if (PickupLabel)
    {
        PickupLabel->SetText(FText::Format(
            PickupMessageFormat,
            FText::AsNumber(CollectedCount),
            FText::AsNumber(TargetCount)));
    }

    if (PickupPanel)
    {
        PickupPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    PickupMessageRemainingSeconds = PickupMessageDurationSeconds;
}

void UUtopianHeartHUDWidget::BuildDefaultWidgetTree()
{
    if (!WidgetTree || RootCanvas)
    {
        return;
    }

    RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("UtopianHeartHUDRoot"));
    WidgetTree->RootWidget = RootCanvas;

    HudPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("HeartProgressPanel"));
    HudPanel->SetBrushColor(FLinearColor(0.02f, 0.02f, 0.025f, 0.76f));
    HudPanel->SetPadding(FMargin(18.0f, 10.0f));
    RootCanvas->AddChild(HudPanel);

    if (UCanvasPanelSlot* HudSlot = Cast<UCanvasPanelSlot>(HudPanel->Slot))
    {
        HudSlot->SetAnchors(FAnchors(1.0f, 0.0f));
        HudSlot->SetAlignment(FVector2D(1.0f, 0.0f));
        HudSlot->SetPosition(FVector2D(-32.0f, 32.0f));
        HudSlot->SetAutoSize(true);
    }

    ProgressLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeartProgressLabel"));
    ProgressLabel->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.84f, 0.88f, 1.0f)));
    ProgressLabel->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 26));
    HudPanel->SetContent(ProgressLabel);

    PickupPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("HeartPickupPanel"));
    PickupPanel->SetBrushColor(FLinearColor(0.02f, 0.02f, 0.025f, 0.82f));
    PickupPanel->SetPadding(FMargin(18.0f, 10.0f));
    PickupPanel->SetVisibility(ESlateVisibility::Collapsed);
    RootCanvas->AddChild(PickupPanel);

    if (UCanvasPanelSlot* PickupSlot = Cast<UCanvasPanelSlot>(PickupPanel->Slot))
    {
        PickupSlot->SetAnchors(FAnchors(0.5f, 0.18f));
        PickupSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        PickupSlot->SetAutoSize(true);
    }

    PickupLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeartPickupLabel"));
    PickupLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.94f, 1.0f, 1.0f)));
    PickupLabel->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 24));
    PickupPanel->SetContent(PickupLabel);
}
