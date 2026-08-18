// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MyCharacter.h"
#include "MyCharacter2D.generated.h"

class AZone2DVolume;
class UPaperFlipbookComponent;
class UPaperFlipbook;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class UNREALPLAYGROUND_API AMyCharacter2D : public AMyCharacter
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
    TObjectPtr<UPaperFlipbookComponent> Flipbook;

    UPROPERTY(EditDefaultsOnly, Category = "Sprite")
    TObjectPtr<UPaperFlipbook> IdleAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Sprite")
    TObjectPtr<UPaperFlipbook> RunAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Sprite")
    TObjectPtr<UPaperFlipbook> JumpAnim;

    /** 뒤쪽 벽까지의 거리에 맞춰 이동 평면을 따라가게 할지. 벽 지오메트리가 정리되기 전엔 꺼둘 것 */
    UPROPERTY(EditAnywhere, Category = "Zone2D")
    bool bDepthSnap = false;

    /** 캐릭터에서 평면 노멀 반대 방향으로 훑는 거리 */
    UPROPERTY(EditAnywhere, Category = "Zone2D", meta = (EditCondition = "bDepthSnap"))
    float SnapProbeDist = 300.f;

    UPROPERTY(EditAnywhere, Category = "Zone2D", meta = (EditCondition = "bDepthSnap"))
    float SnapProbeRadius = 30.f;

    /** 벽과 캡슐 표면 사이 여유 */
    UPROPERTY(EditAnywhere, Category = "Zone2D", meta = (EditCondition = "bDepthSnap"))
    float SnapSkin = 5.f;

    /** 평면 원점 이동 보간 속도. 0이면 즉시 스냅 */
    UPROPERTY(EditAnywhere, Category = "Zone2D", meta = (EditCondition = "bDepthSnap"))
    float SnapInterpSpeed = 10.f;

    UPROPERTY(EditAnywhere, Category = "Zone2D|Debug")
    bool bDrawDebug = false;

private:
    UPROPERTY() TObjectPtr<AZone2DVolume> ActiveZone = nullptr;

    FVector PlaneNormal = FVector::ForwardVector;
    FVector PlaneRight = FVector::RightVector;
    FVector PlaneOrigin = FVector::ZeroVector;
    float   LastFacing = 1.f;

    /** 사용할 플립북이 하나라도 지정돼 있는가 (BeginPlay에서 결정) */
    bool bHasFlipbook = false;

public:
    AMyCharacter2D();

    virtual void Tick(float DeltaTime) override;

    // FinishSpawning 전에 호출할 것
    UFUNCTION(BlueprintCallable, Category = "Zone2D")
    void InitializeForZone(AZone2DVolume* Zone);

    FVector GetPlaneRight() const { return PlaneRight; }

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(UInputComponent*) override;

    void UpdateFlipbook();
    void UpdateDepthSnap(float DeltaTime);

private:
    /** PlaneNormal/PlaneOrigin을 CharacterMovement에 반영 */
    void ApplyPlaneConstraint();

    void Move(const FInputActionValue&);
};
