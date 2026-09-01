#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpartaCharacter.h" 
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "MineItem.h" // Wave 2 지뢰/함정 스폰 및 Wave 1 스폰 제외 처리를 위해 추가
#include "SpikeTrap.h" // 가시 함정 스폰을 위해 추가
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h" // 체력 바(ProgressBar) 사용을 위해 추가
#include "Blueprint/UserWidget.h"
#include "Particles/ParticleSystem.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
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

// === [도전 2 구현] 알림 메시지 제어 및 웨이브 이벤트 구현 ===
void ASpartaGameState::ShowNoticeMessage(const FString& Message, float DisplayTime)
{
	CurrentNoticeMessage = Message;

	GetWorldTimerManager().SetTimer(
		NoticeTimerHandle,
		this,
		&ASpartaGameState::ClearNoticeMessage,
		DisplayTime,
		false
	);

	UpdateHUD();
}

void ASpartaGameState::ClearNoticeMessage()
{
	CurrentNoticeMessage = TEXT("");
	UpdateHUD();
}

void ASpartaGameState::TriggerWaveEvent()
{
	// Wave 2: 레벨 주기에 맞춰 넓은 맵에 무작위 가시 함정 스폰
	if (CurrentWave == 2)
	{
		SpawnSpikeTrap();
	}
	// Wave 3: 레벨 주기에 맞춰 무작위 위치 폭발 이벤트 발생
	else if (CurrentWave >= 3)
	{
		TriggerRandomExplosion();
	}
}

// Wave 2: 플레이어 기준 넓은 범위(-1200 ~ 1200) 무작위 좌표에 3개씩 가시 함정을 스폰
void ASpartaGameState::SpawnSpikeTrap()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn)) return;

	FVector PlayerLoc = PlayerPawn->GetActorLocation();

	if (SpikeTrapClass)
	{
		// 한 번 호출될 때마다 맵 사방 무작위 위치 3곳에 가시 스폰
		const int32 SpawnCount = 3;
		for (int32 i = 0; i < SpawnCount; ++i)
		{
			FVector SpawnLoc = PlayerLoc + FVector(
				FMath::RandRange(-1200.0f, 1200.0f), // 넓은 맵 범위
				FMath::RandRange(-1200.0f, 1200.0f),
				0.0f
			);

			GetWorld()->SpawnActor<ASpikeTrap>(SpikeTrapClass, SpawnLoc, FRotator::ZeroRotator);
		}
	}

	ShowNoticeMessage(TEXT("[위험] 바닥에서 새로운 스파이크 함정들이 솟구칩니다!"), 2.0f);
	UE_LOG(LogTemp, Warning, TEXT("Wave 2: Random Spike Traps spawned around map!"));
}

// Wave 3: 플레이어 주변 맵 무작위 위치에 폭발 파티클 + 사운드 + 범위 판정 데미지 적용
void ASpartaGameState::TriggerRandomExplosion()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn)) return;

	// 1. 플레이어 위치 기준 반지름 600.0f 범위 내 무작위 폭발 위치 계산
	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	FVector ExplosionLoc = PlayerLoc + FVector(
		FMath::RandRange(-600.0f, 600.0f),
		FMath::RandRange(-600.0f, 600.0f),
		0.0f
	);

	// 2. 계산된 무작위 위치에 폭발 파티클 및 사운드 스폰
	if (Wave3ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			Wave3ExplosionParticle,
			ExplosionLoc,
			FRotator::ZeroRotator,
			true
		);
	}

	if (Wave3ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			Wave3ExplosionSound,
			ExplosionLoc
		);
	}

	// 3. 폭발 위치(ExplosionLoc)와 플레이어(PlayerLoc) 간의 거리 계산
	float Distance = FVector::Dist(PlayerLoc, ExplosionLoc);
	float DamageRadius = 250.0f; // 폭발 피해 반경

	// 4. 폭발 범위(250.0f) 안에 들어와 있을 때만 데미지 부여!
	if (Distance <= DamageRadius)
	{
		UGameplayStatics::ApplyDamage(
			PlayerPawn,
			15.0f,
			nullptr,
			nullptr,
			UDamageType::StaticClass()
		);
		ShowNoticeMessage(TEXT("[경고] 무작위 폭발에 휘말려 피해를 입었습니다!"), 2.0f);
	}
	else
	{
		ShowNoticeMessage(TEXT("[주의] 주변 맵에서 무작위 폭발이 발생했습니다!"), 1.5f);
	}
}

