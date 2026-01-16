// CombatManager.cpp

#include "CombatManager.h"
#include "Kismet/GameplayStatics.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("CombatManager: Iniciado!"));
}

void ACombatManager::StartCombat(const TArray<AActor*>& InPlayerParty, const TArray<AActor*>& InEnemies)
{
	if (IsCombatActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("CombatManager: Combate já está ativo!"));
		return;
	}

	PlayerParty = InPlayerParty;
	Enemies = InEnemies;
	CurrentTurn = 0;
	ActiveParticipantIndex = 0;

	UE_LOG(LogTemp, Log, TEXT("CombatManager: Combate iniciado! %d jogadores vs %d inimigos"), 
		PlayerParty.Num(), Enemies.Num());

	// Determinar ordem de turnos
	DetermineTurnOrder();

	// Iniciar combate
	CurrentState = ECombatState::Initializing;
	OnCombatStarted.Broadcast();

	// Primeiro turno
	NextTurn();
}

void ACombatManager::EndCombat(ECombatState EndState)
{
	UE_LOG(LogTemp, Log, TEXT("CombatManager: Combate terminado! Estado: %d"), (int32)EndState);

	CurrentState = EndState;
	OnCombatEnded.Broadcast(EndState);

	// Limpar
	PlayerParty.Empty();
	Enemies.Empty();
	TurnOrder.Empty();
	CurrentTurn = 0;
	ActiveParticipantIndex = 0;

	// Voltar ao estado inativo
	CurrentState = ECombatState::Inactive;
}

void ACombatManager::NextTurn()
{
	if (!IsCombatActive() || CurrentState == ECombatState::Victory || CurrentState == ECombatState::Defeat)
	{
		return;
	}

	CurrentTurn++;
	ActiveParticipantIndex = 0;

	// Verificar se o combate acabou
	CheckCombatEnd();
	
	if (!IsCombatActive())
	{
		return;
	}

	// Determinar se é turno do jogador ou inimigo
	// Por simplicidade, primeiro todos os jogadores, depois todos os inimigos
	CurrentState = ECombatState::PlayerTurn;
	
	UE_LOG(LogTemp, Log, TEXT("CombatManager: Turno %d - Vez do Jogador"), CurrentTurn);
	OnTurnChanged.Broadcast(true);
}

void ACombatManager::ExecuteAction(ECombatAction Action, AActor* Target, FName SkillID)
{
	if (!IsCombatActive())
	{
		return;
	}

	AActor* ActiveActor = GetActiveParticipant();
	if (!ActiveActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("CombatManager: %s executa ação %d"), *ActiveActor->GetName(), (int32)Action);

	CurrentState = ECombatState::Animating;

	switch (Action)
	{
	case ECombatAction::Attack:
		if (Target)
		{
			FAttackResult Result = CalculateBasicAttack(ActiveActor, Target);
			OnDamageDealt.Broadcast(Target, Result);
			
			// TODO: Aplicar dano ao target
			UE_LOG(LogTemp, Log, TEXT("CombatManager: Ataque causou %d de dano! Crítico: %s"), 
				Result.Damage, Result.bCritical ? TEXT("Sim") : TEXT("Não"));
		}
		break;

	case ECombatAction::Skill:
		// TODO: Implementar uso de skills
		break;

	case ECombatAction::Item:
		// TODO: Implementar uso de itens
		break;

	case ECombatAction::Guard:
		// TODO: Implementar defesa
		break;

	case ECombatAction::Escape:
		TryEscape();
		return;

	case ECombatAction::Talk:
		// TODO: Implementar sistema de negociação
		break;
	}

	// Próximo participante ou próximo turno
	ActiveParticipantIndex++;
	
	if (IsPlayerTurn() && ActiveParticipantIndex >= PlayerParty.Num())
	{
		// Todos os jogadores agiram, turno dos inimigos
		CurrentState = ECombatState::EnemyTurn;
		ActiveParticipantIndex = 0;
		OnTurnChanged.Broadcast(false);
		ProcessEnemyTurn();
	}
	else if (!IsPlayerTurn() && ActiveParticipantIndex >= Enemies.Num())
	{
		// Todos os inimigos agiram, próximo turno
		NextTurn();
	}
	else
	{
		CurrentState = IsPlayerTurn() ? ECombatState::PlayerTurn : ECombatState::EnemyTurn;
	}
}

bool ACombatManager::TryEscape()
{
	// Chance de fuga baseada em Agility
	// Por enquanto, 50% de chance
	float EscapeChance = 50.0f;
	
	if (FMath::FRandRange(0.0f, 100.0f) < EscapeChance)
	{
		UE_LOG(LogTemp, Log, TEXT("CombatManager: Fuga bem sucedida!"));
		EndCombat(ECombatState::Escaped);
		return true;
	}
	
	UE_LOG(LogTemp, Log, TEXT("CombatManager: Fuga falhou!"));
	return false;
}

