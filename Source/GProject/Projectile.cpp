#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Initialize components
	InitializeCollisionComponent();
	InitializeMeshComponent();
	InitializeProjectileMovementComponent();
}

void AProjectile::InitializeCollisionComponent()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;
	CollisionComp->InitBoxExtent(FVector(10.0f, 10.0f, 10.0f)); // Example size
	CollisionComp->SetCollisionProfileName("Projectile");
	// If you have an OnHit function
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	bReplicates = true;
}

void AProjectile::InitializeMeshComponent()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent); // Attach to the root, which is the collision component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));
	if (MeshAsset.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshAsset.Object);
	}
}
void AProjectile::InitializeProjectileMovementComponent()
{
	//replicate projectile movement

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->SetIsReplicated(true);
	ProjectileMovementComp->SetUpdatedComponent(CollisionComp);
	ProjectileMovementComp->InitialSpeed = 3000.0f;
	ProjectileMovementComp->MaxSpeed = 3000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
	ProjectileMovementComp->Bounciness = 0.3f;
}
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Touché"));
	UE_LOG(LogTemp, Warning, TEXT("Tick"));

	Destroy();
}
