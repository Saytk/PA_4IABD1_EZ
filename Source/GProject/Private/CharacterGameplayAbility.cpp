// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterGameplayAbility.h"
#include "CharacterAbilitySystemComponent.h"


UCharacterGameplayAbility::UCharacterGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

void UCharacterGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilitySpec& spec)
{
	Super::OnAvatarSet(actorInfo, spec);

	if (ActivateAbilityOnGranted)
	{
		actorInfo->AbilitySystemComponent->TryActivateAbility(spec.Handle, false);
	}
}
