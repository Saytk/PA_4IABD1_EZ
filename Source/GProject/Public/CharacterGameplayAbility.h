// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include <GProject/GProject.h>
#include "CharacterGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UCharacterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public :

	UCharacterGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	GProjectAbilityID AbilityInputID = GProjectAbilityID::None;	
	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	GProjectAbilityID AbilityID = GProjectAbilityID::None;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilitySpec& spec) override;
};
