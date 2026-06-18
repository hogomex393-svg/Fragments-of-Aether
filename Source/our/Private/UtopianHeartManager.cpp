#include "UtopianHeartManager.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "TempleDialogueWidgetBase.h"
#include "TempleGameInstance.h"
#include "TimerManager.h"
#include "UtopianHeartCollectible.h"
#include "UtopianHeartHUDWidget.h"

AUtopianHeartManager::AUtopianHeartManager()
{
    PrimaryActorTick.bCanEverTick = true;
    HUDWidgetClass = UUtopianHeartHUDWidget::StaticClass();
    DialogueAdvanceKey = EKeys::E;
    DialogueAdvanceGamepadKey = EKeys::Gamepad_FaceButton_Right;
}

void AUtopianHeartManager::BeginPlay()
{
    Super::BeginPlay();

    PrimaryActorTick.SetTickFunctionEnable(true);
    EnsureDefaultDialogueLines();
    CollectedHeartCount = 0;
    CollectedHearts.Reset();
    if (const UTempleGameInstance* TempleGameInstance = Cast<UTempleGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        bOpeningDialogueStarted = TempleGameInstance->HasUtopianCityOpeningDialoguePlayed();
    }
    RegisterExistingHearts();
    CreateHUDWidget();
    EnsureDialogueWidget();
    BroadcastProgress();
}

void AUtopianHeartManager::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    TryStartOpeningDialogue(DeltaSeconds);

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    const bool bIsDown = IsDialogueAdvancePressed(PlayerController);
    const bool bPressedThisFrame = bIsDown && !bWasDialogueAdvanceKeyDown;
    bWasDialogueAdvanceKeyDown = bIsDown;

    if (bDialogueActive && bPressedThisFrame)
    {
        AdvanceDialogue();
    }

    if (!bDialogueActive && bOpeningDialogueStarted && bCompletionTriggered)
    {
        SetActorTickEnabled(false);
    }
}

void AUtopianHeartManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void AUtopianHeartManager::RegisterHeart(AUtopianHeartCollectible* Heart)
{
    if (!Heart)
    {
        return;
    }

    RegisteredHearts.AddUnique(Heart);

    const FName HeartID = Heart->GetHeartID();
    if (const UTempleGameInstance* TempleGameInstance = Cast<UTempleGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        if (TempleGameInstance->IsUtopianHeartCollected(HeartID))
        {
            Heart->RestoreCollectedState();
            CollectedHearts.Add(Heart);
            CollectedHeartCount = CollectedHearts.Num();
        }
    }
}

void AUtopianHeartManager::CollectHeart(AUtopianHeartCollectible* Heart)
{
    if (!Heart || Heart->IsCollected() || CollectedHearts.Contains(Heart))
    {
        return;
    }

    CollectedHearts.Add(Heart);
    CollectedHeartCount = CollectedHearts.Num();
    if (UTempleGameInstance* TempleGameInstance = Cast<UTempleGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        TempleGameInstance->MarkUtopianHeartCollected(Heart->GetHeartID());
        CollectedHeartCount = FMath::Max(CollectedHeartCount, TempleGameInstance->GetCollectedUtopianHeartCount());
    }
    Heart->MarkCollected();
    BroadcastProgress();

    if (CollectedHeartCount >= TargetHeartCount)
    {
        HandleCompletion();
    }
}

int32 AUtopianHeartManager::GetCollectedHeartCount() const
{
    return CollectedHeartCount;
}

int32 AUtopianHeartManager::GetTargetHeartCount() const
{
    return TargetHeartCount;
}

bool AUtopianHeartManager::AreAllHeartsCollected() const
{
    return CollectedHeartCount >= TargetHeartCount;
}

void AUtopianHeartManager::RegisterExistingHearts()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<AUtopianHeartCollectible> It(World); It; ++It)
    {
        RegisterHeart(*It);
    }
}

void AUtopianHeartManager::CreateHUDWidget()
{
    if (!bCreateHUD || !HUDWidgetClass)
    {
        return;
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController)
    {
        return;
    }

    HUDWidget = CreateWidget<UUtopianHeartHUDWidget>(PlayerController, HUDWidgetClass);
    if (HUDWidget)
    {
        HUDWidget->AddToViewport(5);
    }
}

