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

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset4(TEXT("StaticMesh'/Game/Models/RPGItems/Sword'"));
	AttackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attack Mesh"));
	AttackMesh->SetStaticMesh(meshAsset4.Object);
	AttackMesh->SetupAttachment(BottomMesh);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset5(TEXT("StaticMesh'/Game/Models/RPGItems/Crystal3'"));
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

	// Collision delegate
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteractableChest::OnOverlapBegin);
	
	// Make pickup meshes invisible
	HealthMesh->SetVisibility(false);
	AttackMesh->SetVisibility(false);
	LevelMesh->SetVisibility(false);
}

void AInteractableChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		PlayerCharacter = Cast<AThirdPersonCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			if (PlayerCharacter->IsAttacking)
			{
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
		// Open chest
		ClosedTopMesh->SetVisibility(false);
		OpenTopMesh->SetVisibility(true);

		if (!PickedUp)
		{
			PlayerCharacter = Cast<AThirdPersonCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

			// Spawn random pickup and apply effect
			auto random = FMath::RandRange(0, 2);
			switch (random)
			{
			case 0:
				HealthMesh->SetVisibility(true);
				ActiveMesh = HealthMesh;
				PlayerCharacter->HealthPoints = PlayerCharacter->MaxHealth;
				break;
			case 1:
				AttackMesh->SetVisibility(true);
				ActiveMesh = AttackMesh;
				PlayerCharacter->Damage += 5.0f;
				break;
			case 2:
				LevelMesh->SetVisibility(true);
				ActiveMesh = LevelMesh;
				PlayerCharacter->LevelUp();
				break;
			default:
				break;
			}
			
			// Start pickup lifetime timer
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AInteractableChest::ClearPickup, PickupLifetime, false);
			PickedUp = true;
		}
	}
}

void AInteractableChest::ClearPickup()
{
	ActiveMesh->SetVisibility(false);
	ActiveMesh = nullptr;
}

