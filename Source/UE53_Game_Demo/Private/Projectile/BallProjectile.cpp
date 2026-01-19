// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/BallProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Player/PlayerCharacter.h"


// Sets default values
ABallProjectile::ABallProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere Collision");
	SphereComponent->SetSphereRadius(35.f);
	SetRootComponent(SphereComponent);

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);

	SphereComponent->SetGenerateOverlapEvents(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	ProjectileMovement->InitialSpeed = 1300.f;

	ProjectileMovement->ProjectileGravityScale = 0.f;


	SphereComponent->SetNotifyRigidBodyCollision(true);
}

// Called when the game starts or when spawned
void ABallProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// 缓存玩家出生点（只做一次）
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(
		GetWorld(), APlayerStart::StaticClass());

	if (PlayerStart)
	{
		PlayerSpawnLocation = PlayerStart->GetActorLocation();
	}

	// 绑定 Overlap
	if (SphereComponent)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(
			this, &ABallProjectile::OnBallHit);
	}


	SetLifeSpan(2.f);
}


// Called every frame
void ABallProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallProjectile::OnBallHit(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if(!bDestroyHitActor && !bTeleportPlayer)
	{
		return;
	}

	// 防止打到自己
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Ball hit: %s"), *OtherActor->GetName());

	// 传送玩家
	if (bTeleportPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("=== OnBallHit Triggered ==="));


		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Player->SetActorLocation(
				PlayerSpawnLocation,
				false,
				nullptr,
				ETeleportType::TeleportPhysics
			);
		}
	}
	
	// 是否允许销毁目标
	if (bDestroyHitActor)
	{
		if (OtherActor->IsA<ACharacter>() || OtherActor->IsA(AActor::StaticClass()))
		{
			OtherActor->Destroy();
		}
	}

	// 子弹自己也销毁
	Destroy();
}

