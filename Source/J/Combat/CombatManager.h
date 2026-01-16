// CombatManager.h
// Gerenciador do sistema de combate por turnos estilo SMT

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/RPGTypes.h"
#include "CombatManager.generated.h"

class ACombatParticipant;

/**
 * Enum para estado do combate
 */
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Inactive       UMETA(DisplayName = "Inactive"),
	Initializing   UMETA(DisplayName = "Initializing"),
	PlayerTurn     UMETA(DisplayName = "Player Turn"),
	EnemyTurn      UMETA(DisplayName = "Enemy Turn"),
	Animating      UMETA(DisplayName = "Animating"),
	Victory        UMETA(DisplayName = "Victory"),
	Defeat         UMETA(DisplayName = "Defeat"),
	Escaped        UMETA(DisplayName = "Escaped")
};

/**
 * Enum para ações de combate
 */
UENUM(BlueprintType)
enum class ECombatAction : uint8
{
	Attack    UMETA(DisplayName = "Attack"),
	Skill     UMETA(DisplayName = "Skill"),
	Item      UMETA(DisplayName = "Item"),
	Guard     UMETA(DisplayName = "Guard"),
	Escape    UMETA(DisplayName = "Escape"),
	Talk      UMETA(DisplayName = "Talk")  // Negociação estilo SMT
};

/**
 * Resultado de um ataque
 */
USTRUCT(BlueprintType)
struct FAttackResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bHit = true;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	int32 Damage = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bCritical = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	EElementAffinity AffinityResult = EElementAffinity::Normal;
};

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatEnded, ECombatState, EndState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, bool, bIsPlayerTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageDealt, AActor*, Target, const FAttackResult&, Result);

/**
 * Gerenciador central do sistema de combate
 * Controla turnos, ordem de ação, cálculo de dano, etc.
 */
UCLASS()
class J_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:
	ACombatManager();

protected:
	virtual void BeginPlay() override;

public:
	// ==================== ESTADO DO COMBATE ====================

	/** Estado atual do combate */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	ECombatState CurrentState = ECombatState::Inactive;

	/** Lista de participantes do lado do jogador */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TArray<AActor*> PlayerParty;

	/** Lista de inimigos */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TArray<AActor*> Enemies;

	/** Índice do participante ativo */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	int32 ActiveParticipantIndex = 0;

	/** Turno atual */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	int32 CurrentTurn = 0;

	// ==================== EVENTOS ====================

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnCombatStarted OnCombatStarted;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnCombatEnded OnCombatEnded;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnTurnChanged OnTurnChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
	FOnDamageDealt OnDamageDealt;

	// ==================== FUNÇÕES DE CONTROLE ====================

	/** Inicia um combate com os participantes especificados */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCombat(const TArray<AActor*>& InPlayerParty, const TArray<AActor*>& InEnemies);

	/** Termina o combate */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndCombat(ECombatState EndState);

	/** Avança para o próximo turno */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void NextTurn();

	/** Executa uma ação de combate */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteAction(ECombatAction Action, AActor* Target = nullptr, FName SkillID = NAME_None);

	/** Tenta fugir do combate */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryEscape();

	// ==================== FUNÇÕES DE CÁLCULO ====================

	/** Calcula dano de um ataque */
	UFUNCTION(BlueprintCallable, Category = "Combat|Calculation")
	FAttackResult CalculateDamage(AActor* Attacker, AActor* Defender, const FSkillData& Skill);

	/** Calcula dano de ataque físico básico */
	UFUNCTION(BlueprintCallable, Category = "Combat|Calculation")
	FAttackResult CalculateBasicAttack(AActor* Attacker, AActor* Defender);

	/** Obtém o multiplicador de dano baseado na afinidade */
	UFUNCTION(BlueprintPure, Category = "Combat|Calculation")
	static float GetAffinityMultiplier(EElementAffinity Affinity);

	// ==================== FUNÇÕES DE QUERY ====================

	/** Verifica se o combate está ativo */
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsCombatActive() const { return CurrentState != ECombatState::Inactive; }

	/** Retorna o participante ativo */
	UFUNCTION(BlueprintPure, Category = "Combat")
	AActor* GetActiveParticipant() const;

	/** Verifica se é o turno do jogador */
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsPlayerTurn() const { return CurrentState == ECombatState::PlayerTurn; }

	/** Verifica se algum lado foi derrotado */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CheckCombatEnd();

protected:
	/** Determina a ordem de ação baseada em Agility */
	void DetermineTurnOrder();

	/** Processa turno do inimigo (IA) */
	void ProcessEnemyTurn();

	/** Ordem de turnos */
	TArray<AActor*> TurnOrder;
};
