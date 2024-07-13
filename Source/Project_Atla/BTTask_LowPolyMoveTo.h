// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LowPolyMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ATLA_API UBTTask_LowPolyMoveTo : public UBTTaskNode
{
	GENERATED_BODY()
	

public:

	UBTTask_LowPolyMoveTo();


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
