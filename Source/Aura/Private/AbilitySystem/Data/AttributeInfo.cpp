// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AttributeInfo)
	{
		if (Info.AttributeTag.MatchesTag(AttributeTag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeInfo not found for tag: %s"), *AttributeTag.ToString());
	}
    
	return FAuraAttributeInfo();
}
