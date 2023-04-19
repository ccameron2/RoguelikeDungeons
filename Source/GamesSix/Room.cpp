// Fill out your copyright notice in the Description page of Project Settings.
#include "Room.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Room Mesh"));
	ProcMesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> material(TEXT("M_Material'/Game/Materials/M_Material.M_Material'"));
	Material = material.Object;

	LoadMeshes();
}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
}
 
void ARoom::OnConstruction(const FTransform& Transform)
{

}

void ARoom::Destroyed()
{
	Super::Destroyed();
	if (!Torches.IsEmpty())
	{
		for (auto torch : Torches)
		{
			torch->Destroy();
		}
		Torches.Empty();
	}
	if (PillarMesh->GetInstanceCount() > 0) PillarMesh->ClearInstances();
	
	if (ChestMesh->GetInstanceCount() > 0) ChestMesh->ClearInstances();
}
// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoom::GenerateMesh(FastNoise* noise)
{

	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	VertexColours.Empty();
	Tangents.Empty();
	ProcMesh->ClearAllMeshSections();

	//SizeX = FMath::RandRange(5, 25);
	//SizeY = FMath::RandRange(5, 25);

	CreateFloor(noise);
	CreateTop(noise);
	PlacePillars();
}

void ARoom::CreateFloor(FastNoise* noise)
{
	Vertices.Init({ 0,0,0 }, SizeX * SizeY);

	// Create a grid of vertices
	int indexX = 0;
	for (int i = 0; i < SizeX; i++)
	{
		int indexY = 0;
		for (int j = 0; j < SizeY; j++)
		{
			FVector newVector = FVector{ float(i * Scale), float(j * Scale),0 };
			Vertices[SizeY * indexX + indexY] = newVector;
			indexY++;
		}
		indexX++;
	}

	// Generate triangles for grid of vertices
	GenerateTriangles(SizeX, SizeY, Triangles);

	for (auto& vertex : Vertices)
	{
		vertex.X -= (SizeX * Scale) / 2;
		vertex.Y -= (SizeY * Scale) / 2;
	}

	// For each vertex, get 2 different noise values and apply them to vertex hight at different scales.
	for (int i = 0; i < Vertices.Num(); i++)
	{
		// Get input vector from vertex list and sample noise at different levels
		auto input = Vertices[i] + GetActorLocation();
		auto result1 = noise->GetNoise(input.X / 0.9, input.Y / 0.9, input.Z / 0.9);
		Vertices[i].Z += result1 * 8;
		auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);
		Vertices[i].Z += result2 * 2;
	}

	Normals.Init({ 0,0,0 }, Vertices.Num());

	// Faster normals
	CalculateNormals(Normals,Vertices,Triangles);

	// Create mesh section
	ProcMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColours, Tangents, true);
	ProcMesh->SetMaterial(0, Material);
}

void ARoom::CreateTop(FastNoise* noise)
{
	TopVertices.Init({ 0,0,0 }, SizeX * SizeY);

	// Create a grid of vertices
	int indexX = 0;
	for (int i = 0; i < SizeX; i++)
	{
		int indexY = 0;
		for (int j = 0; j < SizeY; j++)
		{
			FVector newVector = FVector{ float(i * Scale), float(j * Scale),0 };
			TopVertices[SizeY * indexX + indexY] = newVector;
			indexY++;
		}
		indexX++;
	}

	// Generate triangles for grid of vertices
	UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeX, SizeY, true, TopTriangles);

	for (auto& vertex : TopVertices)
	{
		vertex.X -= (SizeX * Scale) / 2;
		vertex.Y -= (SizeY * Scale) / 2;
		vertex.Z += ((WallSizeZ * Scale) / 2);
	}

	// For each vertex, get 2 different noise values and apply them to vertex hight at different scales.
	for (int i = 0; i < TopVertices.Num(); i++)
	{
		// Get input vector from vertex list and sample noise at different levels
		auto input = TopVertices[i] + GetActorLocation();
		auto result1 = noise->GetNoise(input.X / 0.9, input.Y / 0.9, input.Z / 0.9);
		TopVertices[i].Z += result1 * 8;
		auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);
		TopVertices[i].Z += result2 * 2;
	}

	TopNormals.Init({ 0,0,0 }, TopVertices.Num());

	// Faster normals
	CalculateNormals(TopNormals, TopVertices, TopTriangles);

	// Create mesh section
	ProcMesh->CreateMeshSection(1, TopVertices, TopTriangles, TopNormals, UVs, VertexColours, Tangents, true);
	ProcMesh->SetMaterial(1, Material);
}

