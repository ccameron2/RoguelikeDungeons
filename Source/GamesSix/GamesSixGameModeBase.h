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
	
	UPROPERTY(EditAnywhere)
		TSubclassOf<ARoomManager> RoomManagerClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AThirdPersonCharacter> WitchClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AThirdPersonCharacter> RogueClass;

	UFUNCTION(BlueprintCallable)
		void StartGame();

	UPROPERTY()
		AThirdPersonPlayerController* PlayerController;

	UPROPERTY()
		APawn* PlayerPawn;

	UPROPERTY()
		AThirdPersonCharacter* PlayerCharacter;

	UPROPERTY()
		ARoomManager* RoomManager;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Rogue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Started = false;

};
