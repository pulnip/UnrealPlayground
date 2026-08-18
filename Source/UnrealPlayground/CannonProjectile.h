// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CannonProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class UNREALPLAYGROUND_API ACannonProjectile : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(EditAnywhere, Category = "Projectile")
    float KnockbackStrength = 900.f;

    UPROPERTY(EditAnywhere, Category = "Projectile|FX")
    TObjectPtr<UNiagaraSystem> ImpactFX;

    UPROPERTY(EditAnywhere, Category = "Projectile|FX")
    TObjectPtr<USoundBase> ImpactSound;

public:
    // Sets default values for this actor's properties
    ACannonProjectile();

    void FireInDirection(const FVector& ShootDirection);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

    void Explode(const FVector& Location);
};
