// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyCharacter.h"
#include "Mode2D/MyCharacter2D.h"
#include "Mode3D/MyCharacter3D.h"
#include "UnrealPlayground.h"
#include "Zone2DVolume.h"

AMyPlayerController::AMyPlayerController() = default;

void AMyPlayerController::SetupInputComponent() {
    Super::SetupInputComponent();

    // only add IMC for local player controller
    if (IsLocalPlayerController()) {
        if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
            Subsystem->AddMappingContext(GlobalInputMappingContext, 0);
        }
    }

    if (auto EIC = Cast<UEnhancedInputComponent>(InputComponent)) {
        if (PawnSwap != nullptr) {
            EIC->BindAction(PawnSwap, ETriggerEvent::Started, this, &AMyPlayerController::DebugSwap);
        }
    }
}

AMyCharacter* AMyPlayerController::SwapPawn(
    TSubclassOf<AMyCharacter> NewClass,
    const FTransform& SpawnTM,
    TFunctionRef<void(AMyCharacter*)> PreInit
) {
    if (NewClass == nullptr) {
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (World == nullptr) {
        return nullptr;
    }

    AMyCharacter* Old = Cast<AMyCharacter>(GetPawn());
    const FCharacterCarryOver Carry = Old ? Old->MakeCarryOver() : FCharacterCarryOver();

    // BeginPlay 전에 초기화하려면 Deferred 스폰
    AMyCharacter* NewChar = World->SpawnActorDeferred<AMyCharacter>(
        NewClass, SpawnTM, nullptr, nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    if (NewChar == nullptr) {
        return nullptr;
    }

    PreInit(NewChar);
    NewChar->FinishSpawning(SpawnTM);

    UnPossess();          // Old->UnPossessed()  → IMC 해제
    Possess(NewChar);     // New->PossessedBy()  → IMC 등록

    // NewChar->ApplyCarryOver(Carry);

    // 즉시 Destroy하면 한 프레임 GetPlayerCharacter()가 null이 됨.
    // 사라지기 전까지 새 폰을 밀어내거나 화면에 남지 않도록 먼저 무력화한다.
    if (Old != nullptr) {
        Old->SetActorEnableCollision(false);
        Old->SetActorHiddenInGame(true);
        Old->SetActorTickEnabled(false);
        Old->SetLifeSpan(0.05f);
    }

    return NewChar;
}

void AMyPlayerController::EnterZone2D(AZone2DVolume* Zone, const FVector& SpawnLoc) {
    if (Zone == nullptr) {
        return;
    }

    const FRotator SpawnRot = UKismetMathLibrary::MakeRotFromX(
        FVector::CrossProduct(FVector::UpVector, Zone->GetPlaneNormal())
    );

    const FTransform SpawnTM(SpawnRot, SpawnLoc);

    auto NewChar = SwapPawn(Character2DClass, SpawnTM, [Zone](AMyCharacter* C) {
        if (auto C2D = Cast<AMyCharacter2D>(C)) {
            C2D->InitializeForZone(Zone);
        }
    });
    if (NewChar == nullptr) {
        UE_LOG(LogUnrealPlayground, Error, TEXT("Character2DClass is not set!"));
        return;
    }

    SetViewTargetWithBlend(Zone, CameraBlendTime, EViewTargetBlendFunction::VTBlend_Cubic);
}

void AMyPlayerController::ExitTo3D(const FTransform& SpawnTM) {
    auto NewChar = SwapPawn(Character3DClass, SpawnTM, [](AMyCharacter*) {});
    if (NewChar == nullptr) {
        UE_LOG(LogUnrealPlayground, Error, TEXT("Character3DClass is not set!"));
        return;
    }

    SetViewTargetWithBlend(NewChar, CameraBlendTime, EViewTargetBlendFunction::VTBlend_Cubic);
}

void AMyPlayerController::DebugSwap() {
    auto P = GetPawn();
    if (P == nullptr) {
        return;
    }

    if (P->IsA<AMyCharacter2D>()) {
        ExitTo3D(P->GetActorTransform());
    }
    else {
        // 레벨에 배치한 Zone 하나 찾아서 전환
        if (auto Zone = Cast<AZone2DVolume>(
            UGameplayStatics::GetActorOfClass(this, AZone2DVolume::StaticClass())
        )) {
            EnterZone2D(Zone, Zone->GetPlaneOrigin() + FVector(0, 0, 100));
        }
        else {
            UE_LOG(LogUnrealPlayground, Warning, TEXT("AZone2DVolume Not Found!"));
        }
    }
}
