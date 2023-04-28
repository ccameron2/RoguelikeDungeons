// Fill out your copyright notice in the Description page of Project Settings.
#include "Archway.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AArchway::AArchway()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArchwayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Archway Mesh"));
	ArchwayMesh->SetupAttachment(RootComponent);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
	PortalMesh->SetupAttachment(ArchwayMesh);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(PortalMesh);
	
}

// Called when the game starts or when spawned
void AArchway::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AArchway::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AArchway::OnOverlapEnd);
}

// Called every frame
void AArchway::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AArchway::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		FString enemyName = OtherActor->GetName();
		auto playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		FString playerName;
		if (playerPawn) playerName = playerPawn->GetName();
		if (enemyName == playerName)
		{
			auto playerChar = Cast<AThirdPersonCharacter>(playerPawn);
			playerChar->LevelWon = true;
		}
	}
}

void AArchway::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		
	}
}

