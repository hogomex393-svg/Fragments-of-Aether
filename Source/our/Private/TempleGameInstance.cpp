#include "TempleGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"

UTempleGameInstance::UTempleGameInstance()
{
    MenuLevelNames = {
        TEXT("MyStartMap"),
        TEXT("StartMap"),
        TEXT("MyMap")
    };

    LegacyDialogueReplayBlockedLevelNames = {
        TEXT("L_Temple_Hub"),
        TEXT("DUN_DungeonExample_MAP")
    };
}

void UTempleGameInstance::Init()
{
    Super::Init();

    if (!OnWorldBeginPlayHandle.IsValid())
    {
        OnWorldBeginPlayHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UTempleGameInstance::HandleWorldInitializedActors);
    }
}

void UTempleGameInstance::Shutdown()
{
    if (OnWorldBeginPlayHandle.IsValid())
    {
        FWorldDelegates::OnWorldInitializedActors.Remove(OnWorldBeginPlayHandle);
        OnWorldBeginPlayHandle.Reset();
    }

    Super::Shutdown();
}

void UTempleGameInstance::MarkIslandCompleted(int32 IslandID)
{
    bool bNewlyCompleted = false;

    if (IslandID == 1)
    {
        bNewlyCompleted = !IsIslandCompleted(1);
        bIsland01Completed = true;
        SetBlueprintIslandCompleted(1, true);
        LastCompletedIslandID = 1;
    }
    else if (IslandID == 2)
    {
        bNewlyCompleted = !IsIslandCompleted(2);
        bIsland02Completed = true;
        SetBlueprintIslandCompleted(2, true);
        LastCompletedIslandID = 2;
    }
    else if (IslandID == 3)
    {
        bNewlyCompleted = !IsIslandCompleted(3);
        bIsland03Completed = true;
        SetBlueprintIslandCompleted(3, true);
        LastCompletedIslandID = 3;
    }

    RecalculateCompletedIslandCount();
    const bool bWasAllCompleted = bAllIslandsCompleted;
    CheckAllIslandsCompleted();

    if (bNewlyCompleted)
    {
        OnIslandCompleted.Broadcast(IslandID);
    }

    if (!bWasAllCompleted && bAllIslandsCompleted)
    {
        OnAllIslandsCompleted.Broadcast();
    }
}

bool UTempleGameInstance::IsIslandCompleted(int32 IslandID) const
{
    if (IslandID == 1)
    {
        return bIsland01Completed || GetBlueprintIslandCompleted(1);
    }
    if (IslandID == 2)
    {
        return bIsland02Completed || GetBlueprintIslandCompleted(2);
    }
    if (IslandID == 3)
    {
        return bIsland03Completed || GetBlueprintIslandCompleted(3);
    }
    return false;
}

int32 UTempleGameInstance::GetCompletedIslandCount() const
{
    int32 Count = 0;
    Count += IsIslandCompleted(1) ? 1 : 0;
    Count += IsIslandCompleted(2) ? 1 : 0;
    Count += IsIslandCompleted(3) ? 1 : 0;
    return Count;
}

void UTempleGameInstance::CheckAllIslandsCompleted()
{
    bAllIslandsCompleted = IsIslandCompleted(1) && IsIslandCompleted(2) && IsIslandCompleted(3);
}

bool UTempleGameInstance::AreAllIslandsCompleted() const
{
    return bAllIslandsCompleted;
}

void UTempleGameInstance::ResetIslandProgress()
{
    bIsland01Completed = false;
    bIsland02Completed = false;
    bIsland03Completed = false;
    SetBlueprintIslandCompleted(1, false);
    SetBlueprintIslandCompleted(2, false);
    SetBlueprintIslandCompleted(3, false);
    CompletedIslandCount = 0;
    bAllIslandsCompleted = false;
    LastCompletedIslandID = 0;
    bUtopianCityOpeningDialoguePlayed = false;
    bReturnToTempleInitialPositionRequested = false;
    bReturnInputRecoveryPending = false;
    CollectedUtopianHeartIDs.Reset();
    VisitedPlayableLevelNames.Reset();
}

