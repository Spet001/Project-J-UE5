// JGameInstance.h
// GameInstance para dados persistentes entre níveis

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "JGameInstance.generated.h"

/**
 * GameInstance para manter dados persistentes do RPG
 * Estatísticas do jogador, inventário, progresso, etc.
 */
UCLASS()
class J_API UJGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UJGameInstance();

	// Dados do jogador que persistem entre mapas
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	int32 PlayerLevel = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	int32 PlayerExperience = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	int32 PlayerGold = 100;

	// Função para salvar/carregar progresso (implementar depois)
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void LoadGame();
};
