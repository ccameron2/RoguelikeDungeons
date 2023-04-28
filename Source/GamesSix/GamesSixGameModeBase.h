// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ResourcePickup.h"
#include "RoomManager.h"
#include "ThirdPersonCharacter.h"
#include "ThirdPersonPlayerController.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GamesSixGameModeBase.generated.h"


/**
 * 
 */
UCLASS()
class GAMESSIX_API AGamesSixGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGamesSixGameModeBase();
protected:
	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	
	// Dungeon manager
	UPROPERTY(EditAnywhere)
		TSubclassOf<ARoomManager> RoomManagerClass;

	// Witch player character
	UPROPERTY(EditAnywhere)
		TSubclassOf<AThirdPersonCharacter> WitchClass;

	// Rogue player character
	UPROPERTY(EditAnywhere)
		TSubclassOf<AThirdPersonCharacter> RogueClass;

	// Start game
	UFUNCTION(BlueprintCallable)
		void StartGame();

	// Player controller ref
	UPROPERTY()
		AThirdPersonPlayerController* PlayerController;

	// Player pawn ref
	UPROPERTY()
		APawn* PlayerPawn;

	// Player character ref
	UPROPERTY()
		AThirdPersonCharacter* PlayerCharacter;

	// Dungeon generator object
	UPROPERTY()
		ARoomManager* RoomManager;
public:
	// Character selection bool
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Rogue = true;

	// Is game started
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Started = false;

};
