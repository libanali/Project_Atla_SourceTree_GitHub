// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_LowPolyMoveTo.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


UBTTask_LowPolyMoveTo::UBTTask_LowPolyMoveTo()
{

	NodeName = "Move To Actor";

}

EBTNodeResult::Type UBTTask_LowPolyMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	
	return EBTNodeResult::Failed;
}


