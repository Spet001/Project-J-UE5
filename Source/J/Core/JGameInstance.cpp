// JGameInstance.cpp

#include "JGameInstance.h"

UJGameInstance::UJGameInstance()
{
	// Valores iniciais
	PlayerLevel = 1;
	PlayerExperience = 0;
	PlayerGold = 100;
}

void UJGameInstance::SaveGame()
{
	// TODO: Implementar sistema de save
	UE_LOG(LogTemp, Log, TEXT("JGameInstance: Salvando jogo..."));
}

void UJGameInstance::LoadGame()
{
	// TODO: Implementar sistema de load
	UE_LOG(LogTemp, Log, TEXT("JGameInstance: Carregando jogo..."));
}
