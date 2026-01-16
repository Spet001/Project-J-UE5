// FirstPersonRPGCharacter.h
// Character Controller principal para exploração em primeira pessoa estilo SMT1

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FirstPersonRPGCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

/**
 * Enum para o tipo de movimento do personagem
 * SMT1 usa movimento em grid, mas oferecemos opção de movimento livre também
 */
UENUM(BlueprintType)
enum class EMovementStyle : uint8
{
	GridBased    UMETA(DisplayName = "Grid Based (SMT Style)"),
	FreeMovement UMETA(DisplayName = "Free Movement")
};

/**
 * Enum para direções no grid
 */
UENUM(BlueprintType)
enum class EGridDirection : uint8
{
	North,
	South,
	East,
	West
};

/**
 * Character Controller de primeira pessoa para RPG estilo Shin Megami Tensei 1
 * Suporta movimento em grid (estilo dungeon crawler) ou livre
 */
UCLASS(config=Game)
class J_API AFirstPersonRPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFirstPersonRPGCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void Tick(float DeltaTime) override;

	// ==================== COMPONENTES ====================
	
	/** Câmera em primeira pessoa */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCamera;

	// ==================== INPUT ACTIONS ====================
	
	/** Input Mapping Context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	/** Input Action para movimento */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	/** Input Action para olhar ao redor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	/** Input Action para interagir */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	/** Input Action para abrir menu */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MenuAction;

	// ==================== CONFIGURAÇÕES DE MOVIMENTO ====================
	
	/** Estilo de movimento (Grid ou Livre) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementStyle MovementStyle = EMovementStyle::GridBased;

	/** Tamanho de cada célula do grid (em unidades Unreal) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Grid", meta = (EditCondition = "MovementStyle == EMovementStyle::GridBased"))
	float GridCellSize = 200.0f;

	/** Tempo para completar um movimento no grid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Grid", meta = (EditCondition = "MovementStyle == EMovementStyle::GridBased"))
	float GridMoveTime = 0.25f;

	/** Ângulo de rotação no grid (90 graus padrão) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Grid", meta = (EditCondition = "MovementStyle == EMovementStyle::GridBased"))
	float GridRotationAngle = 90.0f;

	/** Velocidade de rotação do mouse (modo livre) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Free")
	float LookSensitivity = 1.0f;

	/** Velocidade de movimento (modo livre) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Free")
	float MoveSpeed = 400.0f;

	// ==================== CONFIGURAÇÕES DE RANDOM ENCOUNTER ====================
	
	/** Passos até checar por encontro aleatório */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters")
	int32 StepsPerEncounterCheck = 1;

	/** Contador de passos atual */
	UPROPERTY(BlueprintReadOnly, Category = "Encounters")
	int32 CurrentStepCount = 0;

	// ==================== FUNÇÕES PÚBLICAS ====================
	
	/** Move o personagem uma célula na direção especificada (Grid Mode) */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveInDirection(EGridDirection Direction);

	/** Rotaciona o personagem 90 graus (Grid Mode) */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void TurnLeft();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void TurnRight();

	/** Retorna se o personagem está se movendo no grid */
	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsMovingOnGrid() const { return bIsMovingOnGrid; }

	/** Retorna a posição atual no grid */
	UFUNCTION(BlueprintPure, Category = "Movement")
	FVector GetCurrentGridPosition() const { return CurrentGridPosition; }

	/** Chamado quando um passo é dado (para random encounters) */
	UFUNCTION(BlueprintNativeEvent, Category = "Encounters")
	void OnStepTaken();

	/** Interagir com objeto à frente */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact();

	/** Abrir menu do jogo */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void OpenMenu();

protected:
	// ==================== INPUT HANDLERS ====================
	
	void HandleMoveInput(const FInputActionValue& Value);
	void HandleLookInput(const FInputActionValue& Value);
	void HandleInteractInput();
	void HandleMenuInput();

	// ==================== GRID MOVEMENT HELPERS ====================
	
	/** Processa movimento suave no grid */
	void ProcessGridMovement(float DeltaTime);

	/** Processa rotação suave no grid */
	void ProcessGridRotation(float DeltaTime);

	/** Calcula próxima posição no grid baseado na direção atual */
	FVector CalculateNextGridPosition(EGridDirection Direction) const;

	/** Verifica se pode mover para a posição (colisão) */
	bool CanMoveToPosition(const FVector& TargetPosition) const;

	/** Alinha a posição atual ao grid mais próximo */
	void SnapToGrid();

private:
	// Estado do movimento em grid
	bool bIsMovingOnGrid = false;
	bool bIsRotatingOnGrid = false;
	FVector CurrentGridPosition;
	FVector TargetGridPosition;
	FRotator TargetGridRotation;
	float GridMoveProgress = 0.0f;
	float GridRotationProgress = 0.0f;

	// Posição e rotação iniciais para interpolação
	FVector StartGridPosition;
	FRotator StartGridRotation;
};
