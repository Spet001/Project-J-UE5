// FirstPersonRPGCharacter.cpp

#include "FirstPersonRPGCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

AFirstPersonRPGCharacter::AFirstPersonRPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Configurar cápsula de colisão
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configurar camera em primeira pessoa
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f)); // Altura dos olhos
	FirstPersonCamera->bUsePawnControlRotation = false; // Câmera segue o Pawn, não o Controller

	// Configurar movimento
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	
	// Desabilitar rotação automática do controller
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;  // Importante: desabilitar para Grid mode
	bUseControllerRotationRoll = false;
}

void AFirstPersonRPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Configurar Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// Inicializar posição no grid
	SnapToGrid();
	CurrentGridPosition = GetActorLocation();

	UE_LOG(LogTemp, Log, TEXT("FirstPersonRPGCharacter: Iniciado! Estilo de movimento: %s"), 
		MovementStyle == EMovementStyle::GridBased ? TEXT("Grid Based") : TEXT("Free Movement"));
}

void AFirstPersonRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movimento
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstPersonRPGCharacter::HandleMoveInput);
		}

		// Olhar ao redor
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstPersonRPGCharacter::HandleLookInput);
		}

		// Interagir
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFirstPersonRPGCharacter::HandleInteractInput);
		}

		// Menu
		if (MenuAction)
		{
			EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Started, this, &AFirstPersonRPGCharacter::HandleMenuInput);
		}
	}
}

void AFirstPersonRPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStyle == EMovementStyle::GridBased)
	{
		ProcessGridMovement(DeltaTime);
		ProcessGridRotation(DeltaTime);
	}
}

// ==================== INPUT HANDLERS ====================

void AFirstPersonRPGCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementStyle == EMovementStyle::GridBased)
	{
		// Modo Grid: Movimento discreto
		if (!bIsMovingOnGrid && !bIsRotatingOnGrid)
		{
			// W (frente) / S (trás)
			if (MovementVector.Y > 0.5f)
			{
				MoveInDirection(EGridDirection::North);
			}
			else if (MovementVector.Y < -0.5f)
			{
				MoveInDirection(EGridDirection::South);
			}
			// A (esquerda) / D (direita) - rotação
			else if (MovementVector.X < -0.5f)
			{
				TurnLeft();
			}
			else if (MovementVector.X > 0.5f)
			{
				TurnRight();
			}
		}
	}
	else
	{
		// Modo Livre: Movimento contínuo
		if (Controller)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void AFirstPersonRPGCharacter::HandleLookInput(const FInputActionValue& Value)
{
	// Só permite olhar livremente no modo de movimento livre
	if (MovementStyle == EMovementStyle::FreeMovement)
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		if (Controller)
		{
			AddControllerYawInput(LookAxisVector.X * LookSensitivity);
			AddControllerPitchInput(LookAxisVector.Y * LookSensitivity);
		}
	}
}

void AFirstPersonRPGCharacter::HandleInteractInput()
{
	Interact();
}

void AFirstPersonRPGCharacter::HandleMenuInput()
{
	OpenMenu();
}

// ==================== GRID MOVEMENT ====================

void AFirstPersonRPGCharacter::MoveInDirection(EGridDirection Direction)
{
	if (bIsMovingOnGrid || bIsRotatingOnGrid)
	{
		return;
	}

	FVector NextPosition = CalculateNextGridPosition(Direction);

	if (CanMoveToPosition(NextPosition))
	{
		StartGridPosition = GetActorLocation();
		TargetGridPosition = NextPosition;
		GridMoveProgress = 0.0f;
		bIsMovingOnGrid = true;
	}
}

void AFirstPersonRPGCharacter::TurnLeft()
{
	if (bIsMovingOnGrid || bIsRotatingOnGrid)
	{
		return;
	}

	StartGridRotation = GetActorRotation();
	TargetGridRotation = StartGridRotation;
	TargetGridRotation.Yaw -= GridRotationAngle;
	GridRotationProgress = 0.0f;
	bIsRotatingOnGrid = true;
}

void AFirstPersonRPGCharacter::TurnRight()
{
	if (bIsMovingOnGrid || bIsRotatingOnGrid)
	{
		return;
	}

	StartGridRotation = GetActorRotation();
	TargetGridRotation = StartGridRotation;
	TargetGridRotation.Yaw += GridRotationAngle;
	GridRotationProgress = 0.0f;
	bIsRotatingOnGrid = true;
}

