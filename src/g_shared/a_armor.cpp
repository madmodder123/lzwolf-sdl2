#include <assert.h>

#include "a_inventory.h"
#include "templates.h"
#include "farchive.h"
#include "g_mapinfo.h"
#include "thinker.h"
#include "thingdef/thingdef.h"
#include "wl_agent.h"
#include "wl_state.h"


IMPLEMENT_CLASS (Armor)
IMPLEMENT_CLASS (BasicArmor)
IMPLEMENT_CLASS (BasicArmorPickup)
IMPLEMENT_CLASS (BasicArmorBonus)

//===========================================================================
//
// ABasicArmor :: Serialize
//
//===========================================================================

void ABasicArmor::Serialize (FArchive &arc)
{
	Super::Serialize (arc);
	arc << SavePercent << BonusCount << MaxAbsorb << MaxFullAbsorb << AbsorbCount << ArmorType;

	arc << ActualSaveAmount;
}

//===========================================================================
//
// ABasicArmor :: Tick
//
// If BasicArmor is given to the player by means other than a
// BasicArmorPickup, then it may not have an icon set. Fix that here.
//
//===========================================================================

void ABasicArmor::Tick ()
{
	Super::Tick ();
	AbsorbCount = 0;
	if (!icon.isValid())
	{
		FString newicon = gameinfo.ArmorIcon1;

		if (SavePercent >= gameinfo.Armor2Percent && gameinfo.ArmorIcon2.Len() != 0)
			newicon = gameinfo.ArmorIcon2;

		if (newicon[0] != 0)
			icon = TexMan.CheckForTexture (newicon, FTexture::TEX_Any);
	}
}

//===========================================================================
//
// ABasicArmor :: CreateCopy
//
//===========================================================================

AInventory *ABasicArmor::CreateCopy (AActor *other)
{
	// BasicArmor that is in use is stored in the inventory as BasicArmor.
	// BasicArmor that is in reserve is not.
	ABasicArmor *copy = ::Spawn<ABasicArmor>(0, 0, 0, NO_REPLACE);
	copy->SavePercent = SavePercent != 0 ? SavePercent : FRACUNIT/3;
	copy->amount = amount;
	copy->maxamount = maxamount;
	copy->icon = icon;
	copy->BonusCount = BonusCount;
	copy->ArmorType = ArmorType;
	copy->ActualSaveAmount = ActualSaveAmount;
	GoAwayAndDie ();
	return copy;
}

//===========================================================================
//
// ABasicArmor :: HandlePickup
//
//===========================================================================

bool ABasicArmor::HandlePickup (AInventory *item, bool &good)
{
	if (item->GetClass() == RUNTIME_CLASS(ABasicArmor))
	{
		// You shouldn't be picking up BasicArmor anyway.
		return true;
	}
	if (item->IsKindOf(RUNTIME_CLASS(ABasicArmorBonus)) && !(item->itemFlags & IF_IGNORESKILL))
	{
		ABasicArmorBonus *armor = static_cast<ABasicArmorBonus*>(item);

		armor->SaveAmount = FixedMul(armor->SaveAmount, G_SkillProperty(SKILLP_ArmorFactor));
	}
	else if (item->IsKindOf(RUNTIME_CLASS(ABasicArmorPickup)) && !(item->itemFlags & IF_IGNORESKILL))
	{
		ABasicArmorPickup *armor = static_cast<ABasicArmorPickup*>(item);

		armor->SaveAmount = FixedMul(armor->SaveAmount, G_SkillProperty(SKILLP_ArmorFactor));
	}
	if (inventory != NULL)
	{
		return inventory->HandlePickup (item, good);
	}
	return false;
}

//===========================================================================
//
// ABasicArmor :: AbsorbDamage
//
//===========================================================================

void ABasicArmor::AbsorbDamage (int damage, FName damageType, int &newdamage)
{
	int saved;
	const ClassDef *dmgcls = NULL;
	ADamage *damageinv = NULL;
	bool ignoreArmor = false;
	
	dmgcls = ClassDef::FindClassTentative(damageType, NATIVE_CLASS(Damage));
	if (dmgcls)
	{
		damageinv = static_cast<ADamage *>(players[0].mo->FindInventory(dmgcls));
		if (damageinv && damageinv->ignorearmor)
		{
			ignoreArmor = true;
		}
	}

	if (!ignoreArmor)
	{
		int full = MAX(0, MaxFullAbsorb - AbsorbCount);
		if (damage < full)
		{
			saved = damage;
		}
		else
		{
			saved = full + FixedMul (damage - full, SavePercent);
			if (MaxAbsorb > 0 && saved + AbsorbCount > MaxAbsorb) 
			{
				saved = MAX(0,  MaxAbsorb - AbsorbCount);
			}
		}

		if (amount < (unsigned)saved)
		{
			saved = amount;
		}
		newdamage -= saved;
		amount -= saved;
		AbsorbCount += saved;
		if (amount == 0)
		{
			// The armor has become useless
			SavePercent = 0;
			ArmorType = NAME_None; // Not NAME_BasicArmor.
#ifdef NOTYET
			// Now see if the player has some more armor in their inventory
			// and use it if so. As in Strife, the best armor is used up first.
			ABasicArmorPickup *best = NULL;
			AInventory *probe = owner->inventory;
			while (probe != NULL)
			{
				if (probe->IsKindOf (RUNTIME_CLASS(ABasicArmorPickup)))
				{
					ABasicArmorPickup *inInv = static_cast<ABasicArmorPickup*>(probe);
					if (best == NULL || best->SavePercent < inInv->SavePercent)
					{
						best = inInv;
					}
				}
				probe = probe->inventory;
			}
			if (best != NULL)
			{
				owner->UseInventory (best);
			}
#endif
		}
		damage = newdamage;
	}

	if (inventory != NULL)
	{
		inventory->AbsorbDamage (damage, damageType, newdamage);
	}
}

