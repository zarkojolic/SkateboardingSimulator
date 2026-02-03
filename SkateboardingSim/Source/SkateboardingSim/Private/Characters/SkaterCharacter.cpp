// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SkaterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"
#include "Blueprint/UserWidget.h"

ASkaterCharacter::ASkaterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//! Character Movement Component props adjusting
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = MaxSkateSpeed;

	//! Spring Arm Component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(GetRootComponent());

	//! View Camera attached to spring arm
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("View Camera"));
	ViewCamera->SetupAttachment(CameraBoom);
	
	//! Skateboard Skeletal Mesh
	Skateboard = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skateboard Mesh"));
	Skateboard->SetupAttachment(GetMesh());

}

void ASkaterCharacter::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetController());

	InitialSkateboardOffset = Skateboard->GetRelativeLocation();

	if (HUDWidgetClass)
	{
		UUserWidget* HUD = CreateWidget(GetWorld(), HUDWidgetClass);
		HUD->AddToViewport();
	}
	
}

void ASkaterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//! Current Speed on X and Y Axis and forward vector
	CurrentSpeed = GetVelocity().Size2D();
	// UE_LOG(LogTemp, Warning, TEXT("CurrentSpeed: %f || CurrentSpeedLastFrame: %f"), CurrentSpeed, CurrentSpeedLastFrame);
	FVector Forward = GetActorForwardVector();
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	//! Reset Speed to 0 when Player Hits a wall
	if (CurrentSpeedLastFrame - CurrentSpeed > 100.f)
	{
		MoveForwardValue = 0.f;
	}
	CurrentSpeedLastFrame = CurrentSpeed;

	//! Pushing Mechanic
	if (bIsPushing && MovementComponent && CurrentSpeed > 0 && CurrentSpeed < MovementComponent->MaxWalkSpeed)
	{
		AddMovementInput(Forward, PushAcceleration * DeltaTime);
	}

	//! Braking Mechanic
	if (bIsBraking && CurrentSpeed > 0.f && !MovementComponent->IsFalling())
    {
		if (MoveForwardValue > 0.f && CurrentSpeed > 30.f)
		{
			MoveForwardValue *= 0.97;
		}
		else if (CurrentSpeed <= 30.f)
		{
			MoveForwardValue = 0.f;
		}
    }

	//! Pushing Mechanic
	if (CurrentSpeed == MovementComponent->MaxWalkSpeed || MovementComponent->IsFalling() || !bPushingButtonPressed)
	{
		StopPush();
	}
	else if (bPushingButtonPressed)
	{
		StartPush();
	}
	
}

void ASkaterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASkaterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASkaterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASkaterCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASkaterCharacter::Turn);

	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &ASkaterCharacter::SkateJump);
	PlayerInputComponent->BindAction(FName("Push"), EInputEvent::IE_Pressed, this, &ASkaterCharacter::PushButtonPressed);
	PlayerInputComponent->BindAction(FName("Push"), EInputEvent::IE_Released, this, &ASkaterCharacter::PushButtonReleased);
	PlayerInputComponent->BindAction(FName("Brake"), EInputEvent::IE_Pressed, this, &ASkaterCharacter::StartBrake);
	PlayerInputComponent->BindAction(FName("Brake"), EInputEvent::IE_Released, this, &ASkaterCharacter::StopBrake);
}

void ASkaterCharacter::MoveForward(float Value)
{
	if (PC && PC->IsInputKeyDown(EKeys::S))
	{
		Value = 0.f;
	}
	MoveForwardValue = UKismetMathLibrary::Lerp(MoveForwardValue, Value, 0.001f);
	AddMovementInput(GetActorForwardVector(),MoveForwardValue);
}

void ASkaterCharacter::MoveRight(float Value)
{
	if (Value != 0.f && PC && PC->IsInputKeyDown(EKeys::W))
	{
		AddMovementInput(GetActorRightVector(),Value * 0.02f);
	}
}

void ASkaterCharacter::LookUp(float Value)
{
	if (Value != 0.f)
	{
		AddControllerPitchInput(Value);
	}
}

void ASkaterCharacter::Turn(float Value)
{
	if (Value != 0.f)
	{
		AddControllerYawInput(Value);
	}
}

void ASkaterCharacter::SkateJump()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent->IsFalling())
	{
		JumpElapsedTime = 0.f;

		GetWorldTimerManager().SetTimer(
			JumpOffsetTimerHandle,
			this,
			&ASkaterCharacter::UpdateJumpVisualOffset,
			0.016f,
			true
		);

		UAnimInstance* SkaterAnimInstance = GetMesh()->GetAnimInstance();
		if (JumpMontage && SkaterAnimInstance)
		{
			SkaterAnimInstance->Montage_Play(JumpMontage, 0.75f);
		}
		
		Jump();
	}
}

void ASkaterCharacter::UpdateJumpVisualOffset()
{
	if (!JumpOffsetCurve || !Skateboard)
	{
		StopJumpVisualOffset();
		return;
	}

	JumpElapsedTime += 0.016f;

	float NormalizedTime = JumpElapsedTime / 1.0f;

	if (NormalizedTime >= 0.8f)
	{
		StopJumpVisualOffset();
		return;
	}

	float CurveValue = JumpOffsetCurve->GetFloatValue(NormalizedTime);

	FVector NewOffset = InitialSkateboardOffset;
	NewOffset.Z += CurveValue * MaxJumpVisualOffset;

	Skateboard->SetRelativeLocation(NewOffset);
}

void ASkaterCharacter::StopJumpVisualOffset()
{
	JumpElapsedTime = 0.f;

	GetWorldTimerManager().ClearTimer(JumpOffsetTimerHandle);

	if (Skateboard)
	{
		Skateboard->SetRelativeLocation(InitialSkateboardOffset);
	}
}

void ASkaterCharacter::PushButtonPressed()
{
	bPushingButtonPressed = true;
}

void ASkaterCharacter::PushButtonReleased()
{
	bPushingButtonPressed = false;
}

void ASkaterCharacter::StartPush()
{
	if (PC && PC->IsInputKeyDown(EKeys::W))
	{
		bIsPushing = true;
	}
}

void ASkaterCharacter::StopPush()
{
	bIsPushing = false;
}

void ASkaterCharacter::StartBrake()
{
	bIsBraking = true;
}

void ASkaterCharacter::StopBrake()
{
	bIsBraking = false;
}
