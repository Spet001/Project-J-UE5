// RPGTypes.h
// Tipos e estruturas base do sistema RPG

#pragma once

#include "CoreMinimal.h"
#include "RPGTypes.generated.h"

/**
 * Enum para tipos de elementos (como em SMT)
 * Nota: Renomeado de EElementType para ERPGElement para evitar conflito com SlateCore
 */
UENUM(BlueprintType)
enum class ERPGElement : uint8
{
	None        UMETA(DisplayName = "None"),
	Physical    UMETA(DisplayName = "Physical"),
	Fire        UMETA(DisplayName = "Fire"),
	Ice         UMETA(DisplayName = "Ice"),
	Electric    UMETA(DisplayName = "Electric"),
	Wind        UMETA(DisplayName = "Wind"),
	Light       UMETA(DisplayName = "Light"),
	Dark        UMETA(DisplayName = "Dark"),
	Almighty    UMETA(DisplayName = "Almighty")
};

/**
 * Enum para afinidades de elemento
 */
UENUM(BlueprintType)
enum class EElementAffinity : uint8
{
	Normal   UMETA(DisplayName = "Normal"),    // Dano normal
	Weak     UMETA(DisplayName = "Weak"),      // Fraqueza (2x dano)
	Resist   UMETA(DisplayName = "Resist"),    // Resistência (0.5x dano)
	Null     UMETA(DisplayName = "Null"),      // Nulo (0 dano)
	Repel    UMETA(DisplayName = "Repel"),     // Reflete dano
	Drain    UMETA(DisplayName = "Drain")      // Absorve como HP
};

/**
 * Estrutura para estatísticas base de um personagem/demônio
 */
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxMP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentMP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Strength = 10;    // Dano físico

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Magic = 10;       // Dano mágico

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Vitality = 10;    // Defesa física

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Agility = 10;     // Velocidade/Evasão

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Luck = 10;        // Críticos/Drops
};

/**
 * Estrutura para definir afinidades elementais
 */
USTRUCT(BlueprintType)
struct FElementAffinities
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinities")
	EElementAffinity Physical = EElementAffinity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinities")
	EElementAffinity Fire = EElementAffinity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinities")
	EElementAffinity Ice = EElementAffinity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinities")
	EElementAffinity Electric = EElementAffinity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinities")
	EElementAffinity Wind = EElementAffinity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinities")
	EElementAffinity Light = EElementAffinity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affinities")
	EElementAffinity Dark = EElementAffinity::Normal;

	/** Retorna a afinidade para um elemento específico */
	EElementAffinity GetAffinity(ERPGElement Element) const
	{
		switch (Element)
		{
		case ERPGElement::Physical: return Physical;
		case ERPGElement::Fire: return Fire;
		case ERPGElement::Ice: return Ice;
		case ERPGElement::Electric: return Electric;
		case ERPGElement::Wind: return Wind;
		case ERPGElement::Light: return Light;
		case ERPGElement::Dark: return Dark;
		default: return EElementAffinity::Normal;
		}
	}
};

/**
 * Estrutura para uma skill/magia
 */
USTRUCT(BlueprintType)
struct FSkillData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	ERPGElement Element = ERPGElement::Physical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	int32 BasePower = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	int32 MPCost = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bTargetsAll = false;  // True = atinge todos os inimigos

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float Accuracy = 95.0f;  // Porcentagem de acerto
};

/**
 * Estrutura para dados de um encontro
 */
USTRUCT(BlueprintType)
struct FEncounterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
	FName EncounterID;

	/** Classes de inimigos que podem aparecer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
	TArray<TSubclassOf<class AEnemyBase>> PossibleEnemies;

	/** Número mínimo de inimigos */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
	int32 MinEnemies = 1;

	/** Número máximo de inimigos */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
	int32 MaxEnemies = 4;

	/** Peso do encontro (maior = mais comum) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
	float Weight = 1.0f;
};
