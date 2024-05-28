// Fill out your copyright notice in the Description page of Project Settings.

#include "GProjectCharacter.h"
#include "../GProject.h"

#include "CharacterAbilitySystemComponent.h"
#include <GProject/GProjectPlayerController.h>
#include <GProject/GProjectGameMode.h>
#include <GProjectPlayerState.h>
#include <EnhancedInputComponent.h>
#include "EnhancedInputSubsystems.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "../ProjectileLauncherComponent.h"
#include <Actions/PawnAction.h>
#include "CharacterGameplayAbility.h"
#include "CharacterAttributeSetBase.h"
#include <Kismet/GameplayStatics.h>


AGProjectCharacter::AGProjectCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	bReplicates = true;
	//// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//Initialize Charcter Component 

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ProjectileLauncherComponent = CreateDefaultSubobject<UProjectileLauncherComponent>(TEXT("ProjectileLauncherComponent"));
	ProjectileLauncherComponent->SetupAttachment(RootComponent);
	//GAS


	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	bAlwaysRelevant = true;
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));



}



void AGProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	world = GetWorld();
	PC = Cast<AGProjectPlayerController>(GetController());


	//Add Input Mapping Context
	if (PC)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			PlayerMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/TopDown/Input/IMC_ZQSD"));
			if (PlayerMappingContext) { UE_LOG(LogTemp, Warning, TEXT("Success")); }
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
			bool jh = this->HasAuthority();
			UInputComponent* input = this->InputComponent;
			int z = 5;

		}
	}
}

void AGProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Look();
}


void AGProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		OnInputComponentReady.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInput"));

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGProjectCharacter::Move);

		//Looking

		EnhancedInputComponent->BindAction(DrawAction, ETriggerEvent::Completed, this, &AGProjectCharacter::Draw);

	}
	BindASCInput();

}

void AGProjectCharacter::Draw(const FInputActionValue& Value)
{
	//log
	UE_LOG(LogTemp, Warning, TEXT("Draw Called"));
	ProjectileLauncherComponent->LaunchProjectile();



}



void AGProjectCharacter::Move(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Move Called"));
	if (!IsAlive()) return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (PC != nullptr)
	{
		//// find out which way is forward
		//const FRotator Rotation = PlayerControllerRef->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		//// get forward vector
		//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//// get right vector 
		//const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		FVector ForwardDirection = FVector(1, 0, 0);
		FVector RightDirection = FVector(0, 1, 0);
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void AGProjectCharacter::Look()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController || !PlayerController->GetPawn())
	{
		return;
	}

	if (PC == nullptr)
	{
		return;
	}

	FVector2D MousePosition;
	if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		FVector SpawnLocation = PlayerController->GetPawn()->GetActorLocation();
		FVector SpawnLocation2 = SpawnLocation + FVector(0.0f, 0.0f, ProjectileHeight);
		FVector WorldLocation, WorldDirection;
		if (PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
		{
			// Dessiner un point où la souris touche le monde

			float DistanceToFlatTarget = FMath::Abs(WorldLocation.Z - SpawnLocation2.Z) / FMath::Abs(WorldDirection.Z);
			FVector FlatAimTarget = WorldLocation + WorldDirection * DistanceToFlatTarget;

			DrawDebugSphere(
				GetWorld(),
				FlatAimTarget,
				20.0f, // Rayon de la sphère
				12, // Segments (plus il y en a, plus la sphère est lisse)
				FColor::Red, // Couleur de la sphère
				false, // Persistent (ne pas mettre à true sauf si vous voulez que ça reste plus longtemps que le frame)
				-1.0f, // Durée de vie (en secondes)
				(uint8)'\000', // Profondeur
				1.0f // Épaisseur
			);
			// Dessiner une ligne du pion du joueur jusqu'à la cible "plate"
			DrawDebugLine(
				GetWorld(),
				SpawnLocation2,
				FlatAimTarget,
				FColor::Blue,
				false, // PersistentLines
				-1.0f, // LifeTime
				(uint8)'\000', // DepthPriority
				1.0f // Thickness
			);

			FVector TargetDirection = FlatAimTarget - SpawnLocation2;
			TargetDirection.Z = 0; // S'assurer que le projectile se déplace horizontalement
			float newYaw = (FlatAimTarget - SpawnLocation2).Rotation().Yaw;
			FRotator newRot = this->GetActorRotation();
			newRot.Yaw = newYaw;
			PC->SetControlRotation(newRot);
		}
	}
}
//void AGProjectCharacter::Look()
//{
//	if (PC != nullptr)
//	{// We look for the location in the world where the player has pressed the input
//		FHitResult Hit;
//		FVector CurrLoc = this->GetActorLocation();
//		bool bHitSuccessful = false;
//
//		bHitSuccessful = PC->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, Hit);
//		// If we hit a surface, cache the location
//		if (bHitSuccessful)
//		{
//			CachedDestination = Hit.Location;
//		}
//
//		FRotator newRot = this->GetActorRotation();
//		float newYaw = (CachedDestination - CurrLoc).Rotation().Yaw;
//		newRot.Yaw = newYaw;
//		PC->SetControlRotation(newRot);
//
//	}
//}

