#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "TempleGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTempleIslandCompletedSignature, int32, IslandID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTempleAllIslandsCompletedSignature);

UCLASS(BlueprintType, Blueprintable)
class OUR_API UTempleGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UTempleGameInstance();

    virtual void Init() override;
    virtual void Shutdown() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Island Progress")
    bool bIsland01Completed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Island Progress")
    bool bIsland02Completed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Island Progress")
    bool bIsland03Completed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Island Progress")
    int32 CompletedIslandCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Island Progress")
    bool bAllIslandsCompleted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Island Progress")
    int32 LastCompletedIslandID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Dialogue Progress")
    bool bUtopianCityOpeningDialoguePlayed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Return")
    bool bReturnToTempleInitialPositionRequested = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Input Recovery")
    bool bRecoverInputOnPlayableMapReturn = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Input Recovery")
    TArray<FName> MenuLevelNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Input Recovery")
    TArray<FName> LegacyDialogueReplayBlockedLevelNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Progress")
    TSet<FName> CollectedUtopianHeartIDs;

    UPROPERTY(BlueprintAssignable, Category = "Temple Island Progress")
    FTempleIslandCompletedSignature OnIslandCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Temple Island Progress")
    FTempleAllIslandsCompletedSignature OnAllIslandsCompleted;

    UFUNCTION(BlueprintCallable, Category = "Temple Island Progress")
    void MarkIslandCompleted(int32 IslandID);

    UFUNCTION(BlueprintPure, Category = "Temple Island Progress")
    bool IsIslandCompleted(int32 IslandID) const;

    UFUNCTION(BlueprintPure, Category = "Temple Island Progress")
    int32 GetCompletedIslandCount() const;

    UFUNCTION(BlueprintCallable, Category = "Temple Island Progress")
    void CheckAllIslandsCompleted();

    UFUNCTION(BlueprintPure, Category = "Temple Island Progress")
    bool AreAllIslandsCompleted() const;

    UFUNCTION(BlueprintCallable, Category = "Temple Island Progress")
    void ResetIslandProgress();

    UFUNCTION(BlueprintCallable, Category = "Temple Return")
    void RequestReturnToTempleInitialPosition();

    UFUNCTION(BlueprintCallable, Category = "Temple Return")
    bool ConsumeReturnToTempleInitialPositionRequest();

    UFUNCTION(BlueprintPure, Category = "Temple Return")
    bool IsReturnToTempleInitialPositionRequested() const;

    UFUNCTION(BlueprintCallable, Category = "Temple Input Recovery")
    void RestoreGameplayInputState();

    UFUNCTION(BlueprintCallable, Category = "Temple Input Recovery")
    void SuppressLegacyDialogueForCurrentWorld();

    UFUNCTION(BlueprintCallable, Category = "Temple Dialogue Progress")
    void MarkUtopianCityOpeningDialoguePlayed();

    UFUNCTION(BlueprintPure, Category = "Temple Dialogue Progress")
    bool HasUtopianCityOpeningDialoguePlayed() const;

    UFUNCTION(BlueprintCallable, Category = "Utopian City Progress")
    void MarkUtopianHeartCollected(FName HeartID);

    UFUNCTION(BlueprintPure, Category = "Utopian City Progress")
    bool IsUtopianHeartCollected(FName HeartID) const;

    UFUNCTION(BlueprintPure, Category = "Utopian City Progress")
    int32 GetCollectedUtopianHeartCount() const;

private:
    FDelegateHandle OnWorldBeginPlayHandle;
    TSet<FName> VisitedPlayableLevelNames;
    bool bReturnInputRecoveryPending = false;

    bool GetBlueprintIslandCompleted(int32 IslandID) const;
    void SetBlueprintIslandCompleted(int32 IslandID, bool bCompleted);
    void RecalculateCompletedIslandCount();
    void HandleWorldInitializedActors(const UWorld::FActorsInitializedParams& Params);
    void RecoverAfterPlayableLevelLoad(bool bSuppressLegacyDialogue);
    bool IsMenuLevel(FName LevelName) const;
    bool ShouldBlockLegacyDialogueReplay(FName LevelName) const;
    FName GetNormalizedLevelName(const UWorld* World) const;
};
