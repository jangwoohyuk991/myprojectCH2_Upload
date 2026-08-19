#include "HealingItem.h"
#include "Engine/Engine.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20; 
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		
		GEngine->AddOnScreenDebugMessage(
			-1, 
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Player gained %d HP!"), HealAmount));

		DestroyItem();
	}
}