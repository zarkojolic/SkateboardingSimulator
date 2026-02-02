// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/SkaterAnimInstance.h"
#include "Characters/SkaterCharacter.h"

void USkaterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<ASkaterCharacter>(TryGetPawnOwner());
    if (Character)
    {
        MovementComponent = Character->GetCharacterMovement();
    }
}

void USkaterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (MovementComponent)
    {
        bIsCharacterPushing = Character->GetIsPushing();
    }
}