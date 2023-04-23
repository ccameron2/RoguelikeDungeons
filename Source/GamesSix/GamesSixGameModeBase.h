// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ResourcePickup.h"
#include "RoomManager.h"
#include "ThirdPersonCharacter.h"
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
protected:
	virtual void StartPlay() override;
private:
	UPROPERTY(EditAnywhere)
		TSubclassOf<AResourcePickup> HealthClass;
	
	UPROPERTY(EditAnywhere)
		TSubclassOf<ARoomManager> RoomManagerClass;
};