// === [구현] 레벨 및 웨이브 연동 동적 난이도 시작 로직 ===
void ASpartaGameState::StartWave()
{
	// 기존 반복 이벤트 타이머 정리
	GetWorldTimerManager().ClearTimer(WaveEventTimerHandle);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Level %d - Wave %d Start!"), CurrentLevelIndex + 1, CurrentWave));
	}

	// 1. 레벨에 따른 제한 시간 및 장애물 이벤트 주기 차등 설정
	// Level 1: 30초 (여유) / 주기 4.0초
	// Level 2: 25초 (보통) / 주기 2.5초
	// Level 3: 20초 (촉박) / 주기 1.2초
	float DynamicLevelDuration = FMath::Max(10.0f, 30.0f - (CurrentLevelIndex * 5.0f));
	float EventInterval = FMath::Max(1.0f, 4.0f - (CurrentLevelIndex * 1.3f));

	// 2. 웨이브별 알림 및 타이머 가동
	if (CurrentWave == 1)
	{
		ShowNoticeMessage(FString::Printf(TEXT("Level %d - Wave 1: 기본 플레이"), CurrentLevelIndex + 1), 3.0f);
	}
	else if (CurrentWave == 2)
	{
		ShowNoticeMessage(FString::Printf(TEXT("Level %d - Wave 2: 스파이크 함정주의!"), CurrentLevelIndex + 1), 3.0f);

		SpawnSpikeTrap(); // 진입 즉시 스폰

		GetWorldTimerManager().SetTimer(
			WaveEventTimerHandle,
			this,
			&ASpartaGameState::TriggerWaveEvent,
			EventInterval, // 레벨이 높을수록 빠른 주기
			true
		);
	}
	else if (CurrentWave >= 3)
	{
		ShowNoticeMessage(FString::Printf(TEXT("Level %d - Wave 3: 무작위 폭발 주의!"), CurrentLevelIndex + 1), 3.0f);

		GetWorldTimerManager().SetTimer(
			WaveEventTimerHandle,
			this,
			&ASpartaGameState::TriggerWaveEvent,
			EventInterval, // 레벨이 높을수록 빠른 주기
			true
		);
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	// 3. 레벨과 웨이브를 조합한 차등 스폰 개수 공식
	// Level 1 (Index 0): 기본 10개 + (Wave * 5) -> Wave1: 15개, Wave2: 20개, Wave3: 25개
	// Level 2 (Index 1): 기본 20개 + (Wave * 5) -> Wave1: 25개, Wave2: 30개, Wave3: 35개
	// Level 3 (Index 2): 기본 35개 + (Wave * 5) -> Wave1: 40개, Wave2: 45개, Wave3: 50개
	const int32 BaseSpawnAmount = 10 + (CurrentLevelIndex * 12);
	const int32 ItemToSpawn = BaseSpawnAmount + (CurrentWave * 5);

	if (FoundVolumes.Num() > 0 && IsValid(FoundVolumes[0]))
	{
		if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]))
		{
			for (int32 i = 0; i < ItemToSpawn; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

				if (IsValid(SpawnedActor))
				{
					// Wave 1일 때는 데이터 테이블 스폰 중 지뢰(AMineItem)가 뽑히면 제거
					if (CurrentWave == 1 && SpawnedActor->IsA(AMineItem::StaticClass()))
					{
						SpawnedActor->Destroy();
						continue;
					}

					// 코인 개수 카운트
					if (SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						SpawnedCoinCount++;
					}
				}
			}
		}
	}

	// 4. 레벨별 가변 제한 시간 타이머 재설정
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		DynamicLevelDuration,
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

// === HUD 정보 갱신 (NoticeText 및 애니메이션 연동 적용) ===
void ASpartaGameState::UpdateHUD()
{
	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!IsValid(PlayerController)) return;

	ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
	if (!IsValid(SpartaPlayerController)) return;

	UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget();
	if (!IsValid(HUDWidget)) return;

	// === [추가] NoticeText 알림 메시지 갱신 및 블루프린트 애니메이션 함수 호출 ===
	if (UTextBlock* NoticeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("NoticeText"))))
	{
		// 메시지가 새롭게 설정되었을 때 텍스트를 변경하고 블루프린트 애니메이션 함수 실행
		if (!CurrentNoticeMessage.IsEmpty() && NoticeText->GetText().ToString() != CurrentNoticeMessage)
		{
			NoticeText->SetText(FText::FromString(CurrentNoticeMessage));

			// 위젯 블루프린트 그래프에 만든 PlayNoticeAnimation 함수 호출!
			UFunction* PlayAnimFunc = HUDWidget->FindFunction(FName("PlayNoticeAnimation"));
			if (PlayAnimFunc)
			{
				HUDWidget->ProcessEvent(PlayAnimFunc, nullptr);
			}
		}
		else if (CurrentNoticeMessage.IsEmpty())
		{
			NoticeText->SetText(FText::FromString(TEXT("")));
		}
	}

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

			// === [추가] 디버프 UI 텍스트 갱신 ===
			// Slow 디버프 텍스트 갱신
			if (UTextBlock* SlowText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("SlowText"))))
			{
				if (GetWorldTimerManager().IsTimerActive(Character->SlowTimerHandle))
				{
					float Remain = GetWorldTimerManager().GetTimerRemaining(Character->SlowTimerHandle);
					SlowText->SetText(FText::FromString(FString::Printf(TEXT("[디버프] 이동 속도 감소 (%.1f초)"), Remain)));
				}
				else
				{
					SlowText->SetText(FText::FromString(TEXT(""))); // 효과 없으면 빈 문자열
				}
			}

			// Reverse 디버프 텍스트 갱신
			if (UTextBlock* ReverseText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ReverseText"))))
			{
				if (Character->bIsReverseControl)
				{
					float Remain = GetWorldTimerManager().GetTimerRemaining(Character->ReverseTimerHandle);
					ReverseText->SetText(FText::FromString(FString::Printf(TEXT("[디버프] 조작 반전 (%.1f초)"), Remain)));
				}
				else
				{
					ReverseText->SetText(FText::FromString(TEXT("")));
				}
			}
		}
	}
}