void ARoom::PlacePillars()
{
	for (int i = 0; i < 4; i++)
	{
		FVector vertex = FVector{((-SizeX * Scale) / 2), ((-SizeY * Scale) / 2), 0};
		if (i == 0) vertex += FVector{ (SizeX * Scale) - Scale,0,0 };
		if (i == 1) vertex += FVector{ (SizeX * Scale) - Scale,(SizeY * Scale) - Scale,0 };
		if (i == 2) vertex += FVector{ 0,(SizeY * Scale)-Scale,0 };
		if (i == 3) vertex += FVector{ 0,0,0 };

		// Set location to vertex position and scale randomly
		FTransform transform;
		transform.SetLocation(vertex + GetActorLocation());
		FQuat Rotation = FVector{ 0,0,0 }.ToOrientationQuat();
		transform.SetRotation(Rotation);
		//transform.SetScale3D(FVector{ float(FMath::RandRange(0.8,1.2)) });

		FVector Start = FVector{ transform.GetLocation().X,transform.GetLocation().Y,float(WallSizeZ * Scale)};
		FVector End = transform.GetLocation();
		
		// Setup parameters for raycast
		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = false;

		// Perform raycast and store hit result
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, TraceParams);

		// If raycast hit something
		if (bHit)
		{
			auto actor = HitResult.GetComponent();
			if (actor->GetName() != "Pillar Static Mesh")
			{
				PillarMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
				if (PillarMesh) { PillarMesh->AddInstance(transform); }
			}
		}
		//DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor(125, 18, 255), true, -1, 0, 20);
	}
}

void ARoom::PlaceCobwebs()
{
	for (int i = 0; i < 4; i++)
	{
		FRotator rotation = FRotator{ 0,0,0 };
		FVector vertex = FVector{ ((-SizeX * Scale) / 2), ((-SizeY * Scale) / 2), 0 };	
		if (i == 0) vertex += FVector{ (SizeX * Scale) - 2 * Scale,0,0 };
		if (i == 1) vertex += FVector{ (SizeX * Scale) - 4 * Scale,(SizeY * Scale) - 4 * Scale,0 };
		if (i == 2) vertex += FVector{ 0,(SizeY * Scale) - 4 * Scale,0 };
		if (i == 3) vertex += FVector{ 2.0f * Scale, 0, 0 };
		// Set location to vertex position and scale randomly
		FTransform transform;
		transform.SetLocation(vertex + GetActorLocation());
		FQuat rotationQuat = rotation.Quaternion();
		transform.SetRotation(rotationQuat);
		CobwebMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		if (CobwebMesh) { CobwebMesh->AddInstance(transform); }
	}
}

