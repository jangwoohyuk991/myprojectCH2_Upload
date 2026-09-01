#include "SlowingItem.h"
#include "SpartaCharacter.h"

ASlowingItem::ASlowingItem()
{
	ItemType = "Slowing";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	// 1. 기본 아이템 효과(파티클 및 사운드) 재생
	Super::ActivateItem(Activator);

	// 2. 이 아이템을 밟은 액터가 플레이어인지 검사
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(Activator))
		{
			// 3. 플레이어 캐릭터의 감속 함수 호출
			Character->ApplySlow(SlowRatio, Duration);
		}
	}

	// 4. 아이템 액터 삭제
	DestroyItem();
}