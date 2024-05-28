#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProjectileLauncherComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GPROJECT_API UProjectileLauncherComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UProjectileLauncherComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void LaunchProjectile();

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float LaunchSpeed = 1000.0f;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void SpawnProjectile();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLaunchProjectile();
};