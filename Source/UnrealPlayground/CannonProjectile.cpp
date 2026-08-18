// Fill out your copyright notice in the Description page of Project Settings.


#include "CannonProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACannonProjectile::ACannonProjectile() {
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(Collision);
    Collision->InitSphereRadius(22.f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Block);
    Collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

    Collision->SetNotifyRigidBodyCollision(true);   // = bSimulationGeneratesHitEvents

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->SetUpdatedComponent(Collision);
    ProjectileMovement->InitialSpeed = 1200.f;
    ProjectileMovement->MaxSpeed = 1200.f;
    ProjectileMovement->bRotationFollowsVelocity = true;  // 포탄이 진행방향 보게
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;     // 0=직선(킬러/총알빌), 1=포물선

    InitialLifeSpan = 6.f;   // 6초 뒤 자동 Destroy
}

void ACannonProjectile::FireInDirection(const FVector& ShootDirection) {
    ProjectileMovement->Velocity = ShootDirection.GetSafeNormal() * ProjectileMovement->InitialSpeed;
}

// Called when the game starts or when spawned
void ACannonProjectile::BeginPlay()
{
    Super::BeginPlay();

    Collision->OnComponentHit.AddDynamic(this, &ACannonProjectile::OnHit);

    if (auto MyOwner = GetOwner()) {
        Collision->IgnoreActorWhenMoving(MyOwner, true);
    }
}

void ACannonProjectile::OnHit(UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit
) {
    if (OtherActor == this || OtherActor == GetOwner()) {
        return;
    }

    if (auto Char = Cast<ACharacter>(OtherActor)) {
        const auto Dir = ProjectileMovement->Velocity.GetSafeNormal();
        const auto Launch = (Dir + FVector::UpVector * 0.6f).GetSafeNormal() * KnockbackStrength;
        Char->LaunchCharacter(Launch, true, true);

        // TODO: 여기서 PlayerState 체력 깎기 등
    }
    else if (OtherComp && OtherComp->IsSimulatingPhysics()) {
        OtherComp->AddImpulseAtLocation(ProjectileMovement->Velocity * 80.f, Hit.ImpactPoint);
    }

    Explode(Hit.ImpactPoint);
}

void ACannonProjectile::Explode(const FVector& Location) {
    if (ImpactFX != nullptr) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, Location);
    }
    if (ImpactSound != nullptr) {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
    }
    Destroy();
}

