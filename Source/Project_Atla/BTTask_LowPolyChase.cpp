// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_LowPolyChase.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_LowPolyChase::UBTTask_LowPolyChase()
{

	NodeName = "Chase Player";

}

EBTNodeResult::Type UBTTask_LowPolyChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{



		return EBTNodeResult::Succeeded;


}
