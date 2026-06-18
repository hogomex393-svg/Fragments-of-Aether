#include "UtopianHeartCollectible.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "UtopianHeartManager.h"

AUtopianHeartCollectible::AUtopianHeartCollectible()
{
    PrimaryActorTick.bCanEverTick = true;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    SetRootComponent(PickupSphere);
    PickupSphere->SetSphereRadius(96.0f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupSphere->SetCollisionObjectType(ECC_WorldDynamic);
    PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    PickupSphere->SetGenerateOverlapEvents(true);

    HeartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeartMesh"));
    HeartMesh->SetupAttachment(PickupSphere);
    HeartMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HeartMesh->SetRelativeScale3D(FVector(1.25f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> HeartMeshAsset(TEXT("/Game/Platformer_8_Underworld/Meshes/SM_heart_001.SM_heart_001"));
    if (HeartMeshAsset.Succeeded())
    {
        HeartMesh->SetStaticMesh(HeartMeshAsset.Object);
    }
    else
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh> FallbackHeartMeshAsset(TEXT("/Game/AAMovementSystem/Demo/Meshes/SM_Heart.SM_Heart"));
        if (FallbackHeartMeshAsset.Succeeded())
        {
            HeartMesh->SetStaticMesh(FallbackHeartMeshAsset.Object);
        }
    }
}

void AUtopianHeartCollectible::BeginPlay()
{
    Super::BeginPlay();

    GetHeartID();
    InitialMeshRelativeLocation = HeartMesh ? HeartMesh->GetRelativeLocation() : FVector::ZeroVector;
    PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AUtopianHeartCollectible::HandleBeginOverlap);

    if (AUtopianHeartManager* Manager = ManagerOverride ? ManagerOverride.Get() : FindHeartManager())
    {
        Manager->RegisterHeart(this);
    }
}

void AUtopianHeartCollectible::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bCollected || !HeartMesh)
    {
        return;
    }

    RunningTime += DeltaSeconds;

    if (bRotate)
    {
        AddActorWorldRotation(FRotator(0.0f, RotationSpeedDegrees * DeltaSeconds, 0.0f));
    }

    if (bBob)
    {
        const float OffsetZ = FMath::Sin(RunningTime * BobSpeed) * BobAmplitude;
        HeartMesh->SetRelativeLocation(InitialMeshRelativeLocation + FVector(0.0f, 0.0f, OffsetZ));
    }
}

bool AUtopianHeartCollectible::IsCollected() const
{
    return bCollected;
}

void AUtopianHeartCollectible::MarkCollected()
{
    if (bCollected)
    {
        return;
    }

    bCollected = true;
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorEnableCollision(false);

    if (HeartMesh)
    {
        HeartMesh->SetVisibility(false, true);
    }

    OnHeartCollected();
}

void AUtopianHeartCollectible::RestoreCollectedState()
{
    if (bCollected)
    {
        return;
    }

    bCollected = true;
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorEnableCollision(false);

    if (HeartMesh)
    {
        HeartMesh->SetVisibility(false, true);
    }
}

FName AUtopianHeartCollectible::GetHeartID()
{
    if (HeartID.IsNone())
    {
        HeartID = GetFName();
    }

    return HeartID;
}

void AUtopianHeartCollectible::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bCollected || !OtherActor || !OtherActor->IsA<APawn>())
    {
        return;
    }

    AUtopianHeartManager* Manager = ManagerOverride ? ManagerOverride.Get() : FindHeartManager();
    if (Manager)
    {
        Manager->CollectHeart(this);
    }
    else
    {
        MarkCollected();
    }
}

AUtopianHeartManager* AUtopianHeartCollectible::FindHeartManager() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    for (TActorIterator<AUtopianHeartManager> It(World); It; ++It)
    {
        return *It;
    }

    return nullptr;
}
