// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinRing.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class URotatingMovementComponent;

UCLASS()
class UNREALPLAYGROUND_API ACoinRing : public AActor
{
	GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<USphereComponent> Trigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<UStaticMeshComponent> RingMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<URotatingMovementComponent> Spinner;

    UPROPERTY(EditAnywhere, Category = "Ring")
    float RespawnTime = 10.f;

    UPROPERTY(EditAnywhere, Category = "Ring")
    int32 CoinValue = 1;

private:
    FTimerHandle RespawnTimerHandle;
	
public:	
	// Sets default values for this actor's properties
	ACoinRing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    void Collect();
    void Respawn();
};
