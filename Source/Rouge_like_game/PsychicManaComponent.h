// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PsychicManaComponent.generated.h"

class APsychicCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROUGE_LIKE_GAME_API UPsychicManaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPsychicManaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetPsychicCharacter(APsychicCharacter* Character);

	UFUNCTION()
	void OnItemShoot(APsychicCharacter* Character);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mana Properties", meta = (AllowPrivateAccess = "true"))
	APsychicCharacter* PsychicCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana Properties", meta = (AllowPrivateAccess = "true"))
	float ManaBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mana Runtime", meta = (AllowPrivateAccess = "true"))
	float ManaCurrent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana Properties", meta = (AllowPrivateAccess = "true"))
	float ManaConsumption;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana Properties", meta = (AllowPrivateAccess = "true"))
	float ManaRechargeTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana Properties", meta = (AllowPrivateAccess = "true"))
	float ManaRechargeRate;

	//=================================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mana Runtime", meta = (AllowPrivateAccess = "true"))
	bool bJustFired;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mana Runtime", meta = (AllowPrivateAccess = "true"))
	float ManaRechargeTimeCurrent;

public:
	UFUNCTION(BlueprintCallable)
	float GetManaPercentage() const { return ManaCurrent / ManaBase; }

	UFUNCTION(BlueprintCallable)
	bool HasEnoughMana() const { return ManaCurrent >= ManaConsumption; }
};
