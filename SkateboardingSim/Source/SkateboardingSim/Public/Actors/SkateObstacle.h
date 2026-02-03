// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkateObstacle.generated.h"

UCLASS()
class SKATEBOARDINGSIM_API ASkateObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkateObstacle();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Score")
	int32 ScoreValue = 10;

	UFUNCTION()
	void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
