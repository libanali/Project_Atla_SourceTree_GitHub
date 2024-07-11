// Fill out your copyright notice in the Description page of Project Settings.


#include "LowPoly_AI_Controller.h"
#include "Ren_Low_Poly_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy_Poly.h"


TArray<ALowPoly_AI_Controller*> ALowPoly_AI_Controller::Enemies;
int32 ALowPoly_AI_Controller::CurrentTokenIndex = 0;

ALowPoly_AI_Controller::ALowPoly_AI_Controller()
{

	bHasToken = false;


}


void ALowPoly_AI_Controller::Tick(float DeltaTime)
{

	if (bHasToken)

	{

		if (CanAttack())
		{

			AttackPlayer();
			bHasToken = false;
			AssignNextToken();

		}

		else

		{
			MoveToPlayer();
		}

		

	}

}



void ALowPoly_AI_Controller::OnPossess(APawn* InPawn)
{

	Super::OnPossess(InPawn);

	
	AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(InPawn);

	if (Enemy)

	{

		AddEnemy(this);


	}

	
	

}



void ALowPoly_AI_Controller::AttackPlayer()
{

	AEnemy_Poly* Enemy = Cast<AEnemy_Poly>(GetPawn());

	if (Enemy)

	{

		Enemy->PlayAttackAnim();

	}


}


void ALowPoly_AI_Controller::MoveToPlayer()
{

	ARen_Low_Poly_Character* Player = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player)

	{
		MoveToActor(Player, 40.0f);
	}


}


bool ALowPoly_AI_Controller::CanAttack() const
{

	ARen_Low_Poly_Character* Player = Cast<ARen_Low_Poly_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Player && GetPawn()->GetDistanceTo(Player) < 200.0f)

	{

		return true;

	}


	return false;
}



void ALowPoly_AI_Controller::AddEnemy(ALowPoly_AI_Controller* Enemy)
{

	Enemies.Add(Enemy);

	if (Enemies.Num() == 1)

	{

		Enemies[0]->bHasToken = true;

	}


}

void ALowPoly_AI_Controller::RemoveEnemy(ALowPoly_AI_Controller* Enemy)
{

	Enemies.Remove(Enemy);

	if (CurrentTokenIndex >= Enemies.Num())

	{

		CurrentTokenIndex = 0;
	}

	if (Enemies.Num() > 0)

	{

		Enemies[CurrentTokenIndex]->bHasToken = true;

	}


}

void ALowPoly_AI_Controller::AssignNextToken()
{
	if (Enemies.Num() == 0)

	{

		return;
	}


	CurrentTokenIndex = (CurrentTokenIndex + 1) % Enemies.Num();
	Enemies[CurrentTokenIndex]->bHasToken = true;


}
