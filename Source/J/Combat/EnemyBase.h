// EnemyBase.h
// Classe base para inimigos do jogo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/RPGTypes.h"
#include "EnemyBase.generated.h"

/**
 * Classe base para todos os inimigos/demônios
 */
UCLASS(Abstract, Blueprintable)
class J_API AEnemyBase : public AActor
{
	GENERATED_BODY()

public:
	AEnemyBase();

	// ==================== IDENTIFICAÇÃO ====================

	/** Nome do inimigo para exibição */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	FText EnemyName;

	/** Descrição do inimigo */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	FText EnemyDescription;

	/** Ícone/Sprite do inimigo */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	UTexture2D* EnemySprite;

	// ==================== STATS ====================

	/** Estatísticas do inimigo */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Stats")
	FCharacterStats Stats;

	/** Afinidades elementais */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Stats")
	FElementAffinities Affinities;

	// ==================== HABILIDADES ====================

	/** Skills que o inimigo pode usar */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Skills")
	TArray<FSkillData> Skills;

	// ==================== RECOMPENSAS ====================

	/** EXP concedido ao derrotar */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Rewards")
	int32 ExperienceReward = 10;

	/** Gold concedido ao derrotar */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Rewards")
	int32 GoldReward = 5;

	/** Chance de drop de item (0-100) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Rewards")
	float ItemDropChance = 10.0f;

	// ==================== FUNÇÕES ====================

	/** Aplica dano ao inimigo */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ApplyRPGDamage(int32 Amount, ERPGElement Element);

	/** Cura o inimigo */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void Heal(int32 Amount);

	/** Verifica se está morto */
	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsDead() const { return Stats.CurrentHP <= 0; }

	/** Retorna a afinidade para um elemento */
	UFUNCTION(BlueprintPure, Category = "Enemy")
	EElementAffinity GetElementAffinity(ERPGElement Element) const;

	/** Seleciona uma ação de IA */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy|AI")
	FSkillData SelectAction();

protected:
	virtual void BeginPlay() override;
};
