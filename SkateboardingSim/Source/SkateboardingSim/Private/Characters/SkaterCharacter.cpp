// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SkaterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASkaterCharacter::ASkaterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = MaxSkateSpeed;
	DefaultBrakingDeceleration = GetCharacterMovement()->BrakingDecelerationWalking;

	EnhancedBrakingDeceleration = DefaultBrakingDeceleration*20;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(GetRootComponent());

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("View Camera"));
	ViewCamera->SetupAttachment(CameraBoom);
	
	Skateboard = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skateboard Mesh"));
	Skateboard->SetupAttachment(GetMesh());
}

void ASkaterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkaterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentSpeed = GetVelocity().Size2D();
	FVector Forward = GetActorForwardVector();
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (bIsPushing && MovementComponent && CurrentSpeed > 0 && CurrentSpeed < MovementComponent->MaxWalkSpeed)
	{
		AddMovementInput(Forward, PushAcceleration * DeltaTime);
	}

	if (bIsBraking && CurrentSpeed > 0.f && !MovementComponent->IsFalling())
    {
		MovementComponent->BrakingDecelerationWalking = EnhancedBrakingDeceleration;
    }
	else
	{
		MovementComponent->BrakingDecelerationWalking = DefaultBrakingDeceleration;
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
	PlayerInputComponent->BindAction(FName("Push"), EInputEvent::IE_Pressed, this, &ASkaterCharacter::StartPush);
	PlayerInputComponent->BindAction(FName("Push"), EInputEvent::IE_Released, this, &ASkaterCharacter::StopPush);
	PlayerInputComponent->BindAction(FName("Brake"), EInputEvent::IE_Pressed, this, &ASkaterCharacter::StartBrake);
	PlayerInputComponent->BindAction(FName("Brake"), EInputEvent::IE_Released, this, &ASkaterCharacter::StopBrake);
}

void ASkaterCharacter::MoveForward(float Value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsInputKeyDown(EKeys::S))
	{
		Value = 0.f;
	}
	MoveForwardValue = UKismetMathLibrary::Lerp(MoveForwardValue, Value, 0.01f);
	AddMovementInput(GetActorForwardVector(),MoveForwardValue);
}

void ASkaterCharacter::MoveRight(float Value)
{

	APlayerController* PC = Cast<APlayerController>(GetController());
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
	Jump();
}


void ASkaterCharacter::StartPush()
{
	bIsPushing = true;
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
