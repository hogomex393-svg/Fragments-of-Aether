#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TempleDialogueTypes.h"
#include "TempleDialogueWidgetBase.generated.h"

class UBorder;
class UCanvasPanel;
class UTextBlock;

UCLASS()
class OUR_API UTempleDialogueWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;

    UFUNCTION(BlueprintCallable, Category = "Temple Dialogue")
    void SetPromptVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "Temple Dialogue")
    void ShowDialogueLine(const FTempleDialogueLine& LineData, int32 CurrentIndex, int32 TotalLines);

    UFUNCTION(BlueprintCallable, Category = "Temple Dialogue")
    void HideDialogue();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Dialogue")
    FText PromptText = NSLOCTEXT("TempleDialogue", "Prompt", "Press E / X to Talk");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Dialogue")
    FText ContinueText = NSLOCTEXT("TempleDialogue", "Continue", "Press E / X to Continue");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Dialogue")
    bool bShowContinueHint = true;

protected:
    UPROPERTY(Transient)
    TObjectPtr<UCanvasPanel> RootCanvas;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> PromptPanel;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> PromptLabel;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> DialoguePanel;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> SpeakerLabel;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> LineLabel;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> ContinueLabel;

    void BuildDefaultWidgetTree();
};
