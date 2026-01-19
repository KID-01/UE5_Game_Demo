// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LandEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ALandEnemy::ALandEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
	DamageBox->SetupAttachment(GetRootComponent());


	// 碰撞体大小
	DamageBox->SetBoxExtent(FVector(60.f, 60.f, 80.f));

	// 只用来检测重叠
	DamageBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageBox->SetCollisionObjectType(ECC_WorldDynamic);
	DamageBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

}

void ALandEnemy::MoveToPlayer(float DeltaTime)
{
	if (!TargetPlayerCharacter) return;

	// 敌人 → 玩家 的方向（只在水平面）
	FVector Direction =
		TargetPlayerCharacter->GetActorLocation() - GetActorLocation();

	Direction.Z = 0.f;
	Direction.Normalize();

	

	AddMovementInput(Direction, 1.0f);
}

// Called when the game starts or when spawned
void ALandEnemy::BeginPlay()
{
	Super::BeginPlay();
	TargetPlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	if (DamageBox)
	{
		DamageBox->OnComponentBeginOverlap.AddDynamic(
			this, &ALandEnemy::OnDamageBoxBeginOverlap);
	}
}

void ALandEnemy::OnDamageBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, 
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player) return;   // 敌人自己直接被排除

	// 传送玩家到出生点
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(
		GetWorld(), APlayerStart::StaticClass());

	if (PlayerStart)
	{
		Player->SetActorLocation(
			PlayerStart->GetActorLocation(),
			false, nullptr, ETeleportType::TeleportPhysics);
	}
}

// Called every frame
void ALandEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bCanSeePlayer = LineTraceActor(TargetPlayerCharacter);

	if (bCanSeePlayer)
	{
		MoveToPlayer(DeltaTime);
	}
}

bool ALandEnemy::LineTraceActor(const AActor* TargetActor)
{
	if (TargetActor == nullptr)
	{
		return false;
	}
	FVector Start = GetActorLocation();
	FVector End = TargetActor->GetActorLocation();

	// 看见玩家会朝向玩家
	if (CanSeeActor(TargetActor, Start, End))
	{
		FRotator DirectRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		SetActorRotation(DirectRotation);
		return true;

	}


	return false;
}

bool ALandEnemy::CanSeeActor(const AActor* TargetActor, FVector Start, FVector End) const
{
	if (TargetActor == nullptr)
	{
		return false;
	}
	FHitResult Hit;
	ECollisionChannel Channel = ECC_Visibility;

	// 忽略玩家和敌人
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(TargetActor);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel, QueryParams);

	return !Hit.bBlockingHit;
}

// Called to bind functionality to input
void ALandEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

