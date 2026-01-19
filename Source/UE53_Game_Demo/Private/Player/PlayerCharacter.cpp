// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Component/InteractableComponent.h"
#include "Projectile/BallProjectile.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);// 相机摇杆挂在角色的根节点上
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));// 三个参数：点头，摇头，歪头

	// 启用相机平滑延迟
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;  // 值越小，延迟越大，越平滑
	CameraBoom->CameraRotationLagSpeed = 12.0f;  // 旋转延迟速度


	// 相机附着到boom
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	PlayerCamera->SetupAttachment(CameraBoom);


	// 不要让角色随着控制器旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 相机杆旋转，而相机不旋转
	CameraBoom->bUsePawnControlRotation = true;
	PlayerCamera->bUsePawnControlRotation = false;

	// 角色根据运动方向旋转
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	// 最大行走速度
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	// 最大跳跃高度
	GetCharacterMovement()->JumpZVelocity = 600.f;

	bIsAbsorbing = false;
	bIsAttacking = false;
	CharacterAbilityLevel = 0;
	CurrentInteractableComp = nullptr;
	DefaultCharacterMaterial = nullptr;
	UnlockAbilityMaterial = nullptr;
	CanMesh = nullptr;
	HandCanMeshComp = nullptr;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 获取控制此角色的PlayerController
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 获取Enhanced Input子系统
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem && DefaultMapping)
		{
			// 添加输入映射上下文
			Subsystem->AddMappingContext(DefaultMapping, 0);
		}
	}

	// 缓存出生点（PlayerStart）
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(
		GetWorld(), APlayerStart::StaticClass());

	if (PlayerStart)
	{
		PlayerSpawnLocation = PlayerStart->GetActorLocation();
	}
	else
	{
		// 当前初始位置
		PlayerSpawnLocation = GetActorLocation();
	}

	UpdateCharacterMaterialByAbilityLevel();
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bIsAttacking || bIsAbsorbing) { return; }

	FVector2D MoveAxisValue = Value.Get<FVector2D>();
	if (Controller)
	{
		// 这里出问题了，WS和AD错位，所以我把AddMovementInput的参数调换了一下
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		// 向前向后移动
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MoveAxisValue.X);
		// 向左向右移动
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MoveAxisValue.Y);
	}


}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisValue = Value.Get<FVector2D>();

	// 添加灵敏度系数
	float MouseSensitivityX = 0.5f;  // 水平灵敏度
	float MouseSensitivityY = 0.25f;  // 垂直灵敏度

	if (Controller)
	{
		AddControllerPitchInput(LookAxisValue.Y * MouseSensitivityY);
		AddControllerYawInput(-LookAxisValue.X * MouseSensitivityX);
	}
}

void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	if (bIsAttacking || bIsAbsorbing) { return; }

	bool bIsJumpingNow = Value.Get<bool>();
	bIsJumping = bIsJumpingNow;
	if (bIsJumping)
	{
		ACharacter::Jump();
	}
	else
	{
		ACharacter::StopJumping();
	}
}

void APlayerCharacter::Absorb(const FInputActionValue& Value)
{
	bool bIsAbsorbingNow = Value.Get<bool>();
	bIsAbsorbing = bIsAbsorbingNow;


	if (CurrentInteractableComp && bIsAbsorbingNow)
	{
		OnInteractPressed(); // 触发交互逻辑
		return; // 跳过下面的吸收逻辑
	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (bIsJumping) { return; }

	bool bIsAttackingNow = Value.Get<bool>();
	bIsAttacking = bIsAttackingNow;


	if (bIsAttackingNow && CharacterAbilityLevel >= 1 && AttackMontage)
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		

		PlayAnimMontage(AttackMontage);
	}
}

void APlayerCharacter::Fire()
{
	if (BallProjectileClass == nullptr) { return; }

	/*FVector ForwardVector = GetActorForwardVector();
	float SpawnDistance = 40.f;
	FVector SpawnLocation = GetActorLocation() + ForwardVector * SpawnDistance;
	FRotator SpawnRotation = GetActorRotation();
	GetWorld()->SpawnActor<ABallProjectile>(BallProjectileClass, SpawnLocation, SpawnRotation);*/


	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	const FName FireSocketName(TEXT("FireSocket_R"));
	if (!MeshComp->DoesSocketExist(FireSocketName)) return;

	// 位置：右手 Socket（世界坐标）
	FVector SpawnLocation =
		MeshComp->GetSocketLocation(FireSocketName);

	// 方向：只用角色朝向（水平）
	FRotator SpawnRotation = GetActorRotation();
	SpawnRotation.Pitch = 0.f;   // 关键：禁止向上/下
	SpawnRotation.Roll = 0.f;

	// 生成
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	GetWorld()->SpawnActor<ABallProjectile>(
		BallProjectileClass,
		SpawnLocation,
		SpawnRotation,
		Params
	);
}

