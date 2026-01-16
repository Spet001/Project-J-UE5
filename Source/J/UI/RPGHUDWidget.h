// RPGHUDWidget.h
// Widget base para o HUD do jogo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGHUDWidget.generated.h"

/**
 * Widget base para HUD do RPG
 * Mostra HP, MP, e informações básicas
 */
UCLASS(Abstract, Blueprintable)
class J_API URPGHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Atualiza display de HP */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void UpdateHP(int32 Current, int32 Max);

	/** Atualiza display de MP */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void UpdateMP(int32 Current, int32 Max);

	/** Mostra mensagem na tela */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void ShowMessage(const FText& Message, float Duration = 3.0f);

	/** Mostra/esconde indicador de encontro */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "HUD")
	void ShowEncounterWarning(bool bShow);
};
