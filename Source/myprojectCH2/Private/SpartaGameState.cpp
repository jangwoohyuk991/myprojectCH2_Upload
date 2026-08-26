#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpartaCharacter.h" 
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h" // 체력 바(ProgressBar) 사용을 위해 추가
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 5.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	// === 웨이브 변수 초기화 ===
	CurrentWave = 1;
	MaxWaves = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	// 레벨 시작 시 1웨이브부터 진행
	CurrentWave = 1;
	StartWave();
}

// === [구현] 멀티 웨이브 시작 로직 (안정화 적용) ===
void ASpartaGameState::StartWave()
{
	// 1. 화면에 "Wave N 시작!" 알림 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Wave %d Start!"), CurrentWave));
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	// 웨이브에 맞춰 아이템 스폰 수 차등 부여 (예: 기본 20개 + 웨이브당 10개)
	const int32 ItemToSpawn = 20 + (CurrentWave * 10);

	// Safe Check: FoundVolumes 배열 및 0번 인덱스의 유효성 검사 추가
	if (FoundVolumes.Num() > 0 && IsValid(FoundVolumes[0]))
	{
		if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]))
		{
			for (int32 i = 0; i < ItemToSpawn; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (IsValid(SpawnedActor) && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	// 웨이브 제한 시간 타이머 재설정
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);

	UpdateHUD();
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

// === [구현] 웨이브 종료 및 다음 진행 로직 ===
void ASpartaGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	// 최대 웨이브 미만이면 다음 웨이브 진행
	if (CurrentWave < MaxWaves)
	{
		CurrentWave++;
		StartWave();
	}
	else
	{
		// 3웨이브 완주 시 레벨 종료
		EndLevel();
	}
}

void ASpartaGameState::EndLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);

			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

			if (CurrentLevelIndex >= MaxLevels)
			{
				OnGameOver();
				return;
			}

			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
			}
			else
			{
				OnGameOver();
			}
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

// === HUD 정보 갱신 (안정화 적용) ===
void ASpartaGameState::UpdateHUD()
{
	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!IsValid(PlayerController)) return;

	ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
	if (!IsValid(SpartaPlayerController)) return;

	UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget();
	if (!IsValid(HUDWidget)) return;

	// 남은 시간
	if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
	{
		float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
	}

	// 현재 점수
	if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
			if (SpartaGameInstance)
			{
				ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
			}
		}
	}

	// 현재 레벨
	if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
	{
		LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
	}

	// 현재 웨이브 표시
	if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
	{
		WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave)));
	}

	// 플레이어 체력 표시 (Pawn 및 Character에 대한 IsValid 검사 추가)
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (IsValid(PlayerPawn))
	{
		if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(PlayerPawn))
		{
			float CurrentHP = Character->GetHealth();
			float MaxHP = 100.0f; // SpartaCharacter의 MaxHealth 기준

			if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HPText"))))
			{
				HPText->SetText(FText::FromString(FString::Printf(TEXT("HP : %.0f / %.0f"), CurrentHP, MaxHP)));
			}

			if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HPProgressBar"))))
			{
				HPBar->SetPercent(CurrentHP / MaxHP);
			}
		}
	}
}