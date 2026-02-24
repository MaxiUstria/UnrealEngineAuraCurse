// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("InputAction not found for tag: %s"), *InputTag.ToString());
    }
    
    return nullptr;
}

