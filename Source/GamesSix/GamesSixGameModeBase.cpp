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

	// Get player controller ref
	PlayerController = Cast<AThirdPersonPlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerPawn = PlayerController->GetPawn();

	// Spawn room manager
	RoomManager = GetWorld()->SpawnActor<ARoomManager>(RoomManagerClass, FTransform{});	
}

void AGamesSixGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerCharacter) return;
	if (PlayerCharacter->LevelWon)
	{
		// Make a new dungeon
		RoomManager->Destroy();
		RoomManager = GetWorld()->SpawnActor<ARoomManager>(RoomManagerClass, FTransform{});

		// Put character at the start
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
