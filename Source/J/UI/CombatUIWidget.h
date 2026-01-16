// CombatUIWidget.h
// Widget para interface de combate

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Combat/CombatManager.h"
#include "CombatUIWidget.generated.h"

/**
 * Widget base para UI de combate
 */
UCLASS(Abstract, Blueprintable)
class J_API UCombatUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Referência ao Combat Manager */
	UPROPERTY(BlueprintReadWrite, Category = "Combat UI")
	ACombatManager* CombatManager;

	/** Inicializa a UI de combate */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat UI")
	void InitializeCombatUI(ACombatManager* InCombatManager);

	/** Mostra menu de ações do jogador */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void ShowActionMenu();

	/** Esconde menu de ações */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void HideActionMenu();

	/** Mostra seleção de alvo */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void ShowTargetSelection(const TArray<AActor*>& Targets);

	/** Atualiza HP/MP de um participante */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void UpdateParticipantStats(AActor* Participant, int32 CurrentHP, int32 MaxHP, int32 CurrentMP, int32 MaxMP);

	/** Mostra resultado de ataque */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void ShowAttackResult(const FAttackResult& Result, AActor* Target);

	/** Mostra mensagem de texto */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void ShowCombatMessage(const FText& Message);

	/** Mostra tela de vitória */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void ShowVictoryScreen(int32 ExpGained, int32 GoldGained);

	/** Mostra tela de derrota */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat UI")
	void ShowDefeatScreen();

protected:
	virtual void NativeOnInitialized() override;

	/** Chamado quando uma ação é selecionada */
	UFUNCTION(BlueprintCallable, Category = "Combat UI")
	void OnActionSelected(ECombatAction Action);

	/** Chamado quando um alvo é selecionado */
	UFUNCTION(BlueprintCallable, Category = "Combat UI")
	void OnTargetSelected(AActor* Target);
};
