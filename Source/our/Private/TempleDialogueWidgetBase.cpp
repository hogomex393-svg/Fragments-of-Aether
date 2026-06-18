#include "TempleDialogueWidgetBase.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateColor.h"

void UTempleDialogueWidgetBase::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    BuildDefaultWidgetTree();
}

void UTempleDialogueWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();
    BuildDefaultWidgetTree();
    SetPromptVisible(false);
    HideDialogue();
}

void UTempleDialogueWidgetBase::BuildDefaultWidgetTree()
{
    if (!WidgetTree || RootCanvas)
    {
        return;
    }

    RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TempleDialogueRoot"));
    WidgetTree->RootWidget = RootCanvas;

    PromptPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PromptPanel"));
    PromptPanel->SetBrushColor(FLinearColor(0.02f, 0.02f, 0.02f, 0.72f));
    PromptPanel->SetPadding(FMargin(18.0f, 10.0f));
    RootCanvas->AddChild(PromptPanel);
    if (UCanvasPanelSlot* PromptSlot = Cast<UCanvasPanelSlot>(PromptPanel->Slot))
    {
        PromptSlot->SetAnchors(FAnchors(0.5f, 0.78f));
        PromptSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        PromptSlot->SetAutoSize(true);
    }

    PromptLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PromptLabel"));
    PromptLabel->SetText(PromptText);
    PromptLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    PromptLabel->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 26));
    PromptPanel->SetContent(PromptLabel);

    DialoguePanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DialoguePanel"));
    DialoguePanel->SetBrushColor(FLinearColor(0.01f, 0.01f, 0.012f, 0.88f));
    DialoguePanel->SetPadding(FMargin(30.0f, 22.0f));
    RootCanvas->AddChild(DialoguePanel);
    if (UCanvasPanelSlot* DialogueSlot = Cast<UCanvasPanelSlot>(DialoguePanel->Slot))
    {
        DialogueSlot->SetAnchors(FAnchors(0.5f, 0.82f));
        DialogueSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        DialogueSlot->SetSize(FVector2D(980.0f, 220.0f));
    }

    UCanvasPanel* DialogueCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("DialogueCanvas"));
    DialoguePanel->SetContent(DialogueCanvas);

    SpeakerLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SpeakerLabel"));
    SpeakerLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.78f, 0.42f, 1.0f)));
    SpeakerLabel->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 24));
    DialogueCanvas->AddChild(SpeakerLabel);
    if (UCanvasPanelSlot* SpeakerSlot = Cast<UCanvasPanelSlot>(SpeakerLabel->Slot))
    {
        SpeakerSlot->SetAnchors(FAnchors(0.0f, 0.0f));
        SpeakerSlot->SetOffsets(FMargin(0.0f, 0.0f, 900.0f, 36.0f));
    }

    LineLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LineLabel"));
    LineLabel->SetAutoWrapText(true);
    LineLabel->SetWrapTextAt(900.0f);
    LineLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    LineLabel->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 28));
    DialogueCanvas->AddChild(LineLabel);
    if (UCanvasPanelSlot* LineSlot = Cast<UCanvasPanelSlot>(LineLabel->Slot))
    {
        LineSlot->SetAnchors(FAnchors(0.0f, 0.0f));
        LineSlot->SetOffsets(FMargin(0.0f, 48.0f, 900.0f, 105.0f));
    }

    ContinueLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContinueLabel"));
    ContinueLabel->SetText(ContinueText);
    ContinueLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.82f, 0.9f, 1.0f)));
    ContinueLabel->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 18));
    DialogueCanvas->AddChild(ContinueLabel);
    if (UCanvasPanelSlot* ContinueSlot = Cast<UCanvasPanelSlot>(ContinueLabel->Slot))
    {
        ContinueSlot->SetAnchors(FAnchors(1.0f, 1.0f));
        ContinueSlot->SetAlignment(FVector2D(1.0f, 1.0f));
        ContinueSlot->SetAutoSize(true);
    }
}

void UTempleDialogueWidgetBase::SetPromptVisible(bool bVisible)
{
    BuildDefaultWidgetTree();
    if (PromptPanel)
    {
        PromptPanel->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
    }
}

void UTempleDialogueWidgetBase::ShowDialogueLine(const FTempleDialogueLine& LineData, int32 CurrentIndex, int32 TotalLines)
{
    BuildDefaultWidgetTree();
    SetPromptVisible(false);

    if (DialoguePanel)
    {
        DialoguePanel->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    if (SpeakerLabel)
    {
        SpeakerLabel->SetText(LineData.Speaker);
    }
    if (LineLabel)
    {
        LineLabel->SetText(LineData.Line);
    }
    if (ContinueLabel)
    {
        const FText ProgressText = bShowContinueHint
            ? FText::Format(
                NSLOCTEXT("TempleDialogue", "ContinueWithProgress", "Press E / X to Continue  {0}/{1}"),
                FText::AsNumber(CurrentIndex + 1),
                FText::AsNumber(TotalLines))
            : FText::Format(
                NSLOCTEXT("TempleDialogue", "ProgressOnly", "{0}/{1}"),
                FText::AsNumber(CurrentIndex + 1),
                FText::AsNumber(TotalLines));
        ContinueLabel->SetText(ProgressText);
    }
}

void UTempleDialogueWidgetBase::HideDialogue()
{
    BuildDefaultWidgetTree();
    if (DialoguePanel)
    {
        DialoguePanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}
