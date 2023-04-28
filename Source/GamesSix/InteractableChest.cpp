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

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset4(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/RPGItems/Sword'"));
	AttackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attack Mesh"));
	AttackMesh->SetStaticMesh(meshAsset4.Object);
	AttackMesh->SetupAttachment(BottomMesh);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset5(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Chest_Chest_Top'"));
	LevelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Level Mesh"));
	LevelMesh->SetStaticMesh(meshAsset5.Object);
	LevelMesh->SetupAttachment(BottomMesh);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset6(TEXT("StaticMesh'/Game/Models/RPGItems/Heart'"));
	HealthMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Health Mesh"));
	HealthMesh->SetStaticMesh(meshAsset6.Object);
	HealthMesh->SetupAttachment(BottomMesh);

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
	HealthMesh->SetVisibility(false);
	AttackMesh->SetVisibility(false);
	LevelMesh->SetVisibility(false);
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
				IsOpen = true;
			}			
		}
	}
}

// Called every frame
void AInteractableChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsOpen)
	{
		ClosedTopMesh->SetVisibility(false);
		OpenTopMesh->SetVisibility(true);

		if (!PickedUp)
		{
			auto rand = FMath::RandRange(0, 2);
			switch (rand)
			{
			case 0:
				HealthMesh->SetVisibility(true);
				ActiveMesh = HealthMesh;
				break;
			case 1:
				AttackMesh->SetVisibility(true);
				ActiveMesh = AttackMesh;
				break;
			case 2:
				LevelMesh->SetVisibility(false);
				ActiveMesh = LevelMesh;
				break;
			default:
				break;
			}
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AInteractableChest::ClearPickup, PickupLifetime, false);
		}
	}
}

void AInteractableChest::ClearPickup()
{
	ActiveMesh->SetVisibility(false);
	ActiveMesh = nullptr;
}