void UTempleGameInstance::RecalculateCompletedIslandCount()
{
    CompletedIslandCount = 0;
    CompletedIslandCount += IsIslandCompleted(1) ? 1 : 0;
    CompletedIslandCount += IsIslandCompleted(2) ? 1 : 0;
    CompletedIslandCount += IsIslandCompleted(3) ? 1 : 0;
}

bool UTempleGameInstance::GetBlueprintIslandCompleted(int32 IslandID) const
{
    FName PropertyName = NAME_None;
    if (IslandID == 1)
    {
        PropertyName = TEXT("Level1");
    }
    else if (IslandID == 2)
    {
        PropertyName = TEXT("Level2");
    }
    else if (IslandID == 3)
    {
        PropertyName = TEXT("Level3");
    }

    if (PropertyName.IsNone())
    {
        return false;
    }

    const FBoolProperty* Property = FindFProperty<FBoolProperty>(GetClass(), PropertyName);
    return Property ? Property->GetPropertyValue_InContainer(this) : false;
}

void UTempleGameInstance::SetBlueprintIslandCompleted(int32 IslandID, bool bCompleted)
{
    FName PropertyName = NAME_None;
    if (IslandID == 1)
    {
        PropertyName = TEXT("Level1");
    }
    else if (IslandID == 2)
    {
        PropertyName = TEXT("Level2");
    }
    else if (IslandID == 3)
    {
        PropertyName = TEXT("Level3");
    }

    if (PropertyName.IsNone())
    {
        return;
    }

    if (FBoolProperty* Property = FindFProperty<FBoolProperty>(GetClass(), PropertyName))
    {
        Property->SetPropertyValue_InContainer(this, bCompleted);
    }
}

void UTempleGameInstance::MarkUtopianCityOpeningDialoguePlayed()
{
    bUtopianCityOpeningDialoguePlayed = true;
}

void UTempleGameInstance::RequestReturnToTempleInitialPosition()
{
    bReturnToTempleInitialPositionRequested = true;
    bReturnInputRecoveryPending = true;
}

bool UTempleGameInstance::ConsumeReturnToTempleInitialPositionRequest()
{
    const bool bWasRequested = bReturnToTempleInitialPositionRequested;
    bReturnToTempleInitialPositionRequested = false;
    return bWasRequested;
}

bool UTempleGameInstance::IsReturnToTempleInitialPositionRequested() const
{
    return bReturnToTempleInitialPositionRequested;
}

void UTempleGameInstance::RestoreGameplayInputState()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UGameplayStatics::SetGamePaused(World, false);

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
    if (!PlayerController)
    {
        return;
    }

    PlayerController->SetPause(false);
    PlayerController->bShowMouseCursor = false;
    PlayerController->SetIgnoreMoveInput(false);
    PlayerController->SetIgnoreLookInput(false);

    FInputModeGameOnly InputMode;
    PlayerController->SetInputMode(InputMode);

    APawn* Pawn = PlayerController->GetPawn();
    if (!Pawn)
    {
        Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    }

    if (Pawn)
    {
        Pawn->EnableInput(PlayerController);
        Pawn->SetActorTickEnabled(true);

        if (ACharacter* Character = Cast<ACharacter>(Pawn))
        {
            if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
            {
                Movement->SetComponentTickEnabled(true);
                Movement->Activate(true);
            }
        }
    }
}

