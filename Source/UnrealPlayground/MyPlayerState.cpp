// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "UnrealPlayground.h"

void AMyPlayerState::AddCoins(int32 Amount) {
    Coins += Amount;
    OnCoinsChanged.Broadcast(Coins);

    UE_LOG(LogUnrealPlayground, Log, TEXT("AddCoins"));
}