void AUtopianHeartManager::EnsureDialogueWidget()
{
    if (DialogueWidget)
    {
        return;
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!PlayerController)
    {
        return;
    }

    TSubclassOf<UTempleDialogueWidgetBase> ClassToCreate = DialogueWidgetClass;
    if (!ClassToCreate)
    {
        ClassToCreate = UTempleDialogueWidgetBase::StaticClass();
    }

    DialogueWidget = CreateWidget<UTempleDialogueWidgetBase>(PlayerController, ClassToCreate);
    if (DialogueWidget)
    {
        DialogueWidget->AddToViewport(42);
        DialogueWidget->SetPromptVisible(false);
        DialogueWidget->HideDialogue();
    }
}

void AUtopianHeartManager::EnsureDefaultDialogueLines()
{
    const auto AddLine = [](TArray<FTempleDialogueLine>& TargetLines, const TCHAR* Text)
    {
        FTempleDialogueLine Line;
        Line.Speaker = NSLOCTEXT("UtopianCityDialogue", "NarratorSpeaker", "Narrator");
        Line.Line = FText::FromString(Text);
        TargetLines.Add(Line);
    };

    if (OpeningDialogueLines.Num() == 0)
    {
        AddLine(OpeningDialogueLines, TEXT("Beyond this gate lies UtopianCity - a fortified citadel forged to safeguard the temple's arcane relics. Prepare yourself, traveler."));
        AddLine(OpeningDialogueLines, TEXT("Its luminous streets conceal a treacherous truth: Mechanisms once designed to protect now hunt the unwary. Caution is your armor."));
        AddLine(OpeningDialogueLines, TEXT("Scattered across the city's tiers are Aether Fragments bound to the city seal. Explore. Evaluate. Engage."));
        AddLine(OpeningDialogueLines, TEXT("Avoid traps, decipher environmental cues, and master your ascent to reclaim every Fragment."));
        AddLine(OpeningDialogueLines, TEXT("When the final Fragment is reclaimed, the city rune will pulse with ancient power. Only then will the exit corridor manifest."));
        AddLine(OpeningDialogueLines, TEXT("This city reveals its secrets to those who move with precision and analyze with agility. Fail, and its traps will seal your fate."));
    }

    if (CompletionDialogueLines.Num() == 0)
    {
        AddLine(CompletionDialogueLines, TEXT("The final fragment has returned to the city seal. UtopianCity remembers the purpose for which it was forged."));
        AddLine(CompletionDialogueLines, TEXT("The Aether Fragments no longer lie scattered. All resonance now coalesces into a single awakened pulse."));
        AddLine(CompletionDialogueLines, TEXT("The mechanisms fall silent. The traps withdraw their judgment. The city recognizes you as one who endured its trial."));
        AddLine(CompletionDialogueLines, TEXT("A rune of the Aether Temple has been restored through your precision, perception, and resolve."));
        AddLine(CompletionDialogueLines, TEXT("The exit corridor now manifests beyond the sealed path. Follow its light and return to the temple hub."));
        AddLine(CompletionDialogueLines, TEXT("Carry this victory with care, traveler. The temple will not open until all three echoes answer."));
    }
}

void AUtopianHeartManager::TryStartOpeningDialogue(float DeltaSeconds)
{
    if (!bPlayOpeningDialogueOnBeginPlay || bOpeningDialogueStarted || bDialogueActive)
    {
        return;
    }

    OpeningDialogueElapsedSeconds += DeltaSeconds;
    if (OpeningDialogueElapsedSeconds >= OpeningDialogueDelaySeconds)
    {
        bOpeningDialogueStarted = true;
        if (UTempleGameInstance* TempleGameInstance = Cast<UTempleGameInstance>(UGameplayStatics::GetGameInstance(this)))
        {
            TempleGameInstance->MarkUtopianCityOpeningDialoguePlayed();
        }
        StartDialogue(OpeningDialogueLines, false);
    }
}

void AUtopianHeartManager::StartDialogue(const TArray<FTempleDialogueLine>& DialogueLines)
{
    StartDialogue(DialogueLines, false);
}

