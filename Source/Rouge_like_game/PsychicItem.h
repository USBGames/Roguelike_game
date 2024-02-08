// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PsychicItem.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIT_Idle UMETA(DisplayName = "Idle"),
	EIT_Grabbing UMETA(DisplayName = "Grabbing"),
	EIT_Grabbed UMETA(DisplayName = "Grabbed"),
	EIT_Thrown UMETA(DisplayName = "Thrown"),
	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class ROUGE_LIKE_GAME_API APsychicItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APsychicItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	void SetItemProperties();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychic - Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Psychic - Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

public:
	FORCEINLINE void SetItemState(const EItemState& newState) { ItemState = newState; SetItemProperties(); }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }

	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }
};
