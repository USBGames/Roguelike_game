// Fill out your copyright notice in the Description page of Project Settings.


#include "PsychicCharacter.h"
#include "PsychicDevUtils.h"
#include "PsychicItem.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PsychicInputConfigData.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APsychicCharacter::APsychicCharacter() :
	TeleState(ETeleState::ETS_Idle),
	MaxTeleScanDistance(1000.0f),
	ItemGrabForceAmount(100.0f),
	ItemReleaseForceAmount(1000.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set size for Capsule Component
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	//Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCam"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APsychicCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Setup Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

// Called every frame
void APsychicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DebugLogVal(-1, -1.0, FColor::Cyan, "Current State: ", UEnum::GetValueAsString<ETeleState>(TeleState));
}

// Called to bind functionality to input
void APsychicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Get the EnhancedInputComponent
	if (UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		PEI->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &ACharacter::Jump);
		PEI->BindAction(InputActions->InputJump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &APsychicCharacter::Move);
		PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &APsychicCharacter::Look);

		PEI->BindAction(InputActions->InputPrimary, ETriggerEvent::Started, this, &APsychicCharacter::PrimaryPressed);
		PEI->BindAction(InputActions->InputPrimary, ETriggerEvent::Ongoing, this, &APsychicCharacter::PrimaryHold);
		PEI->BindAction(InputActions->InputPrimary, ETriggerEvent::Canceled, this, &APsychicCharacter::PrimaryRelease);
	}
}

void APsychicCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APsychicCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y);
	}
}

void APsychicCharacter::PrimaryPressed(const FInputActionValue& Value)
{
	//DebugLog(-1, 2.0f, FColor::Red, "Primary - Pressed");

	if (TeleState == ETeleState::ETS_Idle && CurrentItemGrabbed == nullptr)
		TeleState = ETeleState::ETS_Scanning;
}

void APsychicCharacter::PrimaryHold(const FInputActionValue& Value)
{
	//DebugLog(-1, -1.0f, FColor::Green, "Primary - Hold");

	if (TeleState == ETeleState::ETS_Scanning && CurrentItemGrabbed == nullptr)
		ScanForItems();
	else if (TeleState == ETeleState::ETS_Holding && CurrentItemGrabbed != nullptr)
		GrabItem();
}

void APsychicCharacter::PrimaryRelease(const FInputActionValue& Value)
{
	//DebugLog(-1, 2.0f, FColor::Blue, "Primary - Released");

	if (TeleState == ETeleState::ETS_Scanning)
		TeleState = ETeleState::ETS_Idle;
	else if (TeleState == ETeleState::ETS_Holding && CurrentItemGrabbed != nullptr)
	{
		ReleaseShootItem();
		//TODO: Do Timer transition from Firing to Idle State
		TeleState = ETeleState::ETS_Idle;
	}
}

FVector APsychicCharacter::GetGrabLocation()
{
	return CameraComponent->GetComponentLocation() + (CameraComponent->GetForwardVector() * 200.0f - CameraComponent->GetUpVector() * 50.0f);
}

void APsychicCharacter::ScanForItems()
{
	//DebugLog(-1, -1.0f, FColor::Green, "Primary - Scanning Items");

	if (TeleState != ETeleState::ETS_Scanning)
		return;

	const FVector TraceStart = CameraComponent->GetComponentLocation();
	const FVector TraceEnd = CameraComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * MaxTeleScanDistance;

	FHitResult HitResult;
	bool HitSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, HitSuccess ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
	if (HitSuccess)
	{
		CurrentItemGrabbed = Cast<APsychicItem>(HitResult.GetActor());
		if (CurrentItemGrabbed && CurrentItemGrabbed->GetItemState() == EItemState::EIT_Idle)
		{
			TeleState = ETeleState::ETS_Holding;
			CurrentItemGrabbed->SetItemState(EItemState::EIT_Grabbing);
		}
	}
}

void APsychicCharacter::GrabItem()
{
	FVector GrabDir = GetGrabLocation() - CurrentItemGrabbed->GetActorLocation();
	GrabDir.Normalize();

	CurrentItemGrabbed->GetItemMesh()->AddForce(GrabDir * ItemGrabForceAmount);
}

void APsychicCharacter::ReleaseShootItem()
{
	CurrentItemGrabbed->SetItemState(EItemState::EIT_Idle);
	CurrentItemGrabbed->GetItemMesh()->AddImpulse(CameraComponent->GetForwardVector() * ItemReleaseForceAmount);
	CurrentItemGrabbed = nullptr;
}