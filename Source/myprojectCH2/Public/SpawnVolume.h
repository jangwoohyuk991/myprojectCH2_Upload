#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h" // 구조체 포함
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class MYPROJECTCH2_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;

	// 에디터에서 할당할 데이터 테이블 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	// 리턴 형식을 void에서 AActor* 로 변경 (GameState에서 스폰된 코인 카운팅용)
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();

	// 데이터 테이블 기반 확률 추첨 함수
	FItemSpawnRow* GetRandomItem() const;

	// 리턴 형식을 void에서 AActor* 로 변경
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);

	FVector GetRandomPointInVolume() const;
};