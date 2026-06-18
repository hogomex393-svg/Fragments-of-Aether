#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestIslandCompleteTrigger.generated.h"

class UBoxComponent;

UCLASS(BlueprintType, Blueprintable)
class OUR_API ATestIslandCompleteTrigger : public AActor
{
    GENERATED_BODY()

public:
    ATestIslandCompleteTrigger();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Test Island Complete Trigger")
    TObjectPtr<UBoxComponent> TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Island Complete Trigger")
    int32 IslandID = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Island Complete Trigger")
    bool bDestroyAfterTrigger = false;

protected:
    UFUNCTION()
    void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
