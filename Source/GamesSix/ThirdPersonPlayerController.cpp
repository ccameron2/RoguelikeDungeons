// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonPlayerController.h"

void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//Get pawn reference
	PlayerPawn = Cast<AThirdPersonCharacter>(GetPawn());
}

void AThirdPersonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	//Bind inputs to assigned functions
	InputComponent->BindAxis("Move Forwards", this, &AThirdPersonPlayerController::CallForward);
	InputComponent->BindAxis("Turn", this, &AThirdPersonPlayerController::CallTurn);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AThirdPersonPlayerController::CallAttack);
	InputComponent->BindAxis("Strafe", this, &AThirdPersonPlayerController::CallStrafe);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AThirdPersonPlayerController::CallJump);
	InputComponent->BindAxis("Look Up", this, &AThirdPersonPlayerController::CallLookUp);
	InputComponent->BindAction("Swap Camera", EInputEvent::IE_Pressed, this, &AThirdPersonPlayerController::CallSwapCamera);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AThirdPersonPlayerController::CallToggleSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AThirdPersonPlayerController::CallToggleSprint);

}

void AThirdPersonPlayerController::CallForward(float Value)
{
	if (PlayerPawn)
	{
		PlayerPawn->MoveForward(Value);
	}
}

void AThirdPersonPlayerController::CallTurn(float Value)
{
	if (PlayerPawn)
	{
		PlayerPawn->Turn(Value);
	}
}


void AThirdPersonPlayerController::CallAttack()
{
	if (PlayerPawn)
	{
		PlayerPawn->Attack();
	}
}

void AThirdPersonPlayerController::CallStrafe(float Value)
{
	if (PlayerPawn)
	{
		PlayerPawn->Strafe(Value);
	}
}

void AThirdPersonPlayerController::CallJump()
{
	if (PlayerPawn)
	{
		PlayerPawn->Jump();
	}
}

void AThirdPersonPlayerController::CallLookUp(float Value)
{
	if (PlayerPawn)
	{
		PlayerPawn->LookUp(Value);
	}
}

void AThirdPersonPlayerController::CallSwapCamera()
{
	if (PlayerPawn)
	{
		PlayerPawn->SwapCamera();
	}
}

void AThirdPersonPlayerController::CallToggleSprint()
{
	if (PlayerPawn)
	{
		PlayerPawn->ToggleSprint();
	}
}
