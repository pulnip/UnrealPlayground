// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class UNREALPLAYGROUND_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TObjectPtr<UInputMappingContext> InputMappingContext;

public:
	AMyPlayerController();

protected:
	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
};
