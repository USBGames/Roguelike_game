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
#include "Components/SkeletalMeshComponent.h"

// Sets default values
APsychicCharacter::APsychicCharacter() :
	TeleState(ETeleState::ETS_Idle),
	MaxTeleScanDistance(1000.0f),
	ItemGrabForceAmount(100.0f),
	ItemReleaseForceAmount(1000.0f),
	ItemMinGrabDistance(100.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Set size for Capsule Component
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	//Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCam"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	FPHandsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FPHandsRoot"));
	FPHandsRoot->SetupAttachment(CameraComponent);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FPHandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPHandsMesh"));
	FPHandsMesh->SetOnlyOwnerSee(true);
	FPHandsMesh->SetupAttachment(FPHandsRoot);
	FPHandsMesh->bCastDynamicShadow = false;
	FPHandsMesh->CastShadow = false;
	FPHandsMesh->SetRelativeLocation(FVector(-5.f, 0.f, -170.f));
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

	/*if (bMouseHolding)
	{
		if (TeleState == ETeleState::ETS_Scanning && CurrentItemGrabbed == nullptr)
			ScanForItems();
		else if (TeleState == ETeleState::ETS_Holding || TeleState == ETeleState::ETS_CanFire && CurrentItemGrabbed != nullptr)
			GrabItem();
	}*/

	//DebugLogVal(-1, -1.0, FColor::Cyan, "Current State: ", UEnum::GetValueAsString<ETeleState>(TeleState));
	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, -1.0, FColor::Red, FString::Printf("Current State: %s", TeleState));

	//UE_LOG(LogTemp, Log, TEXT("Current State: %s"), *UEnum::GetValueAsName(TeleState).ToString());
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
		PEI->BindAction(InputActions->InputPrimary, ETriggerEvent::Completed, this, &APsychicCharacter::PrimaryRelease);
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
	else if (TeleState == ETeleState::ETS_Holding || TeleState == ETeleState::ETS_CanFire && CurrentItemGrabbed != nullptr)
		GrabItem();
}

void APsychicCharacter::PrimaryRelease(const FInputActionValue& Value)
{
	//DebugLog(-1, 2.0f, FColor::Blue, "Primary - Released");

	if (TeleState == ETeleState::ETS_Scanning)
		TeleState = ETeleState::ETS_Idle;
	else if (CurrentItemGrabbed != nullptr)
	{
		if (CurrentItemGrabbed->GetItemState() == EItemState::EIT_Grabbed && TeleState == ETeleState::ETS_CanFire)
		{
			ReleaseShootItem();
			TeleState = ETeleState::ETS_Idle;
		}
		else if (CurrentItemGrabbed->GetItemState() == EItemState::EIT_Grabbing && TeleState == ETeleState::ETS_Holding)
		{
			TeleState = ETeleState::ETS_Idle;
			CurrentItemGrabbed->SetItemState(EItemState::EIT_Idle);
			CurrentItemGrabbed = nullptr;
		}
	}
	else
		TeleState = ETeleState::ETS_Idle;
}

FVector APsychicCharacter::GetGrabLocation()
{
	return CameraComponent->GetComponentLocation() + (CameraComponent->GetForwardVector() * 200.0f - CameraComponent->GetUpVector() * 50.0f);
}

void APsychicCharacter::ScanForItems()
{
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

	if (CurrentItemGrabbed->GetItemState() == EItemState::EIT_Grabbing)
	{
		float distance = FVector::Distance(GetGrabLocation(), CurrentItemGrabbed->GetActorLocation());
		if (distance <= ItemMinGrabDistance)
		{
			TeleState = ETeleState::ETS_CanFire;
			CurrentItemGrabbed->SetItemState(EItemState::EIT_Grabbed);
		}
	}
}

void APsychicCharacter::ReleaseShootItem()
{
	CurrentItemGrabbed->SetItemState(EItemState::EIT_Idle);
	CurrentItemGrabbed->GetItemMesh()->AddImpulse(CameraComponent->GetForwardVector() * ItemReleaseForceAmount);
	CurrentItemGrabbed = nullptr;
}