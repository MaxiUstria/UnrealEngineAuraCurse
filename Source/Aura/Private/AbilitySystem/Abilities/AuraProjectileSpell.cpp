// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Actor/AuraProjectile.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile()
{
  const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
   if (!bIsServer) return;

   ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
   if (CombatInterface)
   {
        const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SocketLocation);

        AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
            ProjectileClass,
            SpawnTransform,
            GetOwningActorFromActorInfo(),
            Cast<APawn>(GetOwningActorFromActorInfo()),
             ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );

        //TODO: Give the projectile a gameplay effect spec to apply on hit
        Projectile->FinishSpawning(SpawnTransform);
   }
}