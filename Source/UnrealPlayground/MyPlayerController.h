// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerTypes.h"
#include "MyPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class AMyCharacter;
class AMyCharacter2D;
class AMyCharacter3D;
class AZone2DVolume;

/**
 * 
 */
UCLASS()
class UNREALPLAYGROUND_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> GlobalInputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PawnSwap;

    UPROPERTY(EditDefaultsOnly, Category = "Odyssey")
    TSubclassOf<AMyCharacter2D> Character2DClass;

    UPROPERTY(EditDefaultsOnly, Category = "Odyssey")
    TSubclassOf<AMyCharacter3D> Character3DClass;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Odyssey")
    float CameraBlendTime = 0.6f;

public:
	AMyPlayerController();

    virtual void SetupInputComponent() override;

    // 범용 폰 스왑.
    AMyCharacter* SwapPawn(TSubclassOf<AMyCharacter> NewClass,
        const FTransform& SpawnTM,
        TFunctionRef<void(AMyCharacter*)> PreInit
    );

    UFUNCTION(BlueprintCallable, Category = "Odyssey")
    void EnterZone2D(AZone2DVolume* Zone, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Odyssey")
    void ExitTo3D(const FTransform& SpawnTM);

private:
    void DebugSwap();
};
