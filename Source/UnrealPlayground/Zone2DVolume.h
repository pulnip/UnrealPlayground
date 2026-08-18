// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Zone2DVolume.generated.h"

class UBoxComponent;
class UCameraComponent;
class UArrowComponent;


UCLASS()
class UNREALPLAYGROUND_API AZone2DVolume : public AActor
{
	GENERATED_BODY()

protected:
    /** 구역 경계 (디버그/범위 판정용) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone2D")
    TObjectPtr<UBoxComponent> Bounds;

    /** 이 방향이 평면 노멀. 에디터에서 화살표로 보임 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone2D")
    TObjectPtr<UArrowComponent> NormalArrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone2D")
    TObjectPtr<UCameraComponent> ZoneCamera;

    /** 평면에서 카메라까지의 거리 (노멀 방향) */
    UPROPERTY(EditAnywhere, Category = "Zone2D|Camera")
    float CameraDistance = 800.f;

    /** 사이드스크롤러는 직교 투영이 원근 왜곡이 없어 다루기 쉽다 */
    UPROPERTY(EditAnywhere, Category = "Zone2D|Camera")
    bool bOrthographic = true;

    /** 직교 투영일 때 화면에 담기는 가로 폭 */
    UPROPERTY(EditAnywhere, Category = "Zone2D|Camera", meta = (EditCondition = "bOrthographic"))
    float OrthoWidth = 1600.f;

    /** 원근 투영일 때 수평 시야각. 20도 같은 값은 화면이 수백 유닛밖에 안 돼 캐릭터가 프레임 밖으로 나간다 */
    UPROPERTY(EditAnywhere, Category = "Zone2D|Camera", meta = (EditCondition = "!bOrthographic"))
    float CameraFOV = 70.f;

    // 뒤쪽 벽 자동 감지
    UPROPERTY(EditAnywhere, Category = "Zone2D|AutoAlign")
    bool bAutoAlignToWall = false;

    /** 벽을 찾을 최대 거리. 볼륨이 이보다 멀리 있으면 정렬되지 않는다 */
    UPROPERTY(EditAnywhere, Category = "Zone2D|AutoAlign", meta = (EditCondition = "bAutoAlignToWall"))
    float WallProbeDist = 500.f;

public:	
	// Sets default values for this actor's properties
	AZone2DVolume();

    virtual void OnConstruction(const FTransform& Transform) override;

    /** 이 구역의 이동 평면 노멀 (= 카메라가 바라보는 방향의 반대) */
    UFUNCTION(BlueprintPure, Category = "Zone2D")
    FVector GetPlaneNormal() const;

    UFUNCTION(BlueprintPure, Category = "Zone2D")
    FVector GetPlaneOrigin() const { return GetActorLocation(); }

    UFUNCTION(BlueprintPure, Category = "Zone2D")
    UCameraComponent* GetZoneCamera() const { return ZoneCamera; }

protected:
    virtual void BeginPlay() override;

private:
    /** 에디터 프로퍼티를 실제 카메라 컴포넌트에 반영 */
    void ApplyCameraSettings();

#if WITH_EDITOR
    /** 뒤쪽 벽을 찾아 액터 +X를 벽 노멀에 맞춤 (에디터 전용) */
    void AlignToWall(const FTransform& Transform);
#endif

    void DrawZoneDebug() const;
};
