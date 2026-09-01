#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

// 전방 선언
class AMineItem;
class ASpikeTrap; // 가시 함정 클래스 전방 선언 추가
class UParticleSystem;
class USoundBase;

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

	// === [추가 1] 웨이브 관련 변수 선언 ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	// === [도전 2 ] 웨이브별 환경 변화 타이머 및 메시지 변수 ===
	FTimerHandle NoticeTimerHandle;
	FTimerHandle WaveEventTimerHandle;
	FString CurrentNoticeMessage;

	// === [도전 2 ] Wave 2 및 Wave 3 연동용 이펙트/에셋 변수 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Effects")
	TSubclassOf<ASpikeTrap> SpikeTrapClass; // BP_SpikeTrap 할당용으로 변수 타입 변경

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Effects")
	UParticleSystem* Wave3ExplosionParticle; // Wave 3 무작위 폭발 파티클

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Effects")
	USoundBase* Wave3ExplosionSound; // Wave 3 무작위 폭발 사운드

	// === [도전 2 ] 웨이브 환경 변화 함수 ===
	void ShowNoticeMessage(const FString& Message, float DisplayTime = 3.0f);
	void ClearNoticeMessage();
	void TriggerWaveEvent();
	void SpawnSpikeTrap();
	void TriggerRandomExplosion();

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();

	// === [추가 2] 웨이브 흐름 함수 ===
	void StartWave();
	void EndWave();
	void OnLevelTimeUp(); // 웨이브 시간 만료 시 호출
	void OnCoinCollected();

	void EndLevel();
	void UpdateHUD();
};