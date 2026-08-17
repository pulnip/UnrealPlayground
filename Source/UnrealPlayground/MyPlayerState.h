// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinsChanged, int32, NewCoins);

/**
 * 
 */
UCLASS()
class UNREALPLAYGROUND_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Coins")
    int32 Coins = 0;

public:
    UFUNCTION(BlueprintCallable, Category = "Coins")
    void AddCoins(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Coins")
    int32 GetCoins() const { return Coins; }

    UPROPERTY(BlueprintAssignable, Category = "Coins")
    FOnCoinsChanged OnCoinsChanged;
};
