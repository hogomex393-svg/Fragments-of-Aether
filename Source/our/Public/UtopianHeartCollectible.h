#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UtopianHeartCollectible.generated.h"

class APawn;
class AUtopianHeartManager;
class USphereComponent;
class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class OUR_API AUtopianHeartCollectible : public AActor
{
    GENERATED_BODY()

public:
    AUtopianHeartCollectible();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utopian City Heart")
    TObjectPtr<USphereComponent> PickupSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utopian City Heart")
    TObjectPtr<UStaticMeshComponent> HeartMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Heart")
    TObjectPtr<AUtopianHeartManager> ManagerOverride;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Heart")
    FName HeartID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Heart")
    bool bRotate = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Heart", meta = (ClampMin = "0.0"))
    float RotationSpeedDegrees = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Heart")
    bool bBob = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Heart", meta = (ClampMin = "0.0"))
    float BobAmplitude = 12.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utopian City Heart", meta = (ClampMin = "0.0"))
    float BobSpeed = 2.2f;

    UFUNCTION(BlueprintPure, Category = "Utopian City Heart")
    bool IsCollected() const;

    UFUNCTION(BlueprintCallable, Category = "Utopian City Heart")
    void MarkCollected();

    UFUNCTION(BlueprintCallable, Category = "Utopian City Heart")
    void RestoreCollectedState();

    UFUNCTION(BlueprintCallable, Category = "Utopian City Heart")
    FName GetHeartID();

    UFUNCTION(BlueprintImplementableEvent, Category = "Utopian City Heart")
    void OnHeartCollected();

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Utopian City Heart")
    bool bCollected = false;

    FVector InitialMeshRelativeLocation = FVector::ZeroVector;
    float RunningTime = 0.0f;

    UFUNCTION()
    void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    AUtopianHeartManager* FindHeartManager() const;
};
