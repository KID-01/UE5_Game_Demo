// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"


class APlayerCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class UE53_GAME_DEMO_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 角色引用
	UPROPERTY(BlueprintReadOnly, Category="Character")
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	// 角色移动组件
	UPROPERTY(BlueprintReadOnly, Category="Character")
	TObjectPtr<UCharacterMovementComponent> PlayerCharacterMovement;

	// 速度
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	float Speed;

	// 是否在空中
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	// 是否正在吸收
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsAbsorbing;

	// 是否在攻击
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bIsAttacking;

	// 吸收能力
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	int32 CharacterAbilityLevel;
};
