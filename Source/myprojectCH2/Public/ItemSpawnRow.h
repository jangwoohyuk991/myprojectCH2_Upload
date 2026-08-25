#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // FTableRowBase 선언 헤더
#include "ItemSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRow : public FTableRowBase
{
	GENERATED_BODY()
	//TSubclassOf : 하드 래퍼런스 TSubclassPtr : 소프트 레퍼런스
public:
	// 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	// 스폰할 아이템의 블루프린트 클래스 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ItemClass;

	// 스폰 확률 (가중치)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance;
};