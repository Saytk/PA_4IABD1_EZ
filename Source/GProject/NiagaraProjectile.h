#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameplayEffect.h"
#include "Components/SphereComponent.h"
#include "NiagaraProjectile.generated.h"

UCLASS()
class GPROJECT_API ANiagaraProjectile : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ANiagaraProjectile();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    virtual void CustomDestroy();


    virtual void AfterDelay();

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(BlueprintReadOnly)
    float ProjectileHeight;

    UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
    FGameplayEffectSpecHandle DamageEffectSpecHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* NiagaraEffectComponent;

     UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereCollisionComponent;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagraHit")
    UNiagaraSystem* MyNiagaraSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraHit")
    float timerToDestroy;


};
