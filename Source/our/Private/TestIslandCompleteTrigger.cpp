#include "TestIslandCompleteTrigger.h"

#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TempleGameInstance.h"

ATestIslandCompleteTrigger::ATestIslandCompleteTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);
    TriggerBox->SetBoxExtent(FVector(160.0f, 160.0f, 120.0f));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);
}

void ATestIslandCompleteTrigger::BeginPlay()
{
    Super::BeginPlay();
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATestIslandCompleteTrigger::HandleBeginOverlap);
}

void ATestIslandCompleteTrigger::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || !OtherActor->IsA<APawn>())
    {
        return;
    }

    UTempleGameInstance* TempleGameInstance = Cast<UTempleGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!TempleGameInstance)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("TempleGameInstance not found"));
        }
        return;
    }

    TempleGameInstance->MarkIslandCompleted(IslandID);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            4.0f,
            FColor::Green,
            FString::Printf(TEXT("Island %d Completed"), IslandID));

        if (TempleGameInstance->AreAllIslandsCompleted())
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                4.0f,
                FColor::Yellow,
                TEXT("All Islands Completed - Central Gate Can Open"));
        }
    }

    if (bDestroyAfterTrigger)
    {
        Destroy();
    }
}
