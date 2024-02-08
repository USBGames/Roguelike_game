// Fill out your copyright notice in the Description page of Project Settings.


#include "PsychicManaComponent.h"
#include "PsychicCharacter.h"
#include "PsychicDevUtils.h"

// Sets default values for this component's properties
UPsychicManaComponent::UPsychicManaComponent() : 
	ManaBase(1000.0f),
	ManaCurrent(0.0f),
	ManaConsumption(100.0f),
	ManaRechargeTime(2.5f),
	ManaRechargeRate(75.0f),

	bJustFired(false),
	ManaRechargeTimeCurrent(0.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPsychicManaComponent::BeginPlay()
{
	Super::BeginPlay();	

	ManaCurrent = ManaBase;	
}

void UPsychicManaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bJustFired)
	{
		ManaRechargeTimeCurrent += DeltaTime;
		if (ManaRechargeTimeCurrent >= ManaRechargeTime)
		{
			bJustFired = false;
			ManaRechargeTimeCurrent = 0.0f;
		}
	}
	else if(!(PsychicCharacter->GetTeleState() == ETeleState::ETS_Holding) && !(PsychicCharacter->GetTeleState() == ETeleState::ETS_CanFire))
	{
		ManaCurrent += DeltaTime * ManaRechargeRate;
		ManaCurrent = FMath::Clamp(ManaCurrent, 0.0f, ManaBase);
	}
}

void UPsychicManaComponent::SetPsychicCharacter(APsychicCharacter* Character)
{
	PsychicCharacter = Character;
	PsychicCharacter->OnItemShoot.AddDynamic(this, &UPsychicManaComponent::OnItemShoot);	
}

void UPsychicManaComponent::OnItemShoot(APsychicCharacter* Character)
{
	//DebugLog(-1, 1.0f, FColor::Cyan, "Item SHOOT!");

	if (PsychicCharacter == nullptr)
		PsychicCharacter = Character;

	ManaCurrent = FMath::Clamp(ManaCurrent - ManaConsumption, 0.0f, ManaBase);
	
	bJustFired = true;
	ManaRechargeTimeCurrent = 0.0f;
}