void ARoom::PlaceObjects()
{
	if(NumWalls >= 3)
	{
		//PlaceCobwebs();

		int roomNum = FMath::RandRange(0, 6);
		if (roomNum == 1)
		{
			FVector rotation = FVector{ 0,0,0 };
			FVector vertex = FVector{ 0,0,0 };

			GetUnusedNSVertex(vertex);

			// Set location to vertex position and scale randomly
			FTransform transform;
			transform.SetLocation(vertex + GetActorLocation());
			FQuat rotationQuat = rotation.ToOrientationQuat();
			transform.SetRotation(rotationQuat);

			ChestMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			if (ChestMesh) { ChestMesh->AddInstance(transform); }
		}
		else if(roomNum == 2)
		{
			// Room 1
			FVector vertex = FVector{ 0,0,0 };

			// Set location to vertex position and scale randomly
			FTransform transform;
			transform.SetLocation(vertex + GetActorLocation());
			FQuat Rotation = FVector{ 0,0,0 }.ToOrientationQuat();
			transform.SetRotation(Rotation);

			if (Campfires.IsEmpty())
			{
				auto campfire = GetWorld()->SpawnActor<ACampfire>(CampfireClass, transform);
				campfire->SetOwner(this);
				Campfires.Push(campfire);
			}
		}
		else if (roomNum == 3)
		{
			// Room 1
			FVector vertex = FVector{ 0,0,0 };

			auto direction = GetUnusedNSVertex(vertex);

			// Set location to vertex position and scale randomly
			FTransform transform;
			transform.SetLocation(vertex + GetActorLocation());
			FQuat Rotation = FVector{ 0,90,0 }.ToOrientationQuat();
			transform.SetRotation(Rotation);

			TableMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			if (TableMesh) { TableMesh->AddInstance(transform); }

			if (direction == North) transform.SetLocation(vertex + FVector{ 0,4 * float(-Scale),0 } + GetActorLocation());
			else transform.SetLocation(vertex + FVector{ 0,4 * float(Scale),0 } + GetActorLocation());

			if (direction == North) transform.SetRotation(FRotator{ 0,0,0 }.Quaternion());
			else transform.SetRotation(FRotator{ 0,180,0 }.Quaternion());

			ChairMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			if (ChairMesh) { ChairMesh->AddInstance(transform); }

		}
		else if (roomNum == 4)
		{
			// Room 1
			FVector vertex = FVector{ 0,0,0 };

			auto direction = GetUnusedNSVertex(vertex);

			// Set location to vertex position and scale randomly
			FTransform transform;
			transform.SetLocation(vertex + GetActorLocation());
			FQuat Rotation = FVector{ 0,90,0 }.ToOrientationQuat();
			transform.SetRotation(Rotation);

			BigTableMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			if (BigTableMesh) { BigTableMesh->AddInstance(transform); }


			if(direction == North) transform.SetLocation(vertex + FVector{ 0,4 * float(-Scale),0 } + GetActorLocation());
			else transform.SetLocation(vertex + FVector{ 0,4 * float(Scale),0} + GetActorLocation());

			if (direction == North) transform.SetRotation(FRotator{ 0,0,0 }.Quaternion());
			else transform.SetRotation(FRotator{ 0,180,0 }.Quaternion());

			ChairMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			if (ChairMesh) { ChairMesh->AddInstance(transform); }
		}
		if (roomNum == 5)
		{
			FVector rotation = FVector{ 0,0,0 };
			FVector vertex = FVector{ 0,0,0 };

			GetUnusedNSVertex(vertex);

			// Set location to vertex position and scale randomly
			FTransform transform;
			transform.SetLocation(vertex + GetActorLocation());
			FQuat rotationQuat = rotation.ToOrientationQuat();
			transform.SetRotation(rotationQuat);

			BarrelMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			if (BarrelMesh) { BarrelMesh->AddInstance(transform); }
		}
		if (roomNum == 6)
		{
			FVector rotation = FVector{ 0,0,0 };
			FVector vertex = FVector{ 0,0,0 };

			GetUnusedNSVertex(vertex);

			// Set location to vertex position and scale randomly
			FTransform transform;
			transform.SetLocation(vertex + GetActorLocation());
			FQuat rotationQuat = rotation.ToOrientationQuat();
			transform.SetRotation(rotationQuat);

			HorseMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			if (HorseMesh) { HorseMesh->AddInstance(transform); }
		}

	}
}

