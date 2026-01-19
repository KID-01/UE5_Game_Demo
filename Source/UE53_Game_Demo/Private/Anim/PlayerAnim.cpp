// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/PlayerAnim.h"
#include "Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// 获取角色引用
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		// 获取角色移动组件
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (PlayerCharacterMovement && PlayerCharacter)
	{
		// 计算速度大小
		Speed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement->Velocity);

		// 判断是否在空中
		bIsInAir = PlayerCharacterMovement->IsFalling();

		// 判断是否正在吸收
		bIsAbsorbing = PlayerCharacter->bIsAbsorbing;// Bug:长按才能生效，在吸收期间可以移动

		// 判断是否在攻击
		bIsAttacking = PlayerCharacter->bIsAttacking;// Bug:无法连击

		// 获取角色的吸收能力等级
		CharacterAbilityLevel = PlayerCharacter->CharacterAbilityLevel;
	}
}
