// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "CharacterAttributeSetBase.h"
#include "GProjectPlayerState.generated.h"
/**
 * 
 */
UCLASS()
class GPROJECT_API AGProjectPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public :
	AGProjectPlayerState();

	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UCharacterAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState")
	bool isAlive() const;

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState|Attributes")
	float GetHealth() const;											  

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState|Attributes")
	float GetMaxHealth() const;											  

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState|Attributes")
	float GetMana() const;												  

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState|Attributes")
	float GetMaxMana() const;											  

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState|Attributes")
	int32 GetCharacterLevel() const;					

	UFUNCTION(BlueprintCallable, Category = "GProject|GProjectPlayerState|Attributes")
	float GetArmor() const;


protected :
	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()

	class UCharacterAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	virtual void BeginPlay() override;


	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);


};
