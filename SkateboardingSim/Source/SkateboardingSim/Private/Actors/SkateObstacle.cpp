// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SkateObstacle.h"
#include "Characters/SkaterCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASkateObstacle::ASkateObstacle()
{
	PrimaryActorTick.bCanEverTick = false;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
}

void ASkateObstacle::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASkaterCharacter* Skater = Cast<ASkaterCharacter>(OtherActor);
		if (Skater && Skater->GetCharacterMovement()->IsFalling())
		{
			Skater->AddScore(ScoreValue);
		}
	}
}


void ASkateObstacle::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASkateObstacle::OnBoxOverlap);
}

void ASkateObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

