#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TempleDialogueTypes.h"
#include "UtopianHeartManager.generated.h"

class AUtopianHeartCollectible;
class APlayerController;
class UTempleDialogueWidgetBase;
class UUtopianHeartHUDWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUtopianHeartProgressSignature, int32, CollectedCount, int32, TargetCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUtopianAllHeartsCollectedSignature);

UCLASS(BlueprintType, Blueprintable)
class OUR_API AUtopianHeartManager : public AActor
{
    GENERATED_BODY()

public:
    AUtopianHeartManager();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts", meta = (ClampMin = "1"))
    int32 TargetHeartCount = 15;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts")
    int32 TempleIslandID = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts")
    bool bCreateHUD = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts")
    TSubclassOf<UUtopianHeartHUDWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Hearts")
    bool bShowDebugMessages = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Dialogue")
    TSubclassOf<UTempleDialogueWidgetBase> DialogueWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Dialogue")
    bool bPlayOpeningDialogueOnBeginPlay = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Dialogue", meta = (ClampMin = "0.0"))
    float OpeningDialogueDelaySeconds = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Dialogue")
    FKey DialogueAdvanceKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Dialogue")
    FKey DialogueAdvanceGamepadKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Dialogue", meta = (TitleProperty = "Line"))
    TArray<FTempleDialogueLine> OpeningDialogueLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Dialogue", meta = (TitleProperty = "Line"))
    TArray<FTempleDialogueLine> CompletionDialogueLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Completion")
    bool bReturnToTempleHubAfterCompletionDialogue = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Completion", meta = (EditCondition = "bReturnToTempleHubAfterCompletionDialogue"))
    FName TempleHubLevelName = TEXT("L_Temple_Hub");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Completion", meta = (EditCondition = "bReturnToTempleHubAfterCompletionDialogue", ClampMin = "0.0"))
    float ReturnToTempleDelaySeconds = 0.2f;

    UPROPERTY(BlueprintAssignable, Category = "Utopian City Hearts")
    FUtopianHeartProgressSignature OnHeartProgressChanged;

    UPROPERTY(BlueprintAssignable, Category = "Utopian City Hearts")
    FUtopianAllHeartsCollectedSignature OnAllHeartsCollected;

    UFUNCTION(BlueprintCallable, Category = "Utopian City Hearts")
    void RegisterHeart(AUtopianHeartCollectible* Heart);

    UFUNCTION(BlueprintCallable, Category = "Utopian City Hearts")
    void CollectHeart(AUtopianHeartCollectible* Heart);

    UFUNCTION(BlueprintPure, Category = "Utopian City Hearts")
    int32 GetCollectedHeartCount() const;

    UFUNCTION(BlueprintPure, Category = "Utopian City Hearts")
    int32 GetTargetHeartCount() const;

    UFUNCTION(BlueprintPure, Category = "Utopian City Hearts")
    bool AreAllHeartsCollected() const;

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Utopian City Hearts")
    int32 CollectedHeartCount = 0;

    UPROPERTY(Transient)
    TObjectPtr<UUtopianHeartHUDWidget> HUDWidget;

    UPROPERTY(Transient)
    TObjectPtr<UTempleDialogueWidgetBase> DialogueWidget;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AUtopianHeartCollectible>> RegisteredHearts;

    TSet<TWeakObjectPtr<AUtopianHeartCollectible>> CollectedHearts;
    bool bCompletionTriggered = false;
    bool bOpeningDialogueStarted = false;
    bool bDialogueActive = false;
    bool bActiveDialogueIsCompletion = false;
    bool bWasDialogueAdvanceKeyDown = false;
    float OpeningDialogueElapsedSeconds = 0.0f;
    int32 CurrentDialogueLineIndex = INDEX_NONE;
    const TArray<FTempleDialogueLine>* ActiveDialogueLines = nullptr;

    void RegisterExistingHearts();
    void CreateHUDWidget();
    void EnsureDialogueWidget();
    void EnsureDefaultDialogueLines();
    void TryStartOpeningDialogue(float DeltaSeconds);
    void StartDialogue(const TArray<FTempleDialogueLine>& DialogueLines);
    void StartDialogue(const TArray<FTempleDialogueLine>& DialogueLines, bool bIsCompletionDialogue);
    void AdvanceDialogue();
    void FinishDialogue();
    void BroadcastProgress();
    void HandleCompletion();
    void ReturnToTempleHub();
    bool IsDialogueAdvancePressed(APlayerController* PlayerController) const;
};
