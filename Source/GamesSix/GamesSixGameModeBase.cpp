// Copyright Epic Games, Inc. All Rights Reserved.


#include "GamesSixGameModeBase.h"

void AGamesSixGameModeBase::StartPlay()
{
	Super::StartPlay();


	auto RoomManager = GetWorld()->SpawnActor<ARoomManager>(RoomManagerClass, FTransform{});

}
