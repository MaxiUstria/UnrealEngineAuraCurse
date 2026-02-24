// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);

    EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        if (AbilityClass && AbilityClass->IsChildOf<UAuraGameplayAbility>())
        {
            if (const UAuraGameplayAbility* AuraAbilityCDO = AbilityClass->GetDefaultObject<UAuraGameplayAbility>())
            {
                AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbilityCDO->StartupInputTag);
            }
        }
        GiveAbility(AbilitySpec);
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(FGameplayTag InputTag)
{
    // if (InputTag.IsValid()){
    //     for (auto& AbilitySpec : GetActivatableAbilities()){
    //         if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)){
    //             AbilitySpecInputPressed(AbilitySpec);
    //             if (!AbilitySpec.IsActive()){
    //                 TryActivateAbility(AbilitySpec.Handle);
    //             }
    //         }
    //     }
    // }
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (InputTag.IsValid()){
        for (auto& AbilitySpec : GetActivatableAbilities()){
            if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)){
                AbilitySpecInputPressed(AbilitySpec);
                if (!AbilitySpec.IsActive()){
                    TryActivateAbility(AbilitySpec.Handle);
                }
            }
        }
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (InputTag.IsValid()){
        for (auto& AbilitySpec : GetActivatableAbilities()){
            if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)){
                AbilitySpecInputReleased(AbilitySpec);
            }
        }
    }
}