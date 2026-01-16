// RandomEncounterManager.h
// Gerenciador de encontros aleatórios estilo SMT

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/RPGTypes.h"
#include "RandomEncounterManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEncounterTriggered, const FEncounterData&, EncounterData);

/**
 * Componente que gerencia encontros aleatórios
 * Adicione ao seu Character ou GameMode
 */
UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class J_API URandomEncounterManager : public UActorComponent
{
	GENERATED_BODY()

public:
	URandomEncounterManager();

protected:
	virtual void BeginPlay() override;

public:
	// ==================== CONFIGURAÇÕES ====================

	/** Encontros ativos estão habilitados? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters")
	bool bEncountersEnabled = true;

	/** Chance base de encontro por passo (0-100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters", meta = (ClampMin = "0", ClampMax = "100"))
	float BaseEncounterRate = 10.0f;

	/** Número mínimo de passos entre encontros */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters")
	int32 MinStepsBetweenEncounters = 5;

	/** Número máximo de passos sem encontro garantido */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters")
	int32 MaxStepsWithoutEncounter = 30;

	/** Lista de encontros possíveis na área atual */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters")
	TArray<FEncounterData> AreaEncounters;

	/** Multiplicador de taxa de encontro (itens podem modificar) */
	UPROPERTY(BlueprintReadWrite, Category = "Encounters")
	float EncounterRateMultiplier = 1.0f;

	// ==================== EVENTOS ====================

	/** Chamado quando um encontro é acionado */
	UPROPERTY(BlueprintAssignable, Category = "Encounters")
	FOnEncounterTriggered OnEncounterTriggered;

	// ==================== FUNÇÕES ====================

	/** Chame esta função a cada passo do jogador */
	UFUNCTION(BlueprintCallable, Category = "Encounters")
	bool CheckForEncounter();

	/** Força um encontro */
	UFUNCTION(BlueprintCallable, Category = "Encounters")
	void ForceEncounter();

	/** Seleciona um encontro aleatório da lista */
	UFUNCTION(BlueprintCallable, Category = "Encounters")
	FEncounterData SelectRandomEncounter() const;

	/** Define a lista de encontros da área atual */
	UFUNCTION(BlueprintCallable, Category = "Encounters")
	void SetAreaEncounters(const TArray<FEncounterData>& NewEncounters);

	/** Reseta o contador de passos */
	UFUNCTION(BlueprintCallable, Category = "Encounters")
	void ResetStepCounter();

	/** Desabilita encontros temporariamente */
	UFUNCTION(BlueprintCallable, Category = "Encounters")
	void DisableEncounters(float Duration = 0.0f);

	/** Habilita encontros */
	UFUNCTION(BlueprintCallable, Category = "Encounters")
	void EnableEncounters();

protected:
	/** Contador de passos desde o último encontro */
	UPROPERTY(BlueprintReadOnly, Category = "Encounters")
	int32 StepsSinceLastEncounter = 0;

	/** Handle do timer para reabilitar encontros */
	FTimerHandle ReenableEncountersTimer;

	/** Calcula a chance atual de encontro */
	float CalculateCurrentEncounterChance() const;
};
