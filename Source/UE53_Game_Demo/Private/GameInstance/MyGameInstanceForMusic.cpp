// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/MyGameInstanceForMusic.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void UMyGameInstanceForMusic::OnStart()
{
	Super::OnStart();
	if (BackgroundMusic)
	{
		BGMComponent = UGameplayStatics::CreateSound2D(
			this,
			BackgroundMusic,
			1.0f,
			1.0f,
			0.0f,
			nullptr,
			true   // Persist Across Level Transitions
		);

		if (BGMComponent)
		{
			BGMComponent->Play();
		}
	}

}
