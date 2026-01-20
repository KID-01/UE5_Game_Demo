// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundBase.h"


#include "MyGameInstanceForMusic.generated.h"

/**
 * 
 */
UCLASS()
class UE53_GAME_DEMO_API UMyGameInstanceForMusic : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void OnStart() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* BackgroundMusic;

	UPROPERTY()
	UAudioComponent* BGMComponent;
	
};