void APlayerCharacter::CheckNearInteractableActor()
{
	CurrentInteractableComp = nullptr;
	const float CheckDistance = 200.0f; // 和交互组件的InteractDistance一致

	// 获取角色胶囊体的中心位置，作为检测起点
	FVector StartLocation = GetCapsuleComponent()->GetComponentLocation();

	// 检测终点 = 角色位置向前（可交互物体在角色前方才触发）
	FVector EndLocation = StartLocation + GetActorForwardVector() * CheckDistance;

	// 球形检测：检测角色前方范围内的所有Actor
	TArray<FHitResult> HitResults;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this); // 忽略角色自身

	bool bIsHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		StartLocation,
		EndLocation,
		100.0f, // 检测球半径，越大检测范围越广
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic),
		false,
		IgnoreActors,
		EDrawDebugTrace::None, // 关闭调试线
		HitResults,
		true
	);

	// 遍历检测结果，找到挂载了可交互组件的物体
	if (bIsHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				CurrentInteractableComp = HitActor->FindComponentByClass<UInteractableComponent>();
				if (CurrentInteractableComp)
				{
					break; // 找到第一个可交互物体即可
				}
			}
		}
	}
}

void APlayerCharacter::OnInteractPressed()
{
	if (CurrentInteractableComp)
	{
		CurrentInteractableComp->DoInteract(this);
	}
}

void APlayerCharacter::CheckFallOut()
{
	if (bIsRespawning)
	{
		return;
	}

	// 低于坠落阈值
	if (GetActorLocation().Z < FallDeathZ)
	{
		bIsRespawning = true;

		// 停止角色一切运动
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();

		// 传送回出生点
		SetActorLocation(
			PlayerSpawnLocation,
			false,
			nullptr,
			ETeleportType::None
		);

		// 恢复移动
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		bIsRespawning = false;
	}
}

void APlayerCharacter::AddNewAbility()
{
	// 永久解锁能力，数值自增（可扩展多个能力：1=能力1，2=能力2...）
	CharacterAbilityLevel = 1;
	UpdateCharacterMaterialByAbilityLevel();

	if (CharacterAbilityLevel == 1)
	{
		SpawnCanOnHand();
	}
}

void APlayerCharacter::UpdateCharacterMaterialByAbilityLevel()
{
	// 获取角色的Mesh组件
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (!CharacterMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("角色Mesh组件为空，无法切换材质！"));
		return;
	}

	// 声明要使用的目标材质
	UMaterialInterface* TargetMaterial = nullptr;

	// 选择材质
	if (CharacterAbilityLevel >= 1)
	{
		// 能力解锁，使用新材质
		TargetMaterial = UnlockAbilityMaterial;
	}
	else
	{
		// 默认状态，使用原材质
		TargetMaterial = DefaultCharacterMaterial;
	}

	// 判空+防止重复切换材质（优化性能，避免重复赋值）
	if (TargetMaterial && CharacterMesh->GetMaterial(0) != TargetMaterial)
	{
		// 给角色Mesh的第0个材质槽位赋值 (角色的主体材质都在第0槽位)
		CharacterMesh->SetMaterial(0, TargetMaterial);
	}
}

void APlayerCharacter::SpawnCanOnHand()
{
	// 判空：有能力+有罐头模型+还没生成罐头 → 才执行
	if (!IsHasNewAbility() || !CanMesh || HandCanMeshComp) return;

	// 游戏运行中创建组件
	HandCanMeshComp = NewObject<UStaticMeshComponent>(this, TEXT("HandCanMeshComp"));
	HandCanMeshComp->RegisterComponent(); // 必须注册组件，否则不显示

	// 赋值罐头模型+基础设置
	HandCanMeshComp->SetStaticMesh(CanMesh);
	HandCanMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 无碰撞防止卡手
	HandCanMeshComp->SetRelativeScale3D(FVector(0.8f)); // 缩放适配手部

	HandCanMeshComp->AttachToComponent(
		GetMesh(), // 父组件：角色的骨骼网格体
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, // 挂载规则：缩放不继承，防止罐头变形
		FName("middle_01_r")
	);

	// 微调罐头的位置和旋转，完美贴合手掌
	HandCanMeshComp->SetRelativeLocation(FVector(0.0f, 5.0f, -5.0f));
	HandCanMeshComp->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
}

void APlayerCharacter::PlayAnimMontageSimple(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay != nullptr)
	{
		PlayAnimMontage(MontageToPlay);
	}
}





// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckFallOut();

	CheckNearInteractableActor();
}

// Called to bind functionality to input输入组件
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(AbsorbAction, ETriggerEvent::Started, this, &APlayerCharacter::Absorb);
		EnhancedInputComponent->BindAction(AbsorbAction, ETriggerEvent::Completed, this, &APlayerCharacter::Absorb);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::Attack);
	}

}

