// Copyright Epic Games, Inc. All Rights Reserved.


#include "GamesSixGameModeBase.h"

AGamesSixGameModeBase::AGamesSixGameModeBase()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGamesSixGameModeBase::StartPlay()
{
	Super::StartPlay();

	PlayerController = Cast<AThirdPersonPlayerController>(GetWorld()->GetFirstPlayerController());
	
	RoomManager = GetWorld()->SpawnActor<ARoomManager>(RoomManagerClass, FTransform{});
	PlayerController = Cast<AThirdPersonPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerPawn = PlayerController->GetPawn();
}

void AGamesSixGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerCharacter) return;
	if (PlayerCharacter->LevelWon)
	{
		RoomManager->Destroy();
		RoomManager = GetWorld()->SpawnActor<ARoomManager>(RoomManagerClass, FTransform{});
		PlayerCharacter->SetActorLocation(FVector{ 0,0,125 });
		PlayerCharacter->LevelWon = false;
	}

}

void AGamesSixGameModeBase::StartGame()
{
	// Spawn character into the world
	FTransform Transform;
	const FVector Location = PlayerPawn->GetActorLocation();
	Transform.SetTranslation(Location);

	// Destroy old actor
	if (PlayerPawn) PlayerPawn->Destroy();

	if (Rogue) PlayerCharacter = GetWorld()->SpawnActor<AThirdPersonCharacter>(RogueClass, Transform);
	else PlayerCharacter = GetWorld()->SpawnActor<AThirdPersonCharacter>(WitchClass, Transform);

	PlayerPawn = PlayerCharacter;

	// Possess with custom player controller
	PlayerController->UnPossess();
	PlayerController->AutoReceiveInput = EAutoReceiveInput::Player0;
	PlayerController->Possess((PlayerCharacter));

	PlayerController->BindCharacter();

	Started = true;
}