//===========================================================================
//
// ABasicArmorPickup :: Serialize
//
//===========================================================================

void ABasicArmorPickup::Serialize (FArchive &arc)
{
	Super::Serialize (arc);
	arc << SavePercent << SaveAmount << MaxAbsorb << MaxFullAbsorb;
	arc << DropTime;
}

//===========================================================================
//
// ABasicArmorPickup :: CreateCopy
//
//===========================================================================

AInventory *ABasicArmorPickup::CreateCopy (AActor *other)
{
	ABasicArmorPickup *copy = static_cast<ABasicArmorPickup *> (Super::CreateCopy (other));

	if (!(itemFlags & IF_IGNORESKILL))
	{
		SaveAmount = FixedMul(SaveAmount, G_SkillProperty(SKILLP_ArmorFactor));
	}

	copy->SavePercent = SavePercent;
	copy->SaveAmount = SaveAmount;
	copy->MaxAbsorb = MaxAbsorb;
	copy->MaxFullAbsorb = MaxFullAbsorb;

	return copy;
}

//===========================================================================
//
// ABasicArmorPickup :: Use
//
// Either gives you new armor or replaces the armor you already have (if
// the SaveAmount is greater than the amount of armor you own). When the
// item is auto-activated, it will only be activated if its max amount is 0
// or if you have no armor active already.
//
//===========================================================================

bool ABasicArmorPickup::Use ()
{
	ABasicArmor *armor = owner->FindInventory<ABasicArmor> ();

	if (armor == NULL)
	{
		armor = ::Spawn<ABasicArmor> (0,0,0,NO_REPLACE);
		//armor->BecomeItem ();
		owner->AddInventory (armor);
	}
	else
	{
		// If you already have more armor than this item gives you, you can't
		// use it.
		if ((int)armor->amount >= SaveAmount + armor->BonusCount)
		{
			return false;
		}
		// Don't use it if you're picking it up and already have some.
		if (armor->amount > 0 && maxamount > 0)
		{
			return false;
		}
	}
	armor->SavePercent = SavePercent;
	armor->amount = SaveAmount + armor->BonusCount;
	armor->maxamount = SaveAmount;
	armor->icon = icon;
	armor->MaxAbsorb = MaxAbsorb;
	armor->MaxFullAbsorb = MaxFullAbsorb;
	armor->ArmorType = this->GetClass()->GetName();
	armor->ActualSaveAmount = SaveAmount;
	return true;
}

//===========================================================================
//
// ABasicArmorBonus :: Serialize
//
//===========================================================================

void ABasicArmorBonus::Serialize (FArchive &arc)
{
	Super::Serialize (arc);
	arc << SavePercent << SaveAmount << MaxSaveAmount << BonusCount << BonusMax
		 << MaxAbsorb << MaxFullAbsorb;
}

//===========================================================================
//
// ABasicArmorBonus :: CreateCopy
//
//===========================================================================

AInventory *ABasicArmorBonus::CreateCopy (AActor *other)
{
	ABasicArmorBonus *copy = static_cast<ABasicArmorBonus *> (Super::CreateCopy (other));

	if (!(itemFlags & IF_IGNORESKILL))
	{
		SaveAmount = FixedMul(SaveAmount, G_SkillProperty(SKILLP_ArmorFactor));
	}

	copy->SavePercent = SavePercent;
	copy->SaveAmount = SaveAmount;
	copy->MaxSaveAmount = MaxSaveAmount;
	copy->BonusCount = BonusCount;
	copy->BonusMax = BonusMax;
	copy->MaxAbsorb = MaxAbsorb;
	copy->MaxFullAbsorb = MaxFullAbsorb;

	return copy;
}

//===========================================================================
//
// ABasicArmorBonus :: Use
//
// Tries to add to the amount of BasicArmor a player has.
//
//===========================================================================

bool ABasicArmorBonus::Use ()
{
	ABasicArmor *armor = owner->FindInventory<ABasicArmor> ();
	bool result = false;

	if (armor == NULL)
	{
		armor = ::Spawn<ABasicArmor> (0,0,0,NO_REPLACE);
		armor->BecomeItem ();
		armor->amount = 0;
		armor->maxamount = MaxSaveAmount;
		owner->AddInventory (armor);
	}

	if (BonusCount > 0 && armor->BonusCount < BonusMax)
	{
		armor->BonusCount = MIN (armor->BonusCount + BonusCount, BonusMax);
		result = true;
	}

	int saveAmount = MIN (SaveAmount, MaxSaveAmount);

	if (saveAmount <= 0)
	{ // If it can't give you anything, it's as good as used.
		return BonusCount > 0 ? result : true;
	}

	// If you already have more armor than this item can give you, you can't
	// use it.
	if ((int)armor->amount >= MaxSaveAmount + armor->BonusCount)
	{
		return result;
	}

	if (armor->amount <= 0)
	{ // Should never be less than 0, but might as well check anyway
		armor->amount = 0;
		armor->icon = icon;
		armor->SavePercent = SavePercent;
		armor->MaxAbsorb = MaxAbsorb;
		armor->ArmorType = this->GetClass()->GetName();
		armor->MaxFullAbsorb = MaxFullAbsorb;
		armor->ActualSaveAmount = MaxSaveAmount;
	}

	armor->amount = MIN(armor->amount + saveAmount, (unsigned)MaxSaveAmount + armor->BonusCount);
	armor->maxamount = MAX (armor->maxamount, (unsigned)MaxSaveAmount);
	return true;
}