void AUtopianHeartManager::StartDialogue(const TArray<FTempleDialogueLine>& DialogueLines, bool bIsCompletionDialogue)
{
    EnsureDialogueWidget();
    if (!DialogueWidget || DialogueLines.Num() == 0)
    {
        if (bIsCompletionDialogue)
        {
            ReturnToTempleHub();
        }
        return;
    }

    ActiveDialogueLines = &DialogueLines;
    bActiveDialogueIsCompletion = bIsCompletionDialogue;
    CurrentDialogueLineIndex = 0;
    bDialogueActive = true;
    DialogueWidget->ShowDialogueLine((*ActiveDialogueLines)[CurrentDialogueLineIndex], CurrentDialogueLineIndex, ActiveDialogueLines->Num());
    SetActorTickEnabled(true);
}

void AUtopianHeartManager::AdvanceDialogue()
{
    if (!bDialogueActive || !DialogueWidget || !ActiveDialogueLines)
    {
        return;
    }

    ++CurrentDialogueLineIndex;
    if (ActiveDialogueLines->IsValidIndex(CurrentDialogueLineIndex))
    {
        DialogueWidget->ShowDialogueLine((*ActiveDialogueLines)[CurrentDialogueLineIndex], CurrentDialogueLineIndex, ActiveDialogueLines->Num());
    }
    else
    {
        FinishDialogue();
    }
}

void AUtopianHeartManager::FinishDialogue()
{
    const bool bFinishedCompletionDialogue = bActiveDialogueIsCompletion;

    bDialogueActive = false;
    bActiveDialogueIsCompletion = false;
    CurrentDialogueLineIndex = INDEX_NONE;
    ActiveDialogueLines = nullptr;

    if (DialogueWidget)
    {
        DialogueWidget->HideDialogue();
    }

    if (bFinishedCompletionDialogue)
    {
        ReturnToTempleHub();
    }
}

void AUtopianHeartManager::BroadcastProgress()
{
    if (HUDWidget)
    {
        HUDWidget->SetHeartProgress(CollectedHeartCount, TargetHeartCount);
        if (CollectedHeartCount > 0 && CollectedHeartCount < TargetHeartCount)
        {
            HUDWidget->ShowPickupMessage(CollectedHeartCount, TargetHeartCount);
        }
    }

    OnHeartProgressChanged.Broadcast(CollectedHeartCount, TargetHeartCount);

    if (bShowDebugMessages && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Cyan,
            FString::Printf(TEXT("Celestial Etherlove: %d / %d"), CollectedHeartCount, TargetHeartCount));
    }
}

void AUtopianHeartManager::HandleCompletion()
{
    if (bCompletionTriggered)
    {
        return;
    }

    bCompletionTriggered = true;

    if (HUDWidget)
    {
        HUDWidget->ShowCompletionMessage();
    }

    if (UTempleGameInstance* TempleGameInstance = Cast<UTempleGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        TempleGameInstance->MarkIslandCompleted(TempleIslandID);
    }

    OnAllHeartsCollected.Broadcast();

    StartDialogue(CompletionDialogueLines, true);

    if (bShowDebugMessages && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            5.0f,
            FColor::Green,
            TEXT("UtopianCity Complete - Island 2 marked complete"));
    }
}

void AUtopianHeartManager::ReturnToTempleHub()
{
    if (!bReturnToTempleHubAfterCompletionDialogue || TempleHubLevelName.IsNone())
    {
        return;
    }

    if (UTempleGameInstance* TempleGameInstance = Cast<UTempleGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        TempleGameInstance->RequestReturnToTempleInitialPosition();
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (ReturnToTempleDelaySeconds <= 0.0f)
    {
        UGameplayStatics::OpenLevel(this, TempleHubLevelName);
        return;
    }

    FTimerHandle ReturnTimerHandle;
    World->GetTimerManager().SetTimer(
        ReturnTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this]()
        {
            UGameplayStatics::OpenLevel(this, TempleHubLevelName);
        }),
        ReturnToTempleDelaySeconds,
        false);
}

bool AUtopianHeartManager::IsDialogueAdvancePressed(APlayerController* PlayerController) const
{
    return PlayerController
        && (PlayerController->IsInputKeyDown(DialogueAdvanceKey)
            || PlayerController->IsInputKeyDown(DialogueAdvanceGamepadKey));
}
