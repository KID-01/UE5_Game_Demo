// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FlyEnemy.generated.h"

class ABallProjectile;

UCLASS()
class UE53_GAME_DEMO_API AFlyEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFlyEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// SpawnActor
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABallProjectile> BallProjectileClass;

	// ¿ª»ð¼ä¸ô
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Fire", meta = (MinValue = "0.5"))
	float FireInterval = 1.0f;

	void Fire();

private:
	float FireTimer = 0.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
