// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cannon.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UArrowComponent;
class ACannonProjectile;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class UNREALPLAYGROUND_API ACannon : public AActor
{
	GENERATED_BODY()

protected:
    /** 고정된 받침대 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon")
    TObjectPtr<UStaticMeshComponent> BaseMesh;

    /** 회전하는 포신 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon")
    TObjectPtr<UStaticMeshComponent> BarrelMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon")
    TObjectPtr<UArrowComponent> Muzzle;

    UPROPERTY(EditAnywhere, Category = "Cannon")
    TSubclassOf<ACannonProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Cannon", meta = (ClampMin = "0.1"))
    float FireInterval = 2.f;

    UPROPERTY(EditAnywhere, Category = "Cannon")
    float FirstDelay = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Cannon|Aim")
    bool bTrackPlayer = false;

    UPROPERTY(EditAnywhere, Category = "Cannon|Aim", meta = (EditCondition = "bTrackPlayer"))
    float TrackRange = 2500.f;

    UPROPERTY(EditAnywhere, Category = "Cannon|Aim", meta = (EditCondition = "bTrackPlayer"))
    float TurnSpeed = 90.f;   // deg/sec

    UPROPERTY(EditAnywhere, Category = "Cannon|FX")
    TObjectPtr<UNiagaraSystem> MuzzleFX;

    UPROPERTY(EditAnywhere, Category = "Cannon|FX")
    TObjectPtr<USoundBase> FireSound;

private:
    FTimerHandle FireTimerHandle;
	
public:
	// Sets default values for this actor's properties
	ACannon();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Cannon")
    void Fire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
