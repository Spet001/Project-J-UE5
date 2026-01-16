// JGameMode.cpp

#include "JGameMode.h"
#include "Characters/FirstPersonRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

AJGameMode::AJGameMode()
{
	// Define o Character padrão do jogo
	DefaultPawnClass = AFirstPersonRPGCharacter::StaticClass();
}

void AJGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("JGameMode: Jogo iniciado!"));
}
