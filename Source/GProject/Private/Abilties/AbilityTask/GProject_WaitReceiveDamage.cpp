// Copyright 2020 Dan Kestranek.


#include "GProject/Public/Abilties/AbilityTask/GProject_WaitReceiveDamage.h"
#include "GProject/Public/CharacterAbilitySystemComponent.h"


UGProject_WaitReceiveDamage::UGProject_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UGProject_WaitReceiveDamage* UGProject_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool InTriggerOnce)
{
	UGProject_WaitReceiveDamage* MyObj = NewAbilityTask<UGProject_WaitReceiveDamage>(OwningAbility);
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UGProject_WaitReceiveDamage::Activate()
{
	UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->ReceivedDamage.AddDynamic(this, &UGProject_WaitReceiveDamage::OnDamageReceived);
	}
}

void UGProject_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->ReceivedDamage.RemoveDynamic(this, &UGProject_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UGProject_WaitReceiveDamage::OnDamageReceived(UCharacterAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}