// Fill out your copyright notice in the Description page of Project Settings.
#include "ThirdPersonCharacter.h"
#include "GameFramework/PawnMovementComponent.h" 
#include "Kismet/GameplayStatics.h"

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create spring arm and attach to the root component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	// Spring Arm settings
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	SpringArm->SetRelativeLocation(FVector(-361.0f, -305.0f, 113.0f));
	SpringArm->SetupAttachment(RootComponent);

	// Create third person camera and attach to SpringArm
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Third Person Camera"));
	ThirdPersonCamera->SetupAttachment(SpringArm);

	// Create first person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), "head");
	FirstPersonCamera->SetRelativeRotation(FRotator(0, 0, 0));
	FirstPersonCamera->SetRelativeLocation(FVector{ 0,10,0 });
	FirstPersonCamera->bUsePawnControlRotation = true;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AThirdPersonCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AThirdPersonCharacter::OnOverlapEnd);

	//Possess player 0
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	ThirdPersonCamera->SetActive(true);
	FirstPersonCamera->SetActive(false);


	FTransform transform;
	FVector location;
	location = GetActorLocation() - FVector{ -10,0,0 };

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TorchClass, Torches);

	//Torch = GetWorld()->SpawnActor<ATorch>(TorchClass, transform);
	//Torch->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, "Middle1_L");
	//Torch->SetActorEnableCollision(false);
}

// Called every frame
void AThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Energy < MaxEnergy){ Energy++; }
	if (ExpPoints >= MaxExp) { LevelUp(); }
	
	//if (Torches.Num() > 0)
	//{
	//	for (auto& torch : Torches)
	//	{
	//		//if (FVector::Distance(torch->GetActorLocation(), GetActorLocation()) < 2000)
	//		//{
	//		//	auto torchC = static_cast<ATorch*>(torch);
	//		//	torchC->PointLight->SetVisibility(true);
	//		//}
	//		FHitResult HitResult;
	//		FCollisionQueryParams CollisionParams;

	//		bool bHit = GetWorld()->LineTraceSingleByChannel(
	//			HitResult,
	//			ThirdPersonCamera->GetComponentLocation(),
	//			torch->GetActorLocation(),
	//			ECC_Visibility, // channel used to check for visibility
	//			CollisionParams
	//		);

	//		if (HitResult.GetComponent())
	//		{
	//			FString s = HitResult.GetComponent()->GetName();
	//			UE_LOG(LogTemp, Display, TEXT("Component name: %s"), *s);
	//		}

	//		//DrawDebugLine(GetWorld(), ThirdPersonCamera->GetComponentLocation(), torch->GetActorLocation(), FColor{ 255,0,255 }, false, 1.0f, 0, 10.0f);
	//		auto torchC = static_cast<ATorch*>(torch);
	//		if (bHit)
	//		{
	//			auto torchSphere = dynamic_cast<USphereComponent*>(HitResult.GetComponent());
	//			if (torchSphere == torchC->LightCollision)
	//			{
	//				torchC->PointLight->SetVisibility(true);
	//			}
	//			else
	//			{
	//				torchC->PointLight->SetVisibility(false);
	//			}
	//		}

	//	}
	//}
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AThirdPersonCharacter::OnConstruction(const FTransform& Transform)
{
	FirstPersonCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "head");
}

void AThirdPersonCharacter::MoveForward(float AxisValue)
{
	//Move character forwards
	if (Controller != nullptr && AxisValue != 0)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if(!Walking) AddMovementInput(Direction, AxisValue);
		else AddMovementInput(Direction, AxisValue / 4);
	}
}

void AThirdPersonCharacter::Strafe(float AxisValue)
{
	//Strafe character left and right
	if (Controller != nullptr && AxisValue != 0)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		if (!Walking) AddMovementInput(Direction, AxisValue);
		else AddMovementInput(Direction, AxisValue / 4);
	}
}

void AThirdPersonCharacter::Turn(float AxisValue)
{
	//Turn character left and right
	AddControllerYawInput(AxisValue);
}

void AThirdPersonCharacter::LookUp(float AxisValue)
{
	//Rotate view up and down
	AddControllerPitchInput(AxisValue);
}

void AThirdPersonCharacter::Attack()
{
	if (!IsAttacking)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AThirdPersonCharacter::EndAttack, AttackTime, false);
		IsAttacking = true;
	}
}

void AThirdPersonCharacter::Jump()
{
	Super::Jump();
	auto MovementComponent = GetMovementComponent();

	if (Energy > 50 && !MovementComponent->IsFalling())
	{
		Energy -= 75;
	}	
}

void AThirdPersonCharacter::SwapCamera()
{
	ThirdPersonCamera->ToggleActive();
	FirstPersonCamera->ToggleActive();
}

void AThirdPersonCharacter::ToggleSprint()
{
	Walking = !Walking;
}

void AThirdPersonCharacter::EndAttack()
{
	IsAttacking = false;
	AttackTimer.Invalidate();
}

void AThirdPersonCharacter::LevelUp()
{
	Level++; 
	ExpPoints = 0.0f; 
	MaxExp += 5;
	MaxHealth += 5;
	MaxMana += 5;
	MaxEnergy += 5;
}

float AThirdPersonCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Take Damage"));
	HealthPoints -= DamageAmount;
	return 0.0f;
}

void AThirdPersonCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap Begin"));
		AResourcePickup* resourcePickup = Cast<AResourcePickup>(OtherActor);
		if (resourcePickup)
		{
			if (resourcePickup->Type == 0) { if(HealthPoints < MaxHealth ){ HealthPoints++; }}
			if (resourcePickup->Type == 1) { if(Mana		 < MaxMana	 ){ Mana++; }}
			if (resourcePickup->Type == 2) { if(Energy		 < MaxEnergy ){ Energy++; }}
			if (resourcePickup->Type == 3) { if(ExpPoints	 < MaxExp	 ){ ExpPoints++; }}
			if (resourcePickup->Type == 4) { if(Gold		 < MaxGold	 ){ Gold++; }}

		}
		OtherActor->Destroy();
	}
}

void AThirdPersonCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap End"));
	}
}