// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/FlyEnemy.h"
#include "Projectile/BallProjectile.h"

// Sets default values
AFlyEnemy::AFlyEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFlyEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlyEnemy::Fire()
{
	if (BallProjectileClass == nullptr) { return; }

	//FVector ForwardVector = GetActorForwardVector();

	//// 水平发射
	//ForwardVector.Z = 0.f;
	//ForwardVector.Normalize();
	//
	//float SpawnDistance = 40.f;
	//
	//FVector SpawnLocation = GetActorLocation() + ForwardVector * SpawnDistance;
	//
	//FRotator SpawnRotation = ForwardVector.Rotation(); // 水平发射

	//GetWorld()->SpawnActor<ABallProjectile>(BallProjectileClass, SpawnLocation, SpawnRotation);

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	const FName FireSocketName(TEXT("BODY"));
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

// Called every frame
void AFlyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTimer += DeltaTime;
	if (FireTimer >= FireInterval)
	{
		Fire();
		FireTimer = 0.f;
	}
}

// Called to bind functionality to input
void AFlyEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

