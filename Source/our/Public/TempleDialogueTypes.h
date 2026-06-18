#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TempleDialogueTypes.generated.h"

USTRUCT(BlueprintType)
struct FTempleDialogueLine : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Dialogue")
    FText Speaker;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Temple Dialogue", meta = (MultiLine = true))
    FText Line;
};
