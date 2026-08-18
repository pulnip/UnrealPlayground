// Fill out your copyright notice in the Description page of Project Settings.

#include "Zone2DVolume.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealPlayground.h"

// Sets default values
AZone2DVolume::AZone2DVolume() {
 	PrimaryActorTick.bCanEverTick = false;

    Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
    SetRootComponent(Bounds);
    Bounds->SetBoxExtent(FVector(50.f, 800.f, 400.f));
    Bounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    NormalArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("NormalArrow"));
    NormalArrow->SetupAttachment(Bounds);
    NormalArrow->ArrowSize = 3.f;
    NormalArrow->ArrowColor = FColor::Cyan;

    ZoneCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ZoneCamera"));
    ZoneCamera->SetupAttachment(Bounds);
    // 액터 +X 방향(=노멀)으로 CameraDistance 떨어진 곳에서 뒤돌아보게
    ZoneCamera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
    ZoneCamera->bUsePawnControlRotation = false;

    ApplyCameraSettings();
}

void AZone2DVolume::OnConstruction(const FTransform& Transform) {
    Super::OnConstruction(Transform);

#if WITH_EDITOR
    AlignToWall(Transform);
#endif

    ApplyCameraSettings();
}

void AZone2DVolume::BeginPlay() {
    Super::BeginPlay();

    // 쿠킹된 빌드에서는 레벨 배치 액터의 OnConstruction이 돌지 않으므로 여기서도 적용
    ApplyCameraSettings();
}

void AZone2DVolume::ApplyCameraSettings() {
    if (ZoneCamera == nullptr) {
        return;
    }

    ZoneCamera->SetRelativeLocation(FVector(CameraDistance, 0.f, 0.f));

    if (bOrthographic) {
        ZoneCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
        ZoneCamera->OrthoWidth = OrthoWidth;
    }
    else {
        ZoneCamera->ProjectionMode = ECameraProjectionMode::Perspective;
        ZoneCamera->FieldOfView = CameraFOV;
    }
}

#if WITH_EDITOR
void AZone2DVolume::AlignToWall(const FTransform& Transform) {
    if (!bAutoAlignToWall) {
        return;
    }

    UWorld* World = GetWorld();
    if (World == nullptr) {
        return;
    }
    // 블루프린트 에디터 프리뷰 월드에는 레벨 지오메트리가 없다
    if (World->WorldType == EWorldType::EditorPreview || World->WorldType == EWorldType::GamePreview) {
        return;
    }

    // 현재 -X 방향(벽 쪽)으로 레이 쏴서 표면 노멀 획득.
    // OnConstruction 중 액터 트랜스폼은 인자로 온 Transform이 기준이다.
    const FVector Loc = Transform.GetLocation();
    const FVector Dir = -Transform.GetRotation().GetForwardVector();

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ZoneAlign), false, this);

    if (!World->LineTraceSingleByChannel(Hit, Loc, Loc + Dir * WallProbeDist, ECC_Visibility, Params)) {
        UE_LOG(LogUnrealPlayground, Warning,
            TEXT("[%s] AutoAlignToWall: 뒤쪽 %.0f 유닛 안에서 벽을 찾지 못했습니다."),
            *GetName(), WallProbeDist
        );
        return;
    }

    // 벽 노멀이 곧 우리 +X가 되도록 회전.
    // 단 yaw만 쓴다. 경사면/바닥을 맞히면 존과 카메라가 통째로 기울어버린다.
    FVector N = Hit.ImpactNormal;
    N.Z = 0.f;
    if (!N.Normalize()) {
        UE_LOG(LogUnrealPlayground, Warning,
            TEXT("[%s] AutoAlignToWall: 바닥/천장을 맞혔습니다 (수평 성분 없음)."), *GetName()
        );
        return;
    }

    const FRotator Desired = UKismetMathLibrary::MakeRotFromX(N);

    // OnConstruction 안에서 트랜스폼을 바꾸면 재구성이 다시 걸릴 수 있으므로
    // 실제로 달라졌을 때만 적용한다.
    if (!Desired.Equals(GetActorRotation(), 0.1f)) {
        SetActorRotation(Desired);
    }
}
#endif

FVector AZone2DVolume::GetPlaneNormal() const {
    return GetActorForwardVector();   // 액터 +X = 노멀
}

void AZone2DVolume::DrawZoneDebug() const {
    const FVector Origin = GetPlaneOrigin();
    const FVector Normal = GetPlaneNormal();
    const FVector Right = FVector::CrossProduct(FVector::UpVector, Normal).GetSafeNormal();
    const FVector Ext = Bounds->GetScaledBoxExtent();

    DrawDebugSolidPlane(GetWorld(),
        FPlane(Origin, Normal), Origin,
        FVector2D(Ext.Y, Ext.Z),
        FColor(0, 200, 255, 40),
        /*bPersistentLines=*/false, /*LifeTime=*/0.f, /*DepthPriority=*/0
    );

    DrawDebugDirectionalArrow(GetWorld(),
        Origin, Origin + Right * 200.f, 30.f, FColor::Green,
        false, 0.f, 0, 4.f
    );
}
