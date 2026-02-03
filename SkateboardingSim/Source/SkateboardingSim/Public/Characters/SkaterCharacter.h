// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class USpringArmComponent;
class UCameraComponent;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkaterCharacter.generated.h"

UCLASS()
class SKATEBOARDINGSIM_API ASkaterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASkaterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void LookUp(float Value);
	void Turn(float Value);

	void SkateJump();
	
	void PushButtonPressed();
	void PushButtonReleased();
	void StartPush();
	void StopPush();

	void StartBrake();
	void StopBrake();

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	
	UPROPERTY(EditAnywhere, Category = Mesh) 
	USkeletalMeshComponent* Skateboard;
	
	UPROPERTY(VisibleAnywhere)
	APlayerController* PC;
	
	UPROPERTY(VisibleAnywhere, Category = "Skate Movement")
	float MoveForwardValue = 0.f;
	
	bool bIsPushing;
	bool bPushingButtonPressed;
	bool bIsBraking;

	float CurrentSpeedLastFrame = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Skate Movement")
	float PushAcceleration = 2.f;
	
	UPROPERTY(EditAnywhere, Category = "Skate Movement")
	float MaxSkateSpeed = 1000.f;

public:

	FORCEINLINE bool GetIsPushing() const { return bIsPushing; } 
};
