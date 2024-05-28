// Copyright Epic Games, Inc. All Rights Reserved.

#include "GProjectCharacterBase.h"
#include "CharacterAttributeSetBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


AGProjectCharacterBase::AGProjectCharacterBase(const class FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

}

// Called when the game starts or when spawned
void AGProjectCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGProjectCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AGProjectCharacterBase::AddCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}
	for (TSubclassOf<UCharacterGameplayAbility>& startupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(startupAbility, GetAbilityLevel(startupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(startupAbility.GetDefaultObject()->AbilityID), this));
	}

	
}

void AGProjectCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("NOT VALID ASC"));

		return;
	}
	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle newHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (newHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*newHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AGProjectCharacterBase::AddStartupEffects()
{
	bool x = GetLocalRole() != ROLE_Authority;
	bool y = !AbilitySystemComponent.IsValid();
	bool z = !AbilitySystemComponent->StartupEffectsApplied;
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->StartupEffectsApplied)
	{
		UE_LOG(LogTemp, Warning, TEXT("COUCOU1"));
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		UE_LOG(LogTemp, Warning, TEXT("COUCOU2"));

		FGameplayEffectSpecHandle newHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (newHandle.IsValid())
		{
			FActiveGameplayEffectHandle activeGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*newHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;



}

void AGProjectCharacterBase::SetHealth(float health)
{
	if (AttributeSetBase.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Health is being set"));

		AttributeSetBase->SetHealth(health);
	}
}

void AGProjectCharacterBase::SetMana(float Mana)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetMana(Mana);
	}
}

UAbilitySystemComponent* AGProjectCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool AGProjectCharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

int32 AGProjectCharacterBase::GetAbilityLevel(GProjectAbilityID abilityId) const
{
	return 1;
}

void AGProjectCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((spec.SourceObject == this) && CharacterAbilities.Contains(spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->CharacterAbilitiesGiven = false;
	}

}

void AGProjectCharacterBase::Die()
{
	RemoveCharacterAbilities();

	OnCharacterDied.Broadcast(this);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->CancelAbilities();
		FGameplayTagContainer EffectsTagsToRemove;
		EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
		int32 NumEffectsEremoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AGProjectCharacterBase::FinishDying()
{
	Destroy();
}


float AGProjectCharacterBase::GetCharacterLevel() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetLevel();
	}

	return 0.0f;
}
float AGProjectCharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}
float AGProjectCharacterBase::GetMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;

}
float AGProjectCharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{	
		return AttributeSetBase->GetMaxHealth();
	}
	return 0.0f;

}
float AGProjectCharacterBase::GetMaxMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxMana();
	}

	return 0.0f;
}