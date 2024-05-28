// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilties/GProject_LaunchFireball.h"

#include "GProject/Public/Abilties/AbilityTask/GProject_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GProject/Public/GProjectCharacter.h"
UGProject_LaunchFireball::UGProject_LaunchFireball()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Ability1"));
	AbilityTags.AddTag(Ability1Tag);
	ActivationOwnedTags.AddTag(Ability1Tag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

	Range = 1000.0f;
	Damage = 12.0f;
}

void UGProject_LaunchFireball::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	
	UAnimMontage* MontageToPlay = FireHipMontage;

	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights"))) &&
		!GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights.Removal"))))
	{
		MontageToPlay = FireIronsightsMontage;
	}

	// Play fire montage and wait for event telling us to spawn the projectile
	UGProject_PlayMontageAndWait* Task = UGProject_PlayMontageAndWait::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGProject_LaunchFireball::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGProject_LaunchFireball::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGProject_LaunchFireball::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGProject_LaunchFireball::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGProject_LaunchFireball::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();

}

void UGProject_LaunchFireball::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGProject_LaunchFireball::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGProject_LaunchFireball::SpawnFireball(AGProjectCharacter* Hero)
{
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
	{ 
		FVector StartLocation = Hero->GetActorLocation(); // Ajustez selon l'origine de la boule de feu
		FRotator LaunchRotation = Hero->GetControlRotation();

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ANiagaraProjectile* Projectile = GetWorld()->SpawnActor<ANiagaraProjectile>(ProjectileClass, StartLocation, LaunchRotation, SpawnParameters);
		if (Projectile)
		{
			FVector LaunchDirection = LaunchRotation.Vector();
			Projectile->ProjectileMovementComponent->Velocity = LaunchDirection * Projectile->ProjectileMovementComponent->InitialSpeed;

			// Vous pouvez ici configurer d'autres propriétés du projectile si nécessaire
		}

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}


void UGProject_LaunchFireball::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Vérifie si l'événement reçu correspond au tag d'événement pour lancer le projectile
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
	{
		AGProjectCharacter* Hero = Cast<AGProjectCharacter>(GetAvatarActorFromActorInfo());
		if (!Hero)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		SpawnFireball(Hero);

	}
	else if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		// Autre gestion d'événements, par exemple, terminer l'ability si le montage se termine avant le lancement du projectile
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
