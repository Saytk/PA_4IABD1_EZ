#include "NiagaraProjectile.h"
#include "NiagaraEmitter.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include <GProjectCharacterBase.h>

// Sets default values
ANiagaraProjectile::ANiagaraProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Projectile Movement Component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 500.f;
	ProjectileMovementComponent->MaxSpeed = 500.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	// Ensure that the actor has a collision component
   // RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));



	// Initialize Niagara Effect Component
	NiagaraEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffectComponent"));
	NiagaraEffectComponent->SetupAttachment(RootComponent);

	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SphereCollisionComponent->SetupAttachment(NiagaraEffectComponent);


	if (AGProjectCharacterBase* character = Cast<AGProjectCharacterBase>(GetInstigator()))
	{
		ProjectileHeight = character->ProjectileHeight;
	}
}

void ANiagaraProjectile::BeginPlay()
{

	Super::BeginPlay();

	if (NiagaraEffectComponent)
	{
		if (UNiagaraSystem* niagaraEffect = NiagaraEffectComponent->GetAsset())
		{
			NiagaraEffectComponent->SetAsset(niagaraEffect);
			NiagaraEffectComponent->SetActive(true);
		}
	}


}





void ANiagaraProjectile::CustomDestroy()

{
	if (NiagaraEffectComponent)
	{
		NiagaraEffectComponent->SetActive(false);
		SphereCollisionComponent->Deactivate();
	}
	UNiagaraComponent* SpawnedSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		MyNiagaraSystem,
		this->GetActorLocation(),
		this->GetActorRotation(),
		FVector(1.0f),
		true, // Whether the system should auto-destroy when its playback is completed
		true, // Whether to auto-activate the system upon spawning
		ENCPoolMethod::None, // Pooling method, None means no pooling
		true // Whether the system should be placed in the world or be a component
	);

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &ANiagaraProjectile::AfterDelay, timerToDestroy, false);


}
void ANiagaraProjectile::AfterDelay()
{
	Destroy();
}


// Called every frame
void ANiagaraProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
