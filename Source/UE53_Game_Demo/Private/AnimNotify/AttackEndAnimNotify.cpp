// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AttackEndAnimNotify.h"
#include "Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // 添加此行以包含完整类型声明


void UAttackEndAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		Player->GetCharacterMovement()->MaxWalkSpeed = 600.f;
		Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	}
}
