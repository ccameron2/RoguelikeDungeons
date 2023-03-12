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
	PointLight->Intensity = 500.0f;
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

	auto distance = FVector::Distance(GetActorLocation(), GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
	if (distance > 3500) PointLight->SetVisibility(false);
	else PointLight->SetVisibility(true);

	//FHitResult HitResult;
	//FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(this); 
	//CollisionParams.AddIgnoredActor(GetOwner());
	//
	//bool bHit = GetWorld()->LineTraceSingleByChannel(
	//	HitResult,
	//	GetActorLocation(),
	//	PlayerPawn->GetActorLocation(),
	//	ECC_Visibility, // channel used to check for visibility
	//	CollisionParams
	//);

	//if (HitResult.GetActor())
	//{
	//	FString s = HitResult.GetActor()->GetName();
	//	UE_LOG(LogTemp, Display, TEXT("Actor name: %s"), *s);
	//}

	////DrawDebugLine(GetWorld(), GetActorLocation(), PlayerPawn->GetActorLocation(), FColor{ 255,0,255 }, false, 1.0f, 0, 10.0f);

	//if (bHit && HitResult.GetActor() == PlayerPawn)
	//{
	//	PointLight->SetVisibility(true);
	//}
	//else
	//{
	//	PointLight->SetVisibility(false);
	//}

}

