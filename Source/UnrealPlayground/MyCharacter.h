// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerTypes.h"
#include "MyCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;

UCLASS(Abstract)
class UNREALPLAYGROUND_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> InputMappingContext;

public:
    AMyCharacter();

    virtual void PossessedBy(AController*) override;
    virtual void UnPossessed() override;

    // 이전 폰의 상태를 이어받음
    virtual void ApplyCarryOver(const FCharacterCarryOver&) {}
    // 다음 폰에게 넘길 상태를 만듦
    virtual FCharacterCarryOver MakeCarryOver() const {
        return {};
    }

protected:
    static UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem(AController*);

private:
    void AddPawnMappingContext(AController*);
    void RemovePawnMappingContext(AController*);
};
