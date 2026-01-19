// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"


class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE53_GAME_DEMO_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "交互系统")
	void DoInteract(APlayerCharacter* InteractCharacter);

private:
	// 交互后要生成的"替换物体蓝图类"
	// Bug:设置的属性不会保存，每次打开编辑器都要重新设置，不知道成型的游戏会不会有这个问题
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactable Setting", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ReplaceActorClass;

	// 交互触发的距离（角色靠近多少厘米内可以交互，默认200cm，蓝图可修改）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable Setting", meta = (AllowPrivateAccess = "true"))
	float InteractDistance = 200.0f;

	// 是否已交互过（防止重复交互）
	bool bIsInteracted = false;
};