void ARoom::LoadMeshes()
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset1(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Column2_Column2'"));
	PillarMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Pillar Static Mesh"));
	PillarMesh->SetStaticMesh(meshAsset1.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset2(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Chest_Gold_Chest_Base'"));
	ChestMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Chest Static Mesh"));
	ChestMesh->SetStaticMesh(meshAsset2.Object);	

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset4(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Table_Big_Table_Big'"));
	BigTableMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Table Big Static Mesh"));
	BigTableMesh->SetStaticMesh(meshAsset4.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset5(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Barrel'"));
	BarrelMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Barrel Static Mesh"));
	BarrelMesh->SetStaticMesh(meshAsset5.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset6(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Chair'"));
	ChairMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Chair Static Mesh"));
	ChairMesh->SetStaticMesh(meshAsset6.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset7(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Cobweb_Cobweb'"));
	CobwebMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cobweb Static Mesh"));
	CobwebMesh->SetStaticMesh(meshAsset7.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset8(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Table_Small_Table_Small'"));
	TableMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Table Small Static Mesh"));
	TableMesh->SetStaticMesh(meshAsset8.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset9(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Horse_Statue_Pedestal'"));
	HorsePedestalMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Horse Pedestal Static Mesh"));
	HorsePedestalMesh->SetStaticMesh(meshAsset9.Object);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset10(TEXT("StaticMesh'/Game/Models/LowPolyDungeon/Horse_Statue_Horse'"));
	HorseMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Horse Static Mesh"));
	HorseMesh->SetStaticMesh(meshAsset10.Object);
}

ARoom::Direction ARoom::GetUnusedNSVertex(FVector& vertex)
{
	for (int i = 0; i < 4; i++)
	{
		auto direction = static_cast<Direction>(i);
		if (!UsedDirections.Contains(direction))
		{
			switch (direction)
			{
			case North:
				vertex = FVector{ 0,((SizeY * Scale) / 2) - 3 * Scale,0 };
				return direction;
				break;
			case South:
				vertex = FVector{ 0,((-SizeY * Scale) / 2) + 3 * Scale,0 };
				return direction;
				break;
			default:
				return direction;
				break;
			}
		}
	}
	return static_cast<Direction>(0);
}

void ARoom::GenerateTriangles(int sizeX, int sizeY, TArray<int32>& triangles)
{
	const int NumTriangles = 2 * (sizeX - 1) * (sizeY - 1);
	triangles.SetNum(NumTriangles * 3);

	int TriangleIndex = 0;
	for (int i = 0; i < sizeX - 1; i++)
	{
		for (int j = 0; j < sizeY - 1; j++)
		{
			int VertexIndex = i * sizeY + j;

			// Create two triangles for each grid cell
			triangles[TriangleIndex++] = VertexIndex;
			triangles[TriangleIndex++] = VertexIndex + 1;
			triangles[TriangleIndex++] = VertexIndex + sizeY;
			
			triangles[TriangleIndex++] = VertexIndex + sizeY;
			triangles[TriangleIndex++] = VertexIndex + 1;
			triangles[TriangleIndex++] = VertexIndex + sizeY + 1;
		}
	}
}

void ARoom::GenerateRandomMesh()
{
	FastNoise noise;
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetSeed(FMath::Rand());
	GenerateMesh(&noise);
}

void ARoom::MakeWalls(FastNoise* noise)
{
	FTransform transform;
	int meshSectionIndex = 2;
	for (int i = 0; i < 4; i++)
	{
		auto direction = static_cast<Direction>(i);
		if (!UsedDirections.Contains(direction))
		{
			NumWalls++;
			FVector location = GetActorLocation();
			TArray<FVector> vertices;
			TArray<FVector> normals;
			TArray<int32> triangles;
			switch (direction)
			{
			case Direction::North:
				for (int j = 0; j < SizeX; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ float(j * Scale),0,float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X -= (SizeX * Scale) / 2;
					vertex.Y += (SizeY * Scale / 2) - Scale;
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeX, WallHeight, false, triangles);

				transform.SetLocation((GetActorLocation() + FVector{ 0,float((SizeY * Scale / 2) - 2 * Scale),float(WallSizeZ * Scale) / 2 - 4 * Scale }));
				transform.SetRotation(FVector{ 0,0,0 }.ToOrientationQuat());
				break;
			case Direction::South:
				for (int j = 0; j < SizeX; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ float(j * Scale),0,float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X -= (SizeX * Scale) / 2;
					vertex.Y -= (SizeY * Scale / 2);
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeX, WallHeight, true, triangles);
				transform.SetLocation(GetActorLocation() + FVector{ 0,float(-SizeY * Scale + Scale) / 2,float(WallSizeZ * Scale) / 2 - 4 * Scale });
				transform.SetRotation(FVector{ 0,0,0 }.ToOrientationQuat());
				break;
			case Direction::East:
				for (int j = 0; j < SizeY; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ 0,float(j * Scale),float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X += (SizeX * Scale / 2) - Scale;
					vertex.Y -= (SizeY * Scale / 2);
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeY, WallHeight, true, triangles);
				transform.SetLocation((GetActorLocation() + FVector{ float((SizeX * Scale / 2) - 2 * Scale),0,float(WallSizeZ * Scale) / 2 - 4 * Scale }));
				transform.SetRotation(FVector{ 0,0,0 }.ToOrientationQuat());
				break;
			case Direction::West:
				for (int j = 0; j < SizeY; j++)
				{
					for (int k = 0; k < WallHeight; k++)
					{
						vertices.Push(FVector{ 0,float(j * Scale),float(k * Scale) });
					}
				}
				for (auto& vertex : vertices)
				{
					vertex.X -= (SizeX * Scale) / 2;
					vertex.Y -= (SizeY * Scale / 2);
				}
				UKismetProceduralMeshLibrary::CreateGridMeshTriangles(SizeY, WallHeight, false, triangles);
				transform.SetLocation((GetActorLocation() + FVector{ float(( -SizeX * Scale  / 2) + 2 * Scale) ,0,float(WallSizeZ * Scale) / 2 - 4 * Scale}));
				transform.SetRotation(FVector{ 0,90,0 }.ToOrientationQuat());
				break;
			}

			// For each vertex, get 2 different noise values and apply them to vertex hight at different scales.
			for (auto& vertex : vertices)
			{
				// Get input vector from vertex list and sample noise at different levels
				auto input = vertex + GetActorLocation();
				auto result1 = noise->GetNoise(input.X / 0.9, input.Y / 0.9, input.Z / 0.9);
				auto result2 = noise->GetNoise(input.X / 0.5, input.Y / 0.5, input.Z / 0.5);

				if (vertex.Z == 0)
				{
					vertex.Z += result1 * 8;
					vertex.Z += result2 * 2;
				}
				else if (vertex.Z == WallHeight * Scale - Scale)
				{
					vertex.Z += result1 * 8;
					vertex.Z += result2 * 2;
				}			 
				else
				{
					vertex.Y += result1 * 15;
					vertex.Y += result2 * 5;
					vertex.X += result1 * 15;
					vertex.X += result2 * 5;							
				}
			}

			normals.Init({ 0,0,0 }, vertices.Num());
			CalculateNormals(normals, vertices, triangles);

			ProcMesh->CreateMeshSection(meshSectionIndex, vertices, triangles, normals, UVs, VertexColours, Tangents, true);
			ProcMesh->SetMaterial(meshSectionIndex, Material);

			// Place torches
			if(Torches.IsEmpty())
			{
				switch (direction)
				{
				case North:
					transform.SetRotation(FRotator{ 0,180,0 }.Quaternion());
					break;
				case East:
					transform.SetRotation(FRotator{ 0,90,0 }.Quaternion());
					break;
				case South:
					transform.SetRotation(FRotator{ 0,0,0 }.Quaternion());
					break;
				case West:
					transform.SetRotation(FRotator{ 0,270,0 }.Quaternion());
					break;
				}
				auto torch = GetWorld()->SpawnActor<ATorch>(TorchClass, transform);
				torch->SetOwner(this);
				Torches.Push(torch);
			}
		}
		meshSectionIndex++;
	}
}

void ARoom::CalculateNormals(TArray<FVector>& normals,TArray<FVector> vertices, TArray<int32> triangles)
{
	for (int i = 0; i < triangles.Num() - 3; i += 3)
	{
		auto a = vertices[triangles[i]];
		auto b = vertices[triangles[i + 1]];
		auto c = vertices[triangles[i + 2]];

		auto v1 = a - b;
		auto v2 = c - b;
		auto n = v1 ^ v2;
		n.Normalize();

		normals[triangles[i]] += n;
		normals[triangles[i + 1]] += n;
		normals[triangles[i + 2]] += n;
	}

	for (auto& normal : normals)
	{
		normal.Normalize();
	}
}
