// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableChest.h"

// Sets default values
AInteractableChest::AInteractableChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset1(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Chest_Gold_Chest_Base'"));
	BottomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bottom Mesh"));
	BottomMesh->SetStaticMesh(meshAsset1.Object);
	SetRootComponent(BottomMesh);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset2(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Chest_Gold_Chest_Top'"));
	OpenTopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Open Top Mesh"));
	OpenTopMesh->SetStaticMesh(meshAsset2.Object);
	OpenTopMesh->SetupAttachment(BottomMesh);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset3(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Chest_Chest_Top'"));
	ClosedTopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Closed Top Mesh"));
	ClosedTopMesh->SetStaticMesh(meshAsset3.Object);
	ClosedTopMesh->SetupAttachment(BottomMesh);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(BottomMesh);
	SphereCollision->SetSphereRadius(75.0f);
	SphereCollision->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AInteractableChest::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteractableChest::OnOverlapBegin);

}

void AInteractableChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		AThirdPersonCharacter* character = Cast<AThirdPersonCharacter>(OtherActor);
		if (character)
		{
			if (character->IsAttacking)
			{
				FString enemyName = character->GetName();
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Actor name: %s"), *enemyName));
				isOpen = true;
			}			
		}
	}
}

// Called every frame
void AInteractableChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isOpen)
	{
		ClosedTopMesh->SetVisibility(false);
		OpenTopMesh->SetVisibility(true);
	}
}

