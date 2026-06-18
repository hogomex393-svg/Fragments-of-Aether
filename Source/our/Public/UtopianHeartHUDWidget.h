#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UtopianHeartHUDWidget.generated.h"

class UBorder;
class UCanvasPanel;
class UTextBlock;

UCLASS()
class OUR_API UUtopianHeartHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Utopian City Hearts")
    void SetHeartProgress(int32 CollectedCount, int32 TargetCount);

    UFUNCTION(BlueprintCallable, Category = "Utopian City Hearts")
    void ShowCompletionMessage();

    UFUNCTION(BlueprintCallable, Category = "Utopian City Hearts")
    void ShowPickupMessage(int32 CollectedCount, int32 TargetCount);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts")
    FText ProgressFormat = NSLOCTEXT("UtopianCity", "HeartProgressFormat", "Celestial Etherlove: {0} / {1}");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts")
    FText CompletionText = NSLOCTEXT("UtopianCity", "HeartCompletionText", "All Celestial Etherlove Fragments reclaimed. Exit unlocked.");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts")
    FText PickupMessageFormat = NSLOCTEXT("UtopianCity", "HeartPickupMessage", "Celestial Etherlove Fragment reclaimed. {0} / {1}");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts", meta = (ClampMin = "0.1"))
    float PickupMessageDurationSeconds = 2.2f;

protected:
    UPROPERTY(Transient)
    TObjectPtr<UCanvasPanel> RootCanvas;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> HudPanel;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> ProgressLabel;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> PickupPanel;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> PickupLabel;

    float PickupMessageRemainingSeconds = 0.0f;

    void BuildDefaultWidgetTree();
};
