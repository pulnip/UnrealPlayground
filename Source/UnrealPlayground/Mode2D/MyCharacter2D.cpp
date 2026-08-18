// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter2D.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "PaperFlipbookComponent.h"
#include "../UnrealPlayground.h"
#include "../Zone2DVolume.h"

AMyCharacter2D::AMyCharacter2D() {
    PrimaryActorTick.bCanEverTick = true;

    auto Capsule = GetCapsuleComponent();
    Capsule->InitCapsuleSize(34.f, 80.f);

    auto Move = GetCharacterMovement();
    Move->bOrientRotationToMovement = false;   // 스프라이트는 Scale로 뒤집음
    Move->JumpZVelocity = 620.f;
    Move->AirControl = 1.0f;    // 2D는 공중 완전 제어
    Move->GravityScale = 2.6f;    // 묵직한 낙하
    Move->MaxWalkSpeed = 400.f;
    Move->GroundFriction = 8.f;
    Move->BrakingDecelerationWalking = 2400.f;

    // 3D 메시는 플립북 유무에 따라 BeginPlay에서 켜고 끈다.
    // (생성자 값은 CDO 기준이라 BP에서 지정한 플립북을 아직 알 수 없음)
    if (auto SkeletalMesh = GetMesh()) {
        SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
    Flipbook->SetupAttachment(Capsule);
    Flipbook->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Flipbook->SetRelativeLocation(FVector(0.f, 0.f, -10.f));
}

void AMyCharacter2D::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    UpdateFlipbook();
    UpdateDepthSnap(DeltaTime);
}

void AMyCharacter2D::InitializeForZone(AZone2DVolume* Zone) {
    if (Zone == nullptr) { return; }

    ActiveZone = Zone;
    PlaneNormal = Zone->GetPlaneNormal();
    PlaneRight = FVector::CrossProduct(FVector::UpVector, PlaneNormal).GetSafeNormal();
    PlaneOrigin = Zone->GetPlaneOrigin();

    ApplyPlaneConstraint();
}

void AMyCharacter2D::ApplyPlaneConstraint() {
    UCharacterMovementComponent* Move = GetCharacterMovement();
    if (Move == nullptr) { return; }

    Move->SetPlaneConstraintEnabled(true);   // bConstrainToPlane = true
    Move->SetPlaneConstraintNormal(PlaneNormal);
    Move->SetPlaneConstraintOrigin(PlaneOrigin);
    Move->bSnapToPlaneAtStart = true;
}

void AMyCharacter2D::BeginPlay() {
    Super::BeginPlay();

    // BP의 ConstructionScript는 FinishSpawning 시점에 돌기 때문에
    // InitializeForZone(=PreInit)에서 넣은 값이 되돌려질 여지가 있다. 여기서 한 번 더 적용.
    if (ActiveZone != nullptr) {
        ApplyPlaneConstraint();
        GetCharacterMovement()->SnapUpdatedComponentToPlane();
    }

    // 플립북 에셋이 하나도 없으면 캐릭터가 완전히 투명해지므로 3D 메시로 대체 표시한다.
    bHasFlipbook = (IdleAnim != nullptr || RunAnim != nullptr || JumpAnim != nullptr);

    if (auto SkeletalMesh = GetMesh()) {
        SkeletalMesh->SetVisibility(!bHasFlipbook);
    }
    if (Flipbook != nullptr) {
        Flipbook->SetVisibility(bHasFlipbook);
    }

    if (!bHasFlipbook) {
        UE_LOG(LogUnrealPlayground, Warning,
            TEXT("[%s] Idle/Run/JumpAnim이 모두 비어 있습니다. 3D 메시로 대체 표시합니다."),
            *GetName()
        );
    }
}

void AMyCharacter2D::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
        if (MoveAction != nullptr) {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter2D::Move);
        }
        else {
            UE_LOG(LogUnrealPlayground, Warning, TEXT("[%s] MoveAction 미지정"), *GetName());
        }

        if (JumpAction != nullptr) {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        }
        else {
            UE_LOG(LogUnrealPlayground, Warning, TEXT("[%s] JumpAction 미지정"), *GetName());
        }
    }
}

