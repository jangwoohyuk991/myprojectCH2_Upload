#include "CoinItem.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		// Á¡¼ö È¹µæ È­¸é Ãâ·Â
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f, 
			FColor::Green,
			FString::Printf(TEXT("Player gained %d points!"), PointValue));

		DestroyItem(); // È¹µæ ÈÄ »èÁ¦
	}
}