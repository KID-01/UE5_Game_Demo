// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ThrowCanAnimNotify.h"
#include "Player/PlayerCharacter.h"


void UThrowCanAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (APlayerCharacter* Player =Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire called"));

		Player->Fire();
	}
}
