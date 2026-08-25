#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

UCLASS()
class MYPROJECTCH2_API ASpartaGameState : public AGameState
{
    GENERATED_BODY()

public:
    ASpartaGameState();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 SpawnedCoinCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 CollectedCoinCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    float LevelDuration;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 CurrentLevelIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 MaxLevels;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    TArray<FName> LevelMapNames;

    FTimerHandle LevelTimerHandle;
    FTimerHandle HUDUpdateTimerHandle; // HUD 갱신용 타이머 핸들

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Level")
    void OnGameOver();

    void StartLevel();
    void OnLevelTimeUp();
    void OnCoinCollected();
    void EndLevel();

    // HUD의 Text Block들을 찾아 업데이트하는 핵심 함수
    void UpdateHUD();
};