void AMyCharacter2D::UpdateFlipbook() {
    if (Flipbook == nullptr || !bHasFlipbook) {
        return;
    }

    // Paper2D 스프라이트는 로컬 XZ 평면에 놓이고 정면 노멀이 -Y다.
    // MakeFromX(PlaneRight)는 Y = Up x PlaneRight = -PlaneNormal 이 되어
    // 스프라이트 앞면이 정확히 ZoneCamera 쪽(= +PlaneNormal)을 향한다.
    Flipbook->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(PlaneRight));

    // 좌우 반전은 스프라이트 평면 안의 축인 X로 (Y는 두께 축이라 효과가 없다)
    const float Facing = FVector::DotProduct(GetVelocity(), PlaneRight);
    if (FMath::Abs(Facing) > 10.f) {
        LastFacing = FMath::Sign(Facing);
    }
    FVector S = Flipbook->GetRelativeScale3D();
    S.X = LastFacing * FMath::Abs(S.X);
    Flipbook->SetRelativeScale3D(S);

    // 애니메이션 선택
    UPaperFlipbook* Desired = IdleAnim;
    if (GetCharacterMovement()->IsFalling()) {
        Desired = JumpAnim;
    }
    else if (GetVelocity().SizeSquared2D() > 100.f) {
        Desired = RunAnim;
    }

    if (Desired != nullptr && Flipbook->GetFlipbook() != Desired) {
        Flipbook->SetFlipbook(Desired);
    }
}

void AMyCharacter2D::UpdateDepthSnap(float DeltaTime) {
    if (!bDepthSnap || ActiveZone == nullptr) {
        return;
    }

    // 캐릭터 위치에서 "뒤쪽"(평면 노멀 반대)으로만 훑는다.
    // 캐릭터 앞에서 시작하면 벽이 아닌 엉뚱한 지오메트리를 먼저 잡아
    // 평면 원점이 매 프레임 튀면서 캐릭터가 깊이 방향으로 순간이동한다.
    const FVector Start = GetActorLocation();
    const FVector End = Start - PlaneNormal * SnapProbeDist;

    FCollisionQueryParams Params(SCENE_QUERY_STAT(DepthSnap), false, this);

    FHitResult Hit;
    const bool bHit = GetWorld()->SweepSingleByChannel(
        Hit,
        Start, End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(SnapProbeRadius),
        Params
    );

    const bool bValidHit = bHit && !Hit.bStartPenetrating;

#if !UE_BUILD_SHIPPING
    if (bDrawDebug) {
        DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 0.f, 0, 2.f);
        if (bValidHit) {
            DrawDebugSphere(GetWorld(), Hit.Location, SnapProbeRadius, 12, FColor::Red, false, 0.f);
            DrawDebugDirectionalArrow(GetWorld(), Hit.ImpactPoint,
                Hit.ImpactPoint + Hit.ImpactNormal * 100.f, 20.f, FColor::Green, false, 0.f
            );
        }
    }
#endif

    if (!bValidHit) {
        return;
    }

    UCharacterMovementComponent* Move = GetCharacterMovement();

    // ImpactPoint(=벽 표면)에서 캡슐 반지름 + 여유만큼 띄운다.
    // Hit.Location은 이미 스윕 구체 중심이라 여기에 더하면 SnapProbeRadius만큼 이중으로 밀린다.
    const float Skin = GetCapsuleComponent()->GetScaledCapsuleRadius() + SnapSkin;
    const FVector Desired = Hit.ImpactPoint + PlaneNormal * Skin;

    PlaneOrigin = (SnapInterpSpeed > 0.f)
        ? FMath::VInterpTo(Move->GetPlaneConstraintOrigin(), Desired, DeltaTime, SnapInterpSpeed)
        : Desired;

    Move->SetPlaneConstraintOrigin(PlaneOrigin);
}

void AMyCharacter2D::Move(const FInputActionValue& Value) {
    const float Axis = Value.Get<FVector2D>().X;
    if (!FMath::IsNearlyZero(Axis)) {
        AddMovementInput(PlaneRight, Axis);
    }
}
