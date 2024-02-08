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
class UPhysicsConstraintComponent;
class UPsychicManaComponent;
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemShootSignature, APsychicCharacter*, PsychicCharacter);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Psychic - Components", meta = (AllowPrivateAccess = "true"))
	class UPsychicManaComponent* ManaComponent;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrimaryPressed(const FInputActionValue& Value);
	void PrimaryHold(const FInputActionValue& Value);
	void PrimaryRelease(const FInputActionValue& Value);

	FVector GetGrabLocation();

	void ScanForItems();
	void GrabItem();
	void ReleaseShootItem(bool bShoot = false);

	void ItemGrabInterpStart();
	void ItemGrabInterpFinish();

public:
	UPROPERTY(BlueprintAssignable, Category = "Psychic - Events")
	FOnItemShootSignature OnItemShoot;

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

	//===========================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FPGrabHeldSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* FPGrabConstraint;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	float ItemInterpTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	float ItemInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Psychic - Combat", meta = (AllowPrivateAccess = "true"))
	APsychicItem* CurrentItemGrabbed;

	//===========================================

	bool bItemIsInterping;
	FTimerHandle ItemGrabInterpTimerHandler;

public:
	FORCEINLINE UPsychicManaComponent* GetManaComponent() const { return ManaComponent; }

	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE USceneComponent* GetHandsRoot() const { return FPHandsRoot; }
	FORCEINLINE ETeleState GetTeleState() const { return TeleState; }
};
