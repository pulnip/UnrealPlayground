// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include "CannonProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"

// Sets default values
ACannon::ACannon()
{
 	PrimaryActorTick.bCanEverTick = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    SetRootComponent(BaseMesh);

    BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMesh"));
    BarrelMesh->SetupAttachment(BaseMesh);
    BarrelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle"));
    Muzzle->SetupAttachment(BaseMesh);
    Muzzle->SetRelativeLocation(FVector(90.f, 0.f, 0.f));  // 포신 앞쪽으로
    Muzzle->ArrowSize = 1.5f;
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();

    GetWorldTimerManager().SetTimer(
        FireTimerHandle, this, &ACannon::Fire,
        FireInterval, /*bLoop=*/true, FirstDelay
    );
}

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!bTrackPlayer) {
        return;
    }

    if (auto Player = UGameplayStatics::GetPlayerCharacter(this, 0)) {
        const auto ToPlayer = Player->GetActorLocation() - BaseMesh->GetComponentLocation();
        if (ToPlayer.SizeSquared() > FMath::Square(TrackRange)) { return; }

        const FRotator Desired = UKismetMathLibrary::MakeRotFromX(ToPlayer);
        const FRotator Current = BaseMesh->GetComponentRotation();
        const FRotator NewRot = FMath::RInterpConstantTo(Current, Desired, DeltaTime, TurnSpeed);

        BaseMesh->SetWorldRotation(NewRot);
    }
}

void ACannon::Fire() {
    if (ProjectileClass == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("[%s] ProjectileClass가 비어있음!"), *GetName());
        return;
    }

    if (auto World = GetWorld()) {
        const FVector  SpawnLoc = Muzzle->GetComponentLocation();
        const FRotator SpawnRot = Muzzle->GetComponentRotation();

        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = GetInstigator();
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        auto Projectile = World->SpawnActor<ACannonProjectile>(
            ProjectileClass,
            SpawnLoc,
            SpawnRot,
            Params
        );

        if (Projectile != nullptr) {
            Projectile->FireInDirection(SpawnRot.Vector());
        }

        if (MuzzleFX != nullptr) {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, MuzzleFX, SpawnLoc, SpawnRot);
        }
        if (FireSound != nullptr) {
            UGameplayStatics::PlaySoundAtLocation(this, FireSound, SpawnLoc);
        }
    }
}
