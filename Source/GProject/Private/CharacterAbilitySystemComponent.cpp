// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAbilitySystemComponent.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CharacterGameplayAbility.h"
#include <GProjectPlayerState.h>
#include <GProject/GProjectPlayerController.h>
#include <GProjectCharacter.h>




void UCharacterAbilitySystemComponent::ReceiveDamage(UCharacterAbilitySystemComponent* sourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(sourceASC, UnmitigatedDamage, MitigatedDamage);
}



namespace CharacterAbilitySystemComponent_Impl
{
	constexpr int32 InvalidInputID = 0;
	int32 IncrementingInputID = InvalidInputID;

	static int32 GetNextInputID()
	{
		return ++IncrementingInputID;
	}
}

void UCharacterAbilitySystemComponent::SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle)
{
	using namespace CharacterAbilitySystemComponent_Impl;

	FGameplayAbilitySpec* BindingAbility = FindAbilitySpec(AbilityHandle);

	FAbilityInputBinding* AbilityInputBinding = MappedAbilities.Find(InputAction);
	if (AbilityInputBinding)
	{
		FGameplayAbilitySpec* OldBoundAbility = FindAbilitySpec(AbilityInputBinding->BoundAbilitiesStack.Top());
		if (OldBoundAbility && OldBoundAbility->InputID == AbilityInputBinding->InputID)
		{
			OldBoundAbility->InputID = InvalidInputID;
		}
	}
	else
	{
		AbilityInputBinding = &MappedAbilities.Add(InputAction);
		AbilityInputBinding->InputID = GetNextInputID();
	}

	if (BindingAbility)
	{
		BindingAbility->InputID = AbilityInputBinding->InputID;
	}

	AbilityInputBinding->BoundAbilitiesStack.Push(AbilityHandle);


	TryBindAbilityInput(InputAction, *AbilityInputBinding);
}

void UCharacterAbilitySystemComponent::ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle)
{
	using namespace CharacterAbilitySystemComponent_Impl;

	if (FGameplayAbilitySpec* FoundAbility = FindAbilitySpec(AbilityHandle))
	{
		// Find the mapping for this ability
		auto MappedIterator = MappedAbilities.CreateIterator();
		while (MappedIterator)
		{
			if (MappedIterator.Value().InputID == FoundAbility->InputID)
			{
				break;
			}

			++MappedIterator;
		}

		if (MappedIterator)
		{
			FAbilityInputBinding& AbilityInputBinding = MappedIterator.Value();

			if (AbilityInputBinding.BoundAbilitiesStack.Remove(AbilityHandle) > 0)
			{
				if (AbilityInputBinding.BoundAbilitiesStack.Num() > 0)
				{
					FGameplayAbilitySpec* StackedAbility = FindAbilitySpec(AbilityInputBinding.BoundAbilitiesStack.Top());
					if (StackedAbility && StackedAbility->InputID == 0)
					{
						StackedAbility->InputID = AbilityInputBinding.InputID;
					}
				}
				else
				{
					// NOTE: This will invalidate the `AbilityInputBinding` ref above
					RemoveEntry(MappedIterator.Key());
				}
				// DO NOT act on `AbilityInputBinding` after here (it could have been removed)


				FoundAbility->InputID = InvalidInputID;
			}
		}
	}
}

void UCharacterAbilitySystemComponent::BindInputComponent(UInputComponent* inputComponent)
{
	if (inputComponent)
	{
		InputComponent = Cast<UEnhancedInputComponent>(inputComponent);
	}
}

void UCharacterAbilitySystemComponent::ClearAbilityBindings(UInputAction* InputAction)
{
	RemoveEntry(InputAction);
}

void UCharacterAbilitySystemComponent::OnAbilityInputPressed(UInputAction* InputAction)
{
	using namespace CharacterAbilitySystemComponent_Impl;

	FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
	if (FoundBinding && ensure(FoundBinding->InputID != InvalidInputID))
	{
		AbilityLocalInputPressed(FoundBinding->InputID);
	}
}

void UCharacterAbilitySystemComponent::OnAbilityInputReleased(UInputAction* InputAction)
{
	using namespace CharacterAbilitySystemComponent_Impl;

	FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
	if (FoundBinding && ensure(FoundBinding->InputID != InvalidInputID))
	{
		AbilityLocalInputReleased(FoundBinding->InputID);
	}
}

void UCharacterAbilitySystemComponent::RemoveEntry(UInputAction* InputAction)
{
	if (FAbilityInputBinding* Bindings = MappedAbilities.Find(InputAction))
	{
		if (InputComponent)
		{
			InputComponent->RemoveBindingByHandle(Bindings->OnPressedHandle);
			InputComponent->RemoveBindingByHandle(Bindings->OnReleasedHandle);
		}

		for (FGameplayAbilitySpecHandle AbilityHandle : Bindings->BoundAbilitiesStack)
		{
			using namespace CharacterAbilitySystemComponent_Impl;

			FGameplayAbilitySpec* AbilitySpec = FindAbilitySpec(AbilityHandle);
			if (AbilitySpec && AbilitySpec->InputID == Bindings->InputID)
			{
				AbilitySpec->InputID = InvalidInputID;
			}
		}

		MappedAbilities.Remove(InputAction);
	}
}

FGameplayAbilitySpec* UCharacterAbilitySystemComponent::FindAbilitySpec(FGameplayAbilitySpecHandle Handle)
{
	FGameplayAbilitySpec* FoundAbility = nullptr;
	FoundAbility = FindAbilitySpecFromHandle(Handle);
	return FoundAbility;
}

void UCharacterAbilitySystemComponent::TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding)
{

	if (InputComponent)
	{
		// Pressed event
		if (AbilityInputBinding.OnPressedHandle == 0)
		{
			AbilityInputBinding.OnPressedHandle = InputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &UCharacterAbilitySystemComponent::OnAbilityInputPressed, InputAction).GetHandle();
		}

		// Released event
		if (AbilityInputBinding.OnReleasedHandle == 0)
		{
			AbilityInputBinding.OnReleasedHandle = InputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &UCharacterAbilitySystemComponent::OnAbilityInputReleased, InputAction).GetHandle();
		}
	}
}

void UCharacterAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* debug = this->GetOwner();
	bool test = debug->HasAuthority();
	AActor* Owner = GetPawnFromASC();



}



FGameplayTag UCharacterAbilitySystemComponent::AbilityContainsTags(FGameplayTagContainer AbilityTags, FGameplayTagContainer InputAbilityTags)
{
	for (const FGameplayTag& tag : AbilityTags)
	{
		if (InputAbilityTags.HasTag(tag))
		{
			return tag;
		}
	}

	return FGameplayTag::EmptyTag;
}
APawn* UCharacterAbilitySystemComponent::GetPawnFromASC()
{
	// Step 1: Get the Owner of the ASC, which should be the PlayerState
	AGProjectPlayerState* PlayerState = Cast<AGProjectPlayerState>(GetOwner());
	if (!PlayerState)
	{
		return nullptr;
	}

	// Step 2: Get the PlayerController from the PlayerState's Owner
	AGProjectPlayerController* Controller = Cast<AGProjectPlayerController>(PlayerState->GetOwner());
	if (!Controller)
	{
		return nullptr;
	}

	// Step 3: Get the Pawn from the Controller
	APawn* ControlledPawn = Controller->GetPawn();
	return ControlledPawn;
}

