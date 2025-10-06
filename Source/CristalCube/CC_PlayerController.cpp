// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/CC_PlayerCharacter.h"


ACC_PlayerController::ACC_PlayerController()
{
    bShowMouseCursor = true; 
    bEnableClickEvents = true;
    bEnableMouseOverEvents = false;

    PrimaryActorTick.bCanEverTick = true;

    CC_LOG_PLAYER(Warning, "PlayerController initialized");
}

void ACC_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
            CC_LOG_PLAYER(Log, "Input mapping context added");
        }
        else
        {
            CC_LOG_PLAYER(Warning, "DefaultMappingContext is null!");
        }
    }

    ControlledCharacter = Cast<ACC_PlayerCharacter>(GetPawn());
    if (ControlledCharacter)
    {
        CC_LOG_PLAYER(Log, "PlayerCharacter reference cached: %s", CC_ACTOR_NAME(ControlledCharacter));
    }
    else
    {
        CC_LOG_PLAYER(Warning, "Failed to cast pawn to CC_PlayerCharacter");
    }
}

void ACC_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
		CC_LOG_PLAYER(Warning, "Enhanced Input Component found");

        // Movement
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACC_PlayerController::HandleMove);
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACC_PlayerController::HandleMove);

			CC_LOG_PLAYER(Warning, "Move action bound successfully");
        }

        // Primary Attack
        if (PrimaryAttackAction)
        {
            EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &ACC_PlayerController::HandlePrimaryAttack);
            EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Completed, this, &ACC_PlayerController::HandlePrimaryAttack);
        }

        // Secondary Attack
        if (SecondaryAttackAction)
        {
            EnhancedInputComponent->BindAction(SecondaryAttackAction, ETriggerEvent::Started, this, &ACC_PlayerController::HandleSecondaryAttack);
            EnhancedInputComponent->BindAction(SecondaryAttackAction, ETriggerEvent::Completed, this, &ACC_PlayerController::HandleSecondaryAttack);
        }

        // Dash
        if (DashAction)
        {
            EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ACC_PlayerController::HandleDash);
        }

        CC_LOG_PLAYER(Log, "Enhanced Input actions bound successfully");
    }
    else
    {
        CC_LOG_PLAYER(Error, "Failed to cast InputComponent to EnhancedInputComponent");
    }
}

void ACC_PlayerController::HandleMove(const FInputActionValue& Value)
{
  
	CurrentMoveInput = Value.Get<FVector2D>();


    if (ControlledCharacter && !CurrentMoveInput.IsZero())
    {
		const FVector ForwardDirection = FVector(0.0f, 1.0f, 0.0f);
		const FVector RightDirection = FVector(1.0f, 0.0f, 0.0f);

        ControlledCharacter->AddMovementInput(ForwardDirection, CurrentMoveInput.Y);  // W/S
        ControlledCharacter->AddMovementInput(RightDirection, CurrentMoveInput.X);    // A/D

        CC_LOG_PLAYER(VeryVerbose, "Movement input: X=%.2f Y=%.2f",
            CurrentMoveInput.X, CurrentMoveInput.Y);
    }
}

void ACC_PlayerController::HandlePrimaryAttack(const FInputActionValue& Value)
{
    bool bIsPressed = Value.Get<bool>();
    bIsPrimaryAttacking = bIsPressed;

    if (ControlledCharacter)
    {
        if (bIsPressed)
        {
            ControlledCharacter->PerformAttack();
            CC_LOG_PLAYER(VeryVerbose, "Primary attack triggered");
        }
    }
}

void ACC_PlayerController::HandleSecondaryAttack(const FInputActionValue& Value)
{
    bool bIsPressed = Value.Get<bool>();
    bIsSecondaryAttacking = bIsPressed;

    if (ControlledCharacter && bIsPressed)
    {
        CC_LOG_PLAYER(VeryVerbose, "Secondary attack triggered");
    }
}

void ACC_PlayerController::HandleDash(const FInputActionValue& Value)
{
    if (ControlledCharacter)
    {
        CC_LOG_PLAYER(Log, "Dash triggered (not implemented yet)");
    }
}

FVector ACC_PlayerController::GetMouseWorldPosition() const
{
    FVector WorldLocation, WorldDirection;

    if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        FVector CharacterLocation = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
        FPlane GroundPlane(CharacterLocation, FVector::UpVector);

        FVector MouseWorldPos = FMath::LinePlaneIntersection(
            WorldLocation,
            WorldLocation + WorldDirection * 10000.0f,
            GroundPlane
        );

        return MouseWorldPos;
    }

    return GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
}

void ACC_PlayerController::UpdateCharacterRotation(float DeltaTime)
{
	if (!ControlledCharacter) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastMouseUpdateTime < MouseUpdateInterval) return; // Limit to 50 FPS
    LastMouseUpdateTime = CurrentTime;
	
    FVector MouseWorldPos = GetMouseWorldPosition();
	FVector CharacterLocation = ControlledCharacter->GetActorLocation();

    FVector DirectionToMouse = MouseWorldPos - CharacterLocation;
    DirectionToMouse.Z = 0.0f; // Ignore vertical difference

    float DistanceToMouse = DirectionToMouse.Size();
	if (DistanceToMouse < MinMouseDistance) return; // Ignore if too close

    FRotator TargetRotation = DirectionToMouse.Rotation();

    FRotator CurrentRotation = ControlledCharacter->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, MouseRotationSpeed);

    ControlledCharacter->SetActorRotation(NewRotation);

    LastMouseWorldPosition = MouseWorldPos;
}

FVector ACC_PlayerController::GetMouseDirection() const
{
    if (ControlledCharacter)
    {
        FVector MousePos = GetMouseWorldPosition();
        FVector CharacterPos = ControlledCharacter->GetActorLocation();
        FVector Direction = MousePos - CharacterPos;
        Direction.Z = 0.0f;
        return Direction.GetSafeNormal();
    }

    return FVector::ForwardVector;
}

void ACC_PlayerController::SetMouseRotationSpeed(float NewSpeed)
{
      MouseRotationSpeed = FMath::Clamp(NewSpeed, 1.0f, 30.0f);
	  CC_LOG_PLAYER(Log, "Mouse rotation speed set to: %.2f", MouseRotationSpeed);
}

void ACC_PlayerController::SetMinMouseDistance(float NewDistance)
{
    MinMouseDistance = FMath::Clamp(NewDistance, 10.0f, 200.0f);
	CC_LOG_PLAYER(Log, "Min mouse distance set to: %.2f", MinMouseDistance);
}
