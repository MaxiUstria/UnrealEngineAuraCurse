// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavigationData.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

    check(AuraContext);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    check(Subsystem);
    Subsystem->AddMappingContext(AuraContext, 0);

    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

    AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

    AuraInputComponent->BindAbilityActions(InputConfig, this, &AAuraPlayerController::AbilityInputTagPressed, &AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const struct FInputActionValue& Value)
{
    const FVector2D InputAxisVector = Value.Get<FVector2D>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (APawn* ControlledPawn = GetPawn<APawn>())
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
    
}

void AAuraPlayerController::CursorTrace()
{
    FHitResult CursorHit;
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;

    LastActor = ThisActor;
    ThisActor = CursorHit.GetActor();

    if (LastActor == nullptr && ThisActor != nullptr){
        ThisActor->HighlightActor();
    }

    if (LastActor != nullptr && ThisActor == nullptr){
        LastActor->UnhighlightActor();
    }

    if (LastActor != nullptr && ThisActor != nullptr && LastActor != ThisActor){
        LastActor->UnhighlightActor();
        ThisActor->HighlightActor();
    }
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    CursorTrace();
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        bTargeting = ThisActor != nullptr;
        bAutoRunning = false;
    }
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagReleased(InputTag);
        }
        return;
    }
    if (bTargeting)
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagReleased(InputTag);
        }
    }
    else
    {
        APawn* ControlledPawn = GetPawn();
        if (FollowTime <= ShortPressThreshold && ControlledPawn)
        {
            UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination);
            if (NavPath && NavPath->IsValid())
            {
                Spline->ClearSplinePoints();
                for (const FVector& PointLoc : NavPath->PathPoints)
                {
                    Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
                    DrawDebugSphere(GetWorld(), PointLoc, 10.f, 10, FColor::Red, false, 5.f);
                }
                bAutoRunning = true;
            }
        }
        FollowTime = 0.0f;
        bTargeting = false;
    }
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagHeld(InputTag);
        }
        return;
    }

    if (bTargeting)
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagHeld(InputTag);
        }
    }
    else
    {
        FollowTime += GetWorld()->GetDeltaSeconds();

        FHitResult Hit;
        if (GetHitResultUnderCursor(ECC_Visibility, false, Hit)){
            CachedDestination = Hit.ImpactPoint;
        }

        if (APawn* ControlledPawn = GetPawn()){
            const FVector Direction = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
            ControlledPawn->AddMovementInput(Direction);
        }
    }
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
    if (AuraAbilitySystemComponent == nullptr)
    {
        AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
    }
    return AuraAbilitySystemComponent;
}