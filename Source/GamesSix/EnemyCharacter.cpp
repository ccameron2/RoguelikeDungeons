// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create attack overlap sphere
	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	AttackSphere->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Set health to max
	HealthPoints = MaxHealth;

	// Set dynamic delegates
	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnOverlapBegin);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnOverlapEnd);
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If health is depleted
	if (HealthPoints <= 0)
	{
		if (Dead) return;

		// Set death timer
		GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &AEnemyCharacter::DeathComplete, 1.0f, false);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());

		Dead = true;
	}
	
	auto playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	auto distance = 0.0f;
	if (playerPawn) 
	{
		// Get distance to player
		distance = FVector::Distance(GetActorLocation(), playerPawn->GetActorLocation());

		// Raycast to player and check if blocked
		FVector rayStart = GetActorLocation();
		FVector rayEnd = playerPawn->GetActorLocation();
		const FCollisionQueryParams RayParams = FCollisionQueryParams::DefaultQueryParam;
		FHitResult hitResult;

		const bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, rayStart, rayEnd, ECC_Visibility, RayParams);
		if (bHit)
		{
			if (hitResult.GetActor() == playerPawn)
			{
				PlayerVisibility = true;
			}
			else
			{
				PlayerVisibility = false;
			}
		}
	} 

	if (distance < AttackDistance) Attacking = true;
	else { Attacking = false; }

	OverlappingEnemy();

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AttackComplete()
{
	Attacking = false;
	OnCooldown = false;
	AttackTimerStarted = false;
}

void AEnemyCharacter::DeathComplete()
{
	// Drop resources on death
	FActorSpawnParameters spawnParams;
	FVector location;
	FTransform transform;
	FVector scale = FVector(0.08, 0.08, 0.08);

	for (int i = 0; i < NumResourcesDropped; i++)
	{		
		location.X = FMath::RandRange(0, 200);
		location.Y = FMath::RandRange(0, 200);
		location.Z = FMath::RandRange(0, 200);
		transform.SetScale3D(scale);
		transform.SetTranslation(GetActorLocation() + location);

		// Spawn resource and set type
		AResourcePickup* ResourcePickup = GetWorld()->SpawnActorDeferred<AResourcePickup>(ResourceClass, transform);
		ResourcePickup->Type = FMath::RandRange(0, ResourcePickup->MaterialList.Num() - 1);
		ResourcePickup->FinishSpawning(transform);
	}

	Destroy();
}

void AEnemyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor)
		{
			FString enemyName = OtherActor->GetName();
			auto playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			FString playerName;

			// If overlapped actor is player
			if(playerPawn) playerName = playerPawn->GetName();
			if (enemyName == playerName)
			{
				Attacking = true;
				IsOverlapping = true;
				CurrOverlappedEnemy = OtherActor;
			}
		}
	}
}

void AEnemyCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		IsOverlapping = false;
		CurrOverlappedEnemy = nullptr;
	}
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Take Damage"));

	// Get positions
	auto damageCauserPos = DamageCauser->GetActorLocation();
	auto pos = GetActorLocation();

	// Find direction and calculate move offset
	FVector relativePos = pos - damageCauserPos;
	float length = relativePos.Length();
	FVector direction = { relativePos / length };
	FVector moveOffset = { direction.X * KnockbackDistance,direction.Y * KnockbackDistance,0.0f };

	// Move the enemy away from the damage
	SetActorLocation(pos + moveOffset,true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, GetActorLocation());

	HealthPoints -= DamageAmount;

	return 0.0f;
}

void AEnemyCharacter::OverlappingEnemy()
{
	// Deal damage if enemy is overlapping and attacking
	if (IsOverlapping)
	{
		if (Attacking)
		{
			if (OnCooldown) return;

			FDamageEvent DamageEvent;
			if (CurrOverlappedEnemy)
			{
				CurrOverlappedEnemy->TakeDamage(AttackDamage, DamageEvent, GetController(), this);			
			}

			// Start attack cooldown
			if (!AttackTimerStarted)
			{
				GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::AttackComplete, 1.0f, false);
				AttackTimerStarted = true;
				OnCooldown = true;
			}
			
		}
	}
}