FAttackResult ACombatManager::CalculateDamage(AActor* Attacker, AActor* Defender, const FSkillData& Skill)
{
	FAttackResult Result;
	
	// TODO: Obter stats do Attacker e Defender
	// Por enquanto, usar valores placeholder
	int32 AttackStat = Skill.Element == ERPGElement::Physical ? 10 : 10; // STR ou MAG
	int32 DefenseStat = Skill.Element == ERPGElement::Physical ? 10 : 10; // VIT ou MAG

	// Verificar acerto
	float HitRoll = FMath::FRandRange(0.0f, 100.0f);
	if (HitRoll > Skill.Accuracy)
	{
		Result.bHit = false;
		Result.Damage = 0;
		return Result;
	}

	// Calcular dano base
	// Fórmula: (BasePower + AttackStat) * random(0.9-1.1) - DefenseStat/2
	float RandomMod = FMath::FRandRange(0.9f, 1.1f);
	int32 BaseDamage = FMath::Max(1, (int32)((Skill.BasePower + AttackStat) * RandomMod) - (DefenseStat / 2));

	// TODO: Verificar afinidade do defensor
	Result.AffinityResult = EElementAffinity::Normal;
	float AffinityMult = GetAffinityMultiplier(Result.AffinityResult);

	// Verificar crítico (baseado em LUK)
	float CritChance = 5.0f; // 5% base
	if (FMath::FRandRange(0.0f, 100.0f) < CritChance)
	{
		Result.bCritical = true;
		AffinityMult *= 1.5f;
	}

	Result.Damage = FMath::Max(1, (int32)(BaseDamage * AffinityMult));
	Result.bHit = true;

	return Result;
}

FAttackResult ACombatManager::CalculateBasicAttack(AActor* Attacker, AActor* Defender)
{
	// Ataque básico físico
	FSkillData BasicAttack;
	BasicAttack.SkillID = FName("BasicAttack");
	BasicAttack.Element = ERPGElement::Physical;
	BasicAttack.BasePower = 30;
	BasicAttack.Accuracy = 90.0f;

	return CalculateDamage(Attacker, Defender, BasicAttack);
}

float ACombatManager::GetAffinityMultiplier(EElementAffinity Affinity)
{
	switch (Affinity)
	{
	case EElementAffinity::Weak:   return 2.0f;
	case EElementAffinity::Resist: return 0.5f;
	case EElementAffinity::Null:   return 0.0f;
	case EElementAffinity::Repel:  return -1.0f;  // Dano refletido
	case EElementAffinity::Drain:  return -0.5f;  // Cura em vez de dano
	default:                       return 1.0f;
	}
}

AActor* ACombatManager::GetActiveParticipant() const
{
	if (IsPlayerTurn())
	{
		if (PlayerParty.IsValidIndex(ActiveParticipantIndex))
		{
			return PlayerParty[ActiveParticipantIndex];
		}
	}
	else
	{
		if (Enemies.IsValidIndex(ActiveParticipantIndex))
		{
			return Enemies[ActiveParticipantIndex];
		}
	}
	return nullptr;
}

void ACombatManager::CheckCombatEnd()
{
	// Verificar se todos os inimigos foram derrotados
	// TODO: Verificar HP dos inimigos
	bool bAllEnemiesDefeated = Enemies.Num() == 0;
	
	if (bAllEnemiesDefeated)
	{
		EndCombat(ECombatState::Victory);
		return;
	}

	// Verificar se todos os jogadores foram derrotados
	// TODO: Verificar HP dos jogadores
	bool bAllPlayersDefeated = PlayerParty.Num() == 0;
	
	if (bAllPlayersDefeated)
	{
		EndCombat(ECombatState::Defeat);
		return;
	}
}

void ACombatManager::DetermineTurnOrder()
{
	TurnOrder.Empty();
	
	// Combinar todos os participantes
	TurnOrder.Append(PlayerParty);
	TurnOrder.Append(Enemies);

	// TODO: Ordenar por Agility
	// Por enquanto, ordem de inserção
}

void ACombatManager::ProcessEnemyTurn()
{
	UE_LOG(LogTemp, Log, TEXT("CombatManager: Turno do Inimigo"));

	// IA simples: cada inimigo ataca um jogador aleatório
	for (int32 i = 0; i < Enemies.Num(); i++)
	{
		if (PlayerParty.Num() > 0)
		{
			int32 RandomTarget = FMath::RandRange(0, PlayerParty.Num() - 1);
			AActor* Target = PlayerParty[RandomTarget];
			
			ActiveParticipantIndex = i;
			ExecuteAction(ECombatAction::Attack, Target);
		}
	}

	// Voltar para o turno do jogador
	NextTurn();
}
