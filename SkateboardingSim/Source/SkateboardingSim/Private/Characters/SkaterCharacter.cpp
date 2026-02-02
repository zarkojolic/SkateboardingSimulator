// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SkaterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASkaterCharacter::ASkaterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 200.f, 0.f);

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

}

void ASkaterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASkaterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASkaterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASkaterCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASkaterCharacter::Turn);

}

void ASkaterCharacter::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASkaterCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
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