void UTempleGameInstance::SuppressLegacyDialogueForCurrentWorld()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TArray<UUserWidget*> DialogWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, DialogWidgets, UUserWidget::StaticClass(), false);
    for (UUserWidget* Widget : DialogWidgets)
    {
        if (!Widget)
        {
            continue;
        }

        const FString WidgetClassName = Widget->GetClass()->GetName();
        if (WidgetClassName.Contains(TEXT("Dialog")) || WidgetClassName.Contains(TEXT("Dialogue")))
        {
            Widget->RemoveFromParent();
        }
    }

    for (TObjectIterator<UActorComponent> It; It; ++It)
    {
        UActorComponent* Component = *It;
        if (!Component || Component->GetWorld() != World)
        {
            continue;
        }

        const FString ComponentClassName = Component->GetClass()->GetName();
        if (ComponentClassName.Contains(TEXT("BPC_Dialog")) || ComponentClassName.Contains(TEXT("BPC_Dialog_C")))
        {
            Component->Deactivate();
            Component->SetComponentTickEnabled(false);
        }
    }
}

bool UTempleGameInstance::HasUtopianCityOpeningDialoguePlayed() const
{
    return bUtopianCityOpeningDialoguePlayed;
}

void UTempleGameInstance::MarkUtopianHeartCollected(FName HeartID)
{
    if (!HeartID.IsNone())
    {
        CollectedUtopianHeartIDs.Add(HeartID);
    }
}

bool UTempleGameInstance::IsUtopianHeartCollected(FName HeartID) const
{
    return !HeartID.IsNone() && CollectedUtopianHeartIDs.Contains(HeartID);
}

int32 UTempleGameInstance::GetCollectedUtopianHeartCount() const
{
    return CollectedUtopianHeartIDs.Num();
}

void UTempleGameInstance::HandleWorldInitializedActors(const UWorld::FActorsInitializedParams& Params)
{
    UWorld* World = Params.World;
    if (!World || World != GetWorld() || !bRecoverInputOnPlayableMapReturn)
    {
        return;
    }

    const FName LevelName = GetNormalizedLevelName(World);
    if (LevelName.IsNone() || IsMenuLevel(LevelName))
    {
        return;
    }

    const bool bWasVisited = VisitedPlayableLevelNames.Contains(LevelName);
    VisitedPlayableLevelNames.Add(LevelName);

    const bool bIsTempleReturn = bReturnInputRecoveryPending || IsReturnToTempleInitialPositionRequested();
    bReturnInputRecoveryPending = false;

    const bool bSuppressLegacyDialogue = bIsTempleReturn || (bWasVisited && ShouldBlockLegacyDialogueReplay(LevelName));
    const bool bShouldRecoverInput = bSuppressLegacyDialogue || bWasVisited;

    if (bShouldRecoverInput)
    {
        RecoverAfterPlayableLevelLoad(bSuppressLegacyDialogue);
    }
}

void UTempleGameInstance::RecoverAfterPlayableLevelLoad(bool bSuppressLegacyDialogue)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FTimerHandle RecoveryTimerHandle;
    World->GetTimerManager().SetTimer(
        RecoveryTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this, bSuppressLegacyDialogue]()
        {
            RestoreGameplayInputState();
            if (bSuppressLegacyDialogue)
            {
                SuppressLegacyDialogueForCurrentWorld();
                RestoreGameplayInputState();
            }
        }),
        0.1f,
        false);
}

bool UTempleGameInstance::IsMenuLevel(FName LevelName) const
{
    return MenuLevelNames.Contains(LevelName);
}

bool UTempleGameInstance::ShouldBlockLegacyDialogueReplay(FName LevelName) const
{
    return LegacyDialogueReplayBlockedLevelNames.Contains(LevelName);
}

FName UTempleGameInstance::GetNormalizedLevelName(const UWorld* World) const
{
    if (!World)
    {
        return NAME_None;
    }

    FString MapName = World->GetMapName();
    MapName.RemoveFromStart(World->StreamingLevelsPrefix);

    const int32 DotIndex = MapName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
    if (DotIndex != INDEX_NONE)
    {
        MapName.RightChopInline(DotIndex + 1);
    }

    return FName(*MapName);
}
