// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LandEnemyAnim.generated.h"

class ALandEnemy;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class UE53_GAME_DEMO_API ULandEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 角色引用
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<ALandEnemy> LandEnemy;

	// 角色移动组件
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterMovementComponent> LandEnemyMovement;

	// 速度
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Speed;
	
};
