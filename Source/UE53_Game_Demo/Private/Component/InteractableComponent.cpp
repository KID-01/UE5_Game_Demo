// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InteractableComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Player/PlayerCharacter.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	bIsInteracted = false;
	
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractableComponent::DoInteract(APlayerCharacter* InteractCharacter)
{
	// 防重复交互+空值判断
	if (bIsInteracted || !InteractCharacter || !ReplaceActorClass)
	{
		return;
	}
	bIsInteracted = true;

	// 获取当前挂载该组件的"原物体"
	AActor* OriginActor = GetOwner();
	if (!OriginActor) return;

	// 生成替换后的新物体
	// 在原物体的位置+旋转，生成替换蓝图物体
	UWorld* World = GetWorld();
	if (World)
	{
		World->SpawnActor<AActor>(ReplaceActorClass, OriginActor->GetActorLocation(), OriginActor->GetActorRotation());
	}

	// 给角色永久新增能力
	InteractCharacter->AddNewAbility(); // 这个方法在角色类里实现

	// 隐藏原物体
	OriginActor->SetActorHiddenInGame(true);
	OriginActor->SetActorEnableCollision(false);
}

