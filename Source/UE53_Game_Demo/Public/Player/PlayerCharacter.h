// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"

#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UInteractableComponent;
class UMaterialInterface;
class StaticMeshComponent;
class ABallProjectile;
class UAnimationMontage;
class UAnimInstance;


UCLASS()
class UE53_GAME_DEMO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void Absorb(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

	// SpawnActor
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABallProjectile> BallProjectileClass;



private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> PlayerCamera;


	// Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AbsorbAction;

	// 通用攻击动作，先设计扔可乐动作
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;


	UPROPERTY()
	UInteractableComponent* CurrentInteractableComp = nullptr;

	// 检测附近的可交互物体（每帧执行，检测范围和交互组件一致）
	void CheckNearInteractableActor();

	// 触发交互逻辑
	void OnInteractPressed();

	void CheckFallOut();


public:
	UPROPERTY(BlueprintReadOnly, Category = "Player|Ability")
	bool bIsAbsorbing;

	UPROPERTY(BlueprintReadOnly, Category = "Player|Ability")
	bool bIsAttacking;

	UPROPERTY(BlueprintReadOnly, Category = "Player|Ability")
	bool bIsJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "角色能力")
	int32 CharacterAbilityLevel = 0;

	UFUNCTION(BlueprintCallable, Category = "角色能力")
	void AddNewAbility();

	UFUNCTION(BlueprintPure, Category = "角色能力")
	bool IsHasNewAbility() const { return CharacterAbilityLevel >= 1; }

	// 角色默认材质 (CharacterAbilityLevel = 0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Change")
	UMaterialInterface* DefaultCharacterMaterial;

	// 能力解锁后的材质 (CharacterAbilityLevel = 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Change")
	UMaterialInterface* UnlockAbilityMaterial;

	// 材质切换函数
	UFUNCTION(BlueprintCallable, Category = "Material Change")
	void UpdateCharacterMaterialByAbilityLevel();

	// 可乐罐的静态网格体模型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Ability - Throw ColaCan")
	UStaticMesh* CanMesh;

	// 手部挂载的可乐罐组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Ability - Throw ColaCan")
	UStaticMeshComponent* HandCanMeshComp;

	// 角色手部生成可乐罐
	void SpawnCanOnHand();

	UFUNCTION(BlueprintCallable, Category = "Player Attack")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Mon")
	void PlayAnimMontageSimple(UAnimMontage* MontageToPlay);

	// 攻击蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mon")
	UAnimMontage* AttackMontage;

	// 玩家出生点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Respawn")
	FVector PlayerSpawnLocation;

	// 坠落判定高度（Z 轴）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	float FallDeathZ = -1000.f;

	// 是否正在重生（防止重复触发）
	bool bIsRespawning = false;

	// 角色获得新能力的音效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* GainAbilitySound;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
