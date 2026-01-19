// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "LandEnemy.generated.h"


class APlayerCharacter;

UCLASS()
class UE53_GAME_DEMO_API ALandEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALandEnemy();

	bool LineTraceActor(const AActor* TargetActor);

	bool CanSeeActor(const AActor* TargetActor, FVector Start, FVector End) const;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveSpeed = 100.f;

	void MoveToPlayer(float DeltaTime);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bCanSeePlayer = false;

	// 接触检测碰撞体
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<UBoxComponent> DamageBox;

	// 重叠回调
	UFUNCTION()
	void OnDamageBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	TObjectPtr<ACharacter> TargetPlayerCharacter;

	// 玩家出生点
	FVector PlayerSpawnLocation;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
