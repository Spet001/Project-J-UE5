// RandomEncounterManager.cpp

#include "RandomEncounterManager.h"
#include "TimerManager.h"

URandomEncounterManager::URandomEncounterManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URandomEncounterManager::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("RandomEncounterManager: Iniciado! Taxa base: %.1f%%"), BaseEncounterRate);
}

bool URandomEncounterManager::CheckForEncounter()
{
	if (!bEncountersEnabled || AreaEncounters.Num() == 0)
	{
		return false;
	}

	StepsSinceLastEncounter++;

	// Verificar se passou do mínimo de passos
	if (StepsSinceLastEncounter < MinStepsBetweenEncounters)
	{
		return false;
	}

	// Calcular chance atual
	float CurrentChance = CalculateCurrentEncounterChance();

	// Rolar dado
	float Roll = FMath::FRandRange(0.0f, 100.0f);

	UE_LOG(LogTemp, Verbose, TEXT("RandomEncounterManager: Passo %d, Chance: %.1f%%, Roll: %.1f"), 
		StepsSinceLastEncounter, CurrentChance, Roll);

	if (Roll < CurrentChance)
	{
		// Encontro!
		ForceEncounter();
		return true;
	}

	// Encontro garantido se passou do máximo
	if (StepsSinceLastEncounter >= MaxStepsWithoutEncounter)
	{
		ForceEncounter();
		return true;
	}

	return false;
}

void URandomEncounterManager::ForceEncounter()
{
	if (AreaEncounters.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("RandomEncounterManager: Nenhum encontro configurado na área!"));
		return;
	}

	FEncounterData SelectedEncounter = SelectRandomEncounter();
	
	UE_LOG(LogTemp, Log, TEXT("RandomEncounterManager: ENCONTRO! ID: %s"), *SelectedEncounter.EncounterID.ToString());

	// Resetar contador
	StepsSinceLastEncounter = 0;

	// Disparar evento
	OnEncounterTriggered.Broadcast(SelectedEncounter);
}

FEncounterData URandomEncounterManager::SelectRandomEncounter() const
{
	if (AreaEncounters.Num() == 0)
	{
		return FEncounterData();
	}

	// Calcular peso total
	float TotalWeight = 0.0f;
	for (const FEncounterData& Encounter : AreaEncounters)
	{
		TotalWeight += Encounter.Weight;
	}

	// Selecionar baseado em peso
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	float CurrentWeight = 0.0f;

	for (const FEncounterData& Encounter : AreaEncounters)
	{
		CurrentWeight += Encounter.Weight;
		if (RandomValue <= CurrentWeight)
		{
			return Encounter;
		}
	}

	// Fallback para o último
	return AreaEncounters.Last();
}

void URandomEncounterManager::SetAreaEncounters(const TArray<FEncounterData>& NewEncounters)
{
	AreaEncounters = NewEncounters;
	UE_LOG(LogTemp, Log, TEXT("RandomEncounterManager: Área atualizada com %d encontros"), AreaEncounters.Num());
}

void URandomEncounterManager::ResetStepCounter()
{
	StepsSinceLastEncounter = 0;
}

void URandomEncounterManager::DisableEncounters(float Duration)
{
	bEncountersEnabled = false;

	if (Duration > 0.0f)
	{
		// Configurar timer para reabilitar
		GetWorld()->GetTimerManager().SetTimer(
			ReenableEncountersTimer,
			this,
			&URandomEncounterManager::EnableEncounters,
			Duration,
			false
		);
	}

	UE_LOG(LogTemp, Log, TEXT("RandomEncounterManager: Encontros desabilitados%s"), 
		Duration > 0.0f ? *FString::Printf(TEXT(" por %.1f segundos"), Duration) : TEXT(""));
}

void URandomEncounterManager::EnableEncounters()
{
	bEncountersEnabled = true;
	UE_LOG(LogTemp, Log, TEXT("RandomEncounterManager: Encontros habilitados"));
}

float URandomEncounterManager::CalculateCurrentEncounterChance() const
{
	// Chance aumenta progressivamente após o mínimo de passos
	int32 StepsOverMinimum = StepsSinceLastEncounter - MinStepsBetweenEncounters;
	
	// Fórmula: BaseRate + (StepsOverMinimum * incremento)
	// Incremento faz a chance aumentar gradualmente
	float IncrementPerStep = (100.0f - BaseEncounterRate) / (MaxStepsWithoutEncounter - MinStepsBetweenEncounters);
	
	float CurrentChance = BaseEncounterRate + (StepsOverMinimum * IncrementPerStep);
	CurrentChance *= EncounterRateMultiplier;
	
	return FMath::Clamp(CurrentChance, 0.0f, 100.0f);
}
