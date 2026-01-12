// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

    check(AttributeInfo);

    for (const auto& Pair : AS->TagsToAttributes)
    {
        FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
        Info.AttributeValue = Pair.Value.Execute(AS).GetCurrentValue();
        AttributeInfoDelegate.Broadcast(Info);
    }
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
    UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
    for (const auto& Pair : AS->TagsToAttributes)
    {
        const FGameplayAttribute* AttributePtr = AS->TagsToGameplayAttributes.Find(Pair.Key);
        if (AttributePtr)
        {
            AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(*AttributePtr).AddLambda([this, Pair, AS](const FOnAttributeChangeData& Data)
            {
                FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
                Info.AttributeValue = Data.NewValue;
                AttributeInfoDelegate.Broadcast(Info);
            });
        }
    }
}