void AGProjectCharacter::FinishDying()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AGProjectGameMode* GM = Cast<AGProjectGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			//GM->HeroDied(GetController());
		}
	}
	Super::FinishDying();
}

//
//void AGProjectCharacter::InitializeStartingValues(AGProjectPlayerState* PS)
//{
//
//	AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
//
//	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
//
//	AttributeSetBase = PS->GetAttributeSetBase();
//
//	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
//	this->InitializeAttributes();
//
//	this->SetHealth(GetMaxHealth());
//	this->SetMana(GetMaxMana());
//}
void AGProjectCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AGProjectPlayerState* PS = GetPlayerState<AGProjectPlayerState>();
	if (PS)
	{

		AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		AttributeSetBase = PS->GetAttributeSetBase();

		InitializeAttributes();

		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);




		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());

		AddStartupEffects();
		AddCharacterAbilities();

		//AGProjectPlayerController* PC = Cast<AGProjectPlayerController>(GetController());
		//if (PC)
		//{
		//	//PC->CreateHUD();
		//}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO PLAYER STATE FOUND"));

	}
}

FGameplayAbilitySpecHandle AGProjectCharacter::GetAbilityHandle(UGameplayAbility* TargetAbility)
{
	if (AbilitySystemComponent.IsValid() && !TargetAbility)
	{
		const TArray<FGameplayAbilitySpec>& Abilities = AbilitySystemComponent->GetActivatableAbilities();
		for (const FGameplayAbilitySpec& Spec : Abilities)
		{
			if (Spec.Ability != TargetAbility)
			{
				// Nous avons trouvé le Spec correspondant à notre capacité cible
				return Spec.Handle;
			}
		}
	}

	return FGameplayAbilitySpecHandle(); // Retourne un handle vide si non trouvé
}

void AGProjectCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AGProjectPlayerState* PS = GetPlayerState<AGProjectPlayerState>();
	if (PS)
	{

		AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		BindASCInput();

		AttributeSetBase = PS->GetAttributeSetBase();

		InitializeAttributes();

		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		//	FGameplayAbilitySpecHandle Ability1Handle = AbilitySystemComponent->GiveAbility(Ability1);
		//AbilitySystemComponent->SetInputBinding(Ability1Action, GetAbilityHandle(nullptr));


		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());



		//InitializeStartingValues(PS);
		//AGProjectPlayerController* PC = Cast<AGProjectPlayerController>(GetController());
		//if (PC)
		//{
		//	//PC->CreateHUD();
		//}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO PLAYER STATE FOUND"));

	}
}
void AGProjectCharacter::InitializeBindingFromTxt()
{
	//TODO 
}

void AGProjectCharacter::SetUpMapHandler()
{
	InputMapHandler.Add(FGameplayTag::RequestGameplayTag("Ability.Skill.Ability1"), Ability1Action);
	InputMapHandler.Add(FGameplayTag::RequestGameplayTag("Ability.Skill.Ability2"), Ability2Action);
}
void AGProjectCharacter::BindASCInput()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		return;
	}
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		/*	FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(FName("/Script/GProject"), FName("GProjectAbilityID"));
			AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
				FString("CancelTarget"), AbilityEnumAssetPath, static_cast<int32>(GProjectAbilityID::Confirm), static_cast<int32>(GProjectAbilityID::Cancel)));*/
		SetUpMapHandler();
		AbilitySystemComponent->BindInputComponent(InputComponent);
		FGameplayTagContainer inputTags;

		TArray<FGameplayTag> TagsArray;
		InputMapHandler.GetKeys(TagsArray);
		inputTags = FGameplayTagContainer::CreateFromArray(TagsArray);
		for (const FGameplayAbilitySpec spec : AbilitySystemComponent->GetActivatableAbilities())
		{

			FGameplayTag tagSpellToBind = AbilitySystemComponent->AbilityContainsTags(spec.Ability->AbilityTags, inputTags);
			AbilitySystemComponent->SetInputBinding(InputMapHandler[tagSpellToBind], spec.Handle);
		}
		ASCInputBound = true;
	}
}
