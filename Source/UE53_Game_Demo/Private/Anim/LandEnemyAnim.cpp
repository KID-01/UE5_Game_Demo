// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/LandEnemyAnim.h"
#include "Enemy/LandEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void ULandEnemyAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// 获取角色引用
	LandEnemy = Cast<ALandEnemy>(TryGetPawnOwner());
	if (LandEnemy)
	{
		// 获取角色移动组件
		LandEnemyMovement = LandEnemy->GetCharacterMovement();
	}
}

void ULandEnemyAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (LandEnemyMovement && LandEnemy)
	{
		// 计算速度大小
		Speed = UKismetMathLibrary::VSizeXY(LandEnemyMovement->Velocity);
	}
}
