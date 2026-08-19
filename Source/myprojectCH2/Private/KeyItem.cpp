#include "KeyItem.h"
#include "Engine/Engine.h" // 

AKeyItem::AKeyItem()
{
	ItemType = "Key";
}

void AKeyItem::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Key collected!"));
		ActivateItem(OtherActor);
	}
}

void AKeyItem::ActivateItem(AActor* Activator)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Key used to unlock the door!"));
	DestroyItem();
}