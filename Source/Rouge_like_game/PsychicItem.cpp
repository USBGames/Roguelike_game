// Fill out your copyright notice in the Description page of Project Settings.


#include "PsychicItem.h"

// Sets default values
APsychicItem::APsychicItem() :
	ItemState(EItemState::EIT_Idle)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
}

// Called when the game starts or when spawned
void APsychicItem::BeginPlay()
{
	Super::BeginPlay();
	
	SetItemState(EItemState::EIT_Idle);
}

// Called every frame
void APsychicItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APsychicItem::SetItemProperties()
{
	if (ItemMesh == nullptr) return;

	switch (ItemState)
	{
	case EItemState::EIT_Idle:
		ItemMesh->SetLinearDamping(0.01f);
		ItemMesh->SetAngularDamping(0.0f);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		break;
	case EItemState::EIT_Grabbing:
	case EItemState::EIT_Grabbed:
		ItemMesh->SetLinearDamping(4.0f);
		ItemMesh->SetAngularDamping(4.0f);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(false);
		break;
	}
}