void AFirstPersonRPGCharacter::ProcessGridMovement(float DeltaTime)
{
	if (!bIsMovingOnGrid)
	{
		return;
	}

	GridMoveProgress += DeltaTime / GridMoveTime;
	
	if (GridMoveProgress >= 1.0f)
	{
		// Movimento completo
		GridMoveProgress = 1.0f;
		SetActorLocation(TargetGridPosition);
		CurrentGridPosition = TargetGridPosition;
		bIsMovingOnGrid = false;
		
		// Notificar que um passo foi dado (para random encounters)
		CurrentStepCount++;
		if (CurrentStepCount >= StepsPerEncounterCheck)
		{
			OnStepTaken();
			CurrentStepCount = 0;
		}
	}
	else
	{
		// Interpolação suave (ease in-out)
		float SmoothProgress = FMath::InterpEaseInOut(0.0f, 1.0f, GridMoveProgress, 2.0f);
		FVector NewLocation = FMath::Lerp(StartGridPosition, TargetGridPosition, SmoothProgress);
		SetActorLocation(NewLocation);
	}
}

void AFirstPersonRPGCharacter::ProcessGridRotation(float DeltaTime)
{
	if (!bIsRotatingOnGrid)
	{
		return;
	}

	GridRotationProgress += DeltaTime / GridMoveTime;
	
	if (GridRotationProgress >= 1.0f)
	{
		// Rotação completa
		GridRotationProgress = 1.0f;
		
		// Atualizar rotação do ACTOR (não do controller)
		SetActorRotation(TargetGridRotation);
		
		bIsRotatingOnGrid = false;
	}
	else
	{
		// Interpolação suave
		float SmoothProgress = FMath::InterpEaseInOut(0.0f, 1.0f, GridRotationProgress, 2.0f);
		FRotator NewRotation = FMath::Lerp(StartGridRotation, TargetGridRotation, SmoothProgress);
		
		// Rotacionar o ACTOR diretamente
		SetActorRotation(NewRotation);
	}
}

FVector AFirstPersonRPGCharacter::CalculateNextGridPosition(EGridDirection Direction) const
{
	FVector CurrentPos = GetActorLocation();
	FRotator CurrentRot = GetActorRotation(); // Usar rotação do Actor
	
	// Obter direção forward baseada na rotação atual
	FVector ForwardDir = FRotationMatrix(FRotator(0, CurrentRot.Yaw, 0)).GetUnitAxis(EAxis::X);
	FVector RightDir = FRotationMatrix(FRotator(0, CurrentRot.Yaw, 0)).GetUnitAxis(EAxis::Y);
	
	FVector MoveDirection;
	
	switch (Direction)
	{
	case EGridDirection::North:
		MoveDirection = ForwardDir;
		break;
	case EGridDirection::South:
		MoveDirection = -ForwardDir;
		break;
	case EGridDirection::East:
		MoveDirection = RightDir;
		break;
	case EGridDirection::West:
		MoveDirection = -RightDir;
		break;
	}
	
	return CurrentPos + (MoveDirection * GridCellSize);
}

bool AFirstPersonRPGCharacter::CanMoveToPosition(const FVector& TargetPosition) const
{
	// Fazer line trace para verificar colisão
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	// Trace do centro do personagem
	FVector StartPos = GetActorLocation();
	FVector EndPos = TargetPosition;
	
	// Ajustar altura para o centro da cápsula
	StartPos.Z += 50.0f;
	EndPos.Z += 50.0f;
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartPos,
		EndPos,
		ECC_Visibility,
		QueryParams
	);
	
	// Se não bateu em nada, pode mover
	return !bHit;
}

void AFirstPersonRPGCharacter::SnapToGrid()
{
	FVector CurrentPos = GetActorLocation();
	
	// Arredondar para o grid mais próximo
	CurrentPos.X = FMath::RoundToFloat(CurrentPos.X / GridCellSize) * GridCellSize;
	CurrentPos.Y = FMath::RoundToFloat(CurrentPos.Y / GridCellSize) * GridCellSize;
	
	SetActorLocation(CurrentPos);
}

// ==================== EVENTOS ====================

void AFirstPersonRPGCharacter::OnStepTaken_Implementation()
{
	// Implementação padrão - pode ser sobrescrita em Blueprint
	// Aqui será chamado o Random Encounter Manager
	UE_LOG(LogTemp, Verbose, TEXT("FirstPersonRPGCharacter: Passo dado! Total: %d"), CurrentStepCount);
}

void AFirstPersonRPGCharacter::Interact_Implementation()
{
	// Fazer line trace para frente e verificar interação
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	FVector StartPos = FirstPersonCamera->GetComponentLocation();
	FVector EndPos = StartPos + (FirstPersonCamera->GetForwardVector() * 200.0f);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartPos,
		EndPos,
		ECC_Visibility,
		QueryParams
	);
	
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("FirstPersonRPGCharacter: Interagindo com %s"), *HitResult.GetActor()->GetName());
		// TODO: Chamar interface de interação no ator
	}
}

void AFirstPersonRPGCharacter::OpenMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("FirstPersonRPGCharacter: Abrindo menu..."));
	// TODO: Implementar abertura do menu
}
