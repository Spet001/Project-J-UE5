// EnemyBase.cpp

#include "EnemyBase.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Garantir HP cheio no início
	Stats.CurrentHP = Stats.MaxHP;
	Stats.CurrentMP = Stats.MaxMP;
}

void AEnemyBase::ApplyRPGDamage(int32 Amount, ERPGElement Element)
{
	EElementAffinity Affinity = GetElementAffinity(Element);
	
	int32 FinalDamage = Amount;
	
	switch (Affinity)
	{
	case EElementAffinity::Weak:
		FinalDamage = Amount * 2;
		UE_LOG(LogTemp, Log, TEXT("%s: Fraqueza! Dano dobrado (%d)"), *EnemyName.ToString(), FinalDamage);
		break;
		
	case EElementAffinity::Resist:
		FinalDamage = Amount / 2;
		UE_LOG(LogTemp, Log, TEXT("%s: Resistência! Dano reduzido (%d)"), *EnemyName.ToString(), FinalDamage);
		break;
		
	case EElementAffinity::Null:
		FinalDamage = 0;
		UE_LOG(LogTemp, Log, TEXT("%s: Nulo! Sem dano"), *EnemyName.ToString());
		break;
		
	case EElementAffinity::Drain:
		Heal(Amount);
		UE_LOG(LogTemp, Log, TEXT("%s: Absorve! Curou %d"), *EnemyName.ToString(), Amount);
		return;
		
	case EElementAffinity::Repel:
		UE_LOG(LogTemp, Log, TEXT("%s: Reflete! (TODO: refletir dano)"), *EnemyName.ToString());
		// TODO: Refletir dano de volta
		return;
		
	default:
		break;
	}
	
	Stats.CurrentHP = FMath::Max(0, Stats.CurrentHP - FinalDamage);
	
	UE_LOG(LogTemp, Log, TEXT("%s: Recebeu %d de dano. HP: %d/%d"), 
		*EnemyName.ToString(), FinalDamage, Stats.CurrentHP, Stats.MaxHP);
	
	if (IsDead())
	{
		UE_LOG(LogTemp, Log, TEXT("%s: Derrotado!"), *EnemyName.ToString());
		// TODO: Trigger death event
	}
}

void AEnemyBase::Heal(int32 Amount)
{
	Stats.CurrentHP = FMath::Min(Stats.MaxHP, Stats.CurrentHP + Amount);
	UE_LOG(LogTemp, Log, TEXT("%s: Curou %d. HP: %d/%d"), 
		*EnemyName.ToString(), Amount, Stats.CurrentHP, Stats.MaxHP);
}

EElementAffinity AEnemyBase::GetElementAffinity(ERPGElement Element) const
{
	return Affinities.GetAffinity(Element);
}

FSkillData AEnemyBase::SelectAction_Implementation()
{
	// IA básica: selecionar skill aleatória se tiver MP, senão ataque básico
	if (Skills.Num() > 0)
	{
		// Filtrar skills que pode usar (tem MP suficiente)
		TArray<FSkillData> UsableSkills;
		for (const FSkillData& Skill : Skills)
		{
			if (Stats.CurrentMP >= Skill.MPCost)
			{
				UsableSkills.Add(Skill);
			}
		}
		
		if (UsableSkills.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, UsableSkills.Num() - 1);
			return UsableSkills[RandomIndex];
		}
	}
	
	// Ataque básico como fallback
	FSkillData BasicAttack;
	BasicAttack.SkillID = FName("BasicAttack");
	BasicAttack.DisplayName = FText::FromString("Attack");
	BasicAttack.Element = ERPGElement::Physical;
	BasicAttack.BasePower = 30;
	BasicAttack.MPCost = 0;
	BasicAttack.Accuracy = 90.0f;
	
	return BasicAttack;
}
