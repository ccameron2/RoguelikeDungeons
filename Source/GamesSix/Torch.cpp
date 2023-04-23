// Fill out your copyright notice in the Description page of Project Settings.


#include "Torch.h"

// Sets default values
ATorch::ATorch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TorchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Torch Light"));
	SetRootComponent(TorchMesh);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));

	PointLight->LightColor = FColor{ 253,138,20 };
	PointLight->Intensity = 15000.0f;
	PointLight->AttenuationRadius = 400.0f;
	PointLight->SetupAttachment(TorchMesh);
	PointLight->SetVisibility(false);
}

// Called when the game starts or when spawned
void ATorch::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

}

// Called every frame
void ATorch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto playerCharacter = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (playerCharacter)
	{
		auto distance = FVector::Distance(GetActorLocation(), playerCharacter->GetActorLocation());
		if (distance > LightDisableDistance) PointLight->SetVisibility(false);
		else PointLight->SetVisibility(true);
	}
}
