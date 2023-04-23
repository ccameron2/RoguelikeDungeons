// Fill out your copyright notice in the Description page of Project Settings.


#include "Campfire.h"

// Sets default values
ACampfire::ACampfire()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/WoodFire_WoodFire'"));
	CampfireMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Campfire Mesh"));
	CampfireMesh->SetStaticMesh(meshAsset.Object);
	SetRootComponent(CampfireMesh);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));

	PointLight->LightColor = FColor{ 253,138,20 };
	PointLight->Intensity = 15000.0f;
	PointLight->AttenuationRadius = 400.0f;
	PointLight->SetupAttachment(CampfireMesh);
	PointLight->SetVisibility(false);

}

// Called when the game starts or when spawned
void ACampfire::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACampfire::Tick(float DeltaTime)
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

