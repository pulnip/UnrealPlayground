// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "UnrealPlayground.h"

AMyCharacter::AMyCharacter() {
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

void AMyCharacter::PossessedBy(AController* NewController) {
    Super::PossessedBy(NewController);

    AddPawnMappingContext(NewController);
}

void AMyCharacter::UnPossessed() {
    // Super가 Controller를 null로 만들기 전에 정리
    RemovePawnMappingContext(GetController());

    Super::UnPossessed();
}

UEnhancedInputLocalPlayerSubsystem* AMyCharacter::GetInputSubsystem(AController* C) {
    if (auto PlayerController = Cast<APlayerController>(C)) {
        if (PlayerController->GetLocalPlayer() != nullptr) {
            return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                PlayerController->GetLocalPlayer()
            );
        }
    }

    return nullptr;
}

void AMyCharacter::AddPawnMappingContext(AController* C) {
    if (InputMappingContext == nullptr) {
        return;
    }

    if (auto Subsystem = GetInputSubsystem(C)) {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }
}

void AMyCharacter::RemovePawnMappingContext(AController* C) {
    if (InputMappingContext == nullptr) {
        return;
    }

    if (auto Subsystem = GetInputSubsystem(C)) {
        Subsystem->RemoveMappingContext(InputMappingContext);
    }
}
