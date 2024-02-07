// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "PsychicCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UInputMappingContext;
class UPsychicInputConfigData;
class APsychicItem;

UENUM(BlueprintType)
enum class ETeleState : uint8
{
	ETS_Idle UMETA(DisplayName = "Idle"),
	ETS_Scanning UMETA(DisplayName = "Scanning"),
	ETS_Holding UMETA(DisplayName = "Holding"),
	ETS_CanFire UMETA(DisplayName = "Can Fire"),
	ETS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class ROUGE_LIKE_GAME_API APsychicCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APsychicCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Psychic - Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Psychic - Input")
	UPsychicInputConfigData* InputActions;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrimaryPressed(const FInputActionValue& Value);
	void PrimaryHold(const FInputActionValue& Value);
	void PrimaryRelease(const FInputActionValue& Value);

	FVector GetGrabLocation();

	void ScanForItems();
	void GrabItem();
	void ReleaseShootItem();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Psychic - Camera", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FPHandsMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Psychic - Camera", meta = (AllowPrivateAccess = "true"))
	USceneComponent* FPHandsRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Psychic - Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FPOffsetRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Psychic - Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FPCamRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Psychic - Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	ETeleState TeleState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	float MaxTeleScanDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	float ItemGrabForceAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	float ItemReleaseForceAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	float ItemMinGrabDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	APsychicItem* CurrentItemGrabbed;	

public:
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE USceneComponent* GetHandsRoot() const { return FPHandsRoot; }
	FORCEINLINE ETeleState GetTeleState() const { return TeleState; }
};