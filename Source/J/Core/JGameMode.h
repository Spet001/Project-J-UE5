// JGameMode.h
// GameMode principal do RPG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JGameMode.generated.h"

/**
 * GameMode principal do jogo RPG estilo SMT
 * Controla as regras gerais do jogo, spawn de jogadores, etc.
 */
UCLASS()
class J_API AJGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AJGameMode();

protected:
	virtual void BeginPlay() override;
};
