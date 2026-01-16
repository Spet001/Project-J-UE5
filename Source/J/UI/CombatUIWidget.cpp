// CombatUIWidget.cpp

#include "CombatUIWidget.h"

void UCombatUIWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UCombatUIWidget::InitializeCombatUI_Implementation(ACombatManager* InCombatManager)
{
	CombatManager = InCombatManager;
}

void UCombatUIWidget::OnActionSelected(ECombatAction Action)
{
	if (!CombatManager)
	{
		return;
	}

	if (Action == ECombatAction::Attack)
	{
		// Mostrar seleção de alvo para ataques
		ShowTargetSelection(CombatManager->Enemies);
	}
	else if (Action == ECombatAction::Escape)
	{
		CombatManager->TryEscape();
	}
	// TODO: Outras ações
}

void UCombatUIWidget::OnTargetSelected(AActor* Target)
{
	if (!CombatManager || !Target)
	{
		return;
	}

	// Executar ataque básico no alvo selecionado
	CombatManager->ExecuteAction(ECombatAction::Attack, Target);
}
