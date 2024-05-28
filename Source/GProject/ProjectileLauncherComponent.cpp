#include "ProjectileLauncherComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
UProjectileLauncherComponent::UProjectileLauncherComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;
//	SetIsReplicated(true); // Ensure the component is replicated
}

void UProjectileLauncherComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UProjectileLauncherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UProjectileLauncherComponent::LaunchProjectile()
{
	if (GetOwner()->HasAuthority())
	{
		SpawnProjectile();
	}
	else
	{
		ServerLaunchProjectile();
	}
}

void UProjectileLauncherComponent::ServerLaunchProjectile_Implementation()
{
	SpawnProjectile();
}

bool UProjectileLauncherComponent::ServerLaunchProjectile_Validate()
{
	return true;
}

void UProjectileLauncherComponent::SpawnProjectile()
{
    if (!ProjectileClass) return;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController && PlayerController->GetPawn())
    {
        FVector2D MousePosition;
        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            FVector WorldLocation, WorldDirection;
            if (PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
            {
                // Calculer la position "plate" à la hauteur de l'acteur qui lance
                float DistanceToFlatTarget = FMath::Abs(WorldLocation.Z - PlayerController->GetPawn()->GetActorLocation().Z) / FMath::Abs(WorldDirection.Z);
                FVector FlatAimTarget = WorldLocation + WorldDirection * DistanceToFlatTarget;

                // Maintenant, FlatAimTarget est à la hauteur du joueur mais aligné avec la position de la souris
                // On ajuste la hauteur pour correspondre exactement à celle du joueur pour éviter tout décalage vertical
                FVector SpawnLocation = PlayerController->GetPawn()->GetActorLocation();
                FVector TargetDirection = FlatAimTarget - SpawnLocation;
                TargetDirection.Z = 0; // S'assurer que le projectile se déplace horizontalement

                FRotator SpawnRotation = TargetDirection.Rotation();

                // Lancer le projectile
                AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation);
                if (Projectile)
                {
                    // Initialisation supplémentaire du projectile si nécessaire
                }
            }
        }
    }
}





