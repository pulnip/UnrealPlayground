// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinRing.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Character.h"
#include "MyPlayerState.h"
#include "TimerManager.h"
#include "UnrealPlayground.h"

// Sets default values
ACoinRing::ACoinRing() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetSphereRadius(60.f);
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    RingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingMesh"));
    RingMesh->SetupAttachment(Trigger);
    RingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Spinner = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Spinner"));
    Spinner->RotationRate = FRotator(0.f, 90.f, 0.f);
}

// Called when the game starts or when spawned
void ACoinRing::BeginPlay() {
	Super::BeginPlay();

    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoinRing::OnTriggerBeginOverlap);
}

void ACoinRing::OnTriggerBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/,
    AActor* OtherActor,
    UPrimitiveComponent* /*OtherComp*/,
    int32 /*OtherBodyIndex*/,
    bool /*bFromSweep*/,
    const FHitResult& /*SweepResult*/
) {
    UE_LOG(LogUnrealPlayground, Log, TEXT("Overlap Coin"));


    if (auto Character = Cast<ACharacter>(OtherActor)) {
        if (auto State = Character->GetPlayerState<AMyPlayerState>()) {
            State->AddCoins(CoinValue);
            Collect();
        }
    }
}

void ACoinRing::Collect() {
    SetActorHiddenInGame(true);
    Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetWorldTimerManager().SetTimer(
        RespawnTimerHandle,
        this,
        &ACoinRing::Respawn,
        RespawnTime,
        false
    );
}

void ACoinRing::Respawn() {
    SetActorHiddenInGame(false);
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
