/*
** actor.h
**
**---------------------------------------------------------------------------
** Copyright 2011 Braden Obrzut
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
**
*/

#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "wl_def.h"
#include "actordef.h"
#include "gamemap.h"
#include "linkedlist.h"
#include "name.h"
#include "dobject.h"
#include "tflags.h"
#include "thinker.h"

enum
{
	AMETA_BASE = 0x12000,

	AMETA_Damage,
	AMETA_DropItems,
	AMETA_SecretDeathSound,
	AMETA_GibHealth,
	AMETA_DefaultHealth1,
	AMETA_DefaultHealth2,
	AMETA_DefaultHealth3,
	AMETA_DefaultHealth4,
	AMETA_DefaultHealth5,
	AMETA_DefaultHealth6,
	AMETA_DefaultHealth7,
	AMETA_DefaultHealth8,
	AMETA_DefaultHealth9,
	AMETA_ConversationID,
	AMETA_DamageResistances,
	AMETA_HaloLights,
	AMETA_ZoneLights,
	AMETA_EnemyFactions,
	AMETA_PickupMessage,
	AMETA_Obituary,			// string (player was killed by this actor)
	AMETA_HitObituary,		// string (player was killed by this actor in melee)
};

enum
{
	SPAWN_AllowReplacement = 1,
	SPAWN_Patrol = 2
};

typedef TFlags<ActorFlag> ActorFlags;
DEFINE_TFLAGS_OPERATORS (ActorFlags)

typedef TFlags<ExtraActorFlag> ExtraActorFlags;
DEFINE_TFLAGS_OPERATORS (ExtraActorFlags)

// Used to affect the logic for thing activation through death, USESPECIAL and BUMPSPECIAL
// "thing" refers to what has the flag and the special, "trigger" refers to what used or bumped it
enum EThingSpecialActivationType
{
	THINGSPEC_Default			= 0,		// Normal behavior: a player must be the trigger, and is the activator
	THINGSPEC_ThingActs			= 1,		// The thing itself is the activator of the special
	THINGSPEC_ThingTargets		= 1<<1,		// The thing changes its target to the trigger
	THINGSPEC_TriggerTargets	= 1<<2,		// The trigger changes its target to the thing
	THINGSPEC_MonsterTrigger	= 1<<3,		// The thing can be triggered by a monster
	THINGSPEC_MissileTrigger	= 1<<4,		// The thing can be triggered by a projectile
	THINGSPEC_ClearSpecial		= 1<<5,		// Clears special after successful activation
	THINGSPEC_NoDeathSpecial	= 1<<6,		// Don't activate special on death
	THINGSPEC_TriggerActs		= 1<<7,		// The trigger is the activator of the special
											// (overrides LEVEL_ACTOWNSPECIAL Hexen hack)
	THINGSPEC_Activate			= 1<<8,		// The thing is activated when triggered
	THINGSPEC_Deactivate		= 1<<9,		// The thing is deactivated when triggered
	THINGSPEC_Switch			= 1<<10,	// The thing is alternatively activated and deactivated when triggered
};

class player_t;
class ClassDef;
class AInventory;
namespace Dialog { struct Page; }
class AActor : public Thinker,
	public EmbeddedList<AActor>::Node
{
	typedef EmbeddedList<AActor>::Node ActorLink;

	DECLARE_CLASS(AActor, Thinker)
	HAS_OBJECT_POINTERS

	public:
		struct DropItem
		{
			public:
				FName			className;
				unsigned int	amount;
				uint8_t			probability;
		};
		typedef LinkedList<DropItem> DropList;

		struct DamageResistance
		{
			public:
				const ClassDef  *damagetype;
				unsigned int	percent;
		};
		typedef LinkedList<DamageResistance> DamageResistanceList;

		struct HaloLight
		{
			public:
				int             id;
				int             light;
				double          radius;
		};
		typedef LinkedList<HaloLight> HaloLightList;

		struct ZoneLight
		{
			public:
				int             id;
				int             light;
		};
		typedef LinkedList<ZoneLight> ZoneLightList;

		struct EnemyFaction
		{
			public:
				const ClassDef  *faction;
		};
		typedef LinkedList<EnemyFaction> EnemyFactionList;

		void			AddInventory(AInventory *item);
		virtual void	BeginPlay() {}
		void			ClearCounters();
		void			ClearInventory();
		virtual void	Destroy();
		virtual void	ObituaryMessage(AActor *attacker) {}
		virtual void	Die();
		void			EnterZone(const MapZone *zone);
		AInventory		*FindInventory(const ClassDef *cls);
		template<class T> T *FindInventory ()
		{
			return static_cast<T *> (FindInventory (RUNTIME_CLASS(T)));
		}
		const Frame		*FindState(const FName &name) const;
		static void		FinishSpawningActors();
		int				GetDamage();
		const AActor	*GetDefault() const;
		DropList		*GetDropList() const;
		DamageResistanceList		*GetDamageResistanceList() const;
		HaloLightList		*GetHaloLightList() const;
		ZoneLightList		*GetZoneLightList() const;
		EnemyFactionList		*GetEnemyFactionList() const;
		const MapZone	*GetZone() const { return soundZone; }
		bool			GiveInventory(const ClassDef *cls, int amount=0, bool allowreplacement=true);
		bool			InStateSequence(const Frame *basestate) const;
		bool			IsFast() const;
		virtual void	PostBeginPlay() {}
		void			RemoveFromWorld();
		virtual void	RemoveInventory(AInventory *item);
		void			Serialize(FArchive &arc);
		void			SetState(const Frame *state, bool norun=false);
		void			SpawnFog();
		static AActor	*Spawn(const ClassDef *type, fixed x, fixed y, fixed z, int flags);
		int32_t			SpawnHealth() const;
		bool			Teleport(fixed x, fixed y, angle_t angle, bool nofog=false);

		virtual void    Activate (AActor *activator);
		virtual void    Deactivate (AActor *activator);

		virtual void	Tick();
		virtual void	Touch(AActor *toucher) {}

		fixed           &GetCoordRef (unsigned int axis);

		void PrintInventory();

		static PointerIndexTable<ExpressionNode> damageExpressions;
		static PointerIndexTable<DropList> dropItems;
		static PointerIndexTable<DamageResistanceList> damageResistances;
		static PointerIndexTable<HaloLightList> haloLights;
		static PointerIndexTable<ZoneLightList> zoneLights;
		static PointerIndexTable<EnemyFactionList> enemyFactions;

		// Spawned actor ID
		unsigned int spawnid;

		// Basic properties from objtype
		ActorFlags flags;

		// rather than use QWORD we have extra flags
		ExtraActorFlags extraflags;

		int32_t	distance; // if negative, wait for that door to open
		dirtype	dir;

#pragma pack(push, 1)
// MSVC and older versions of GCC don't support constant union parts
// We do this instead of just using a regular word since writing to tilex/y
// indicates an error.
#if !defined(_MSC_VER) && (__GNUC__ > 4 || __GNUC_MINOR__ >= 6)
#define COORD_PART const word
#else
#define COORD_PART word
#endif
		union
		{
			fixed x;
#ifdef __BIG_ENDIAN__
			struct { COORD_PART tilex; COORD_PART fracx; };
#else
			struct { COORD_PART fracx; COORD_PART tilex; };
#endif
		};
		union
		{
			fixed y;
#ifdef __BIG_ENDIAN__
			struct { COORD_PART tiley; COORD_PART fracy; };
#else
			struct { COORD_PART fracy; COORD_PART tiley; };
#endif
		};
#pragma pack(pop)
		fixed z;
		fixed	velx, vely;

		angle_t	angle;
		angle_t pitch;
		int32_t	health;
		int32_t	speed, runspeed;
		int		points;
		fixed	radius;
		fixed	projectilepassheight;
		int     loaded;

		const Frame		*state;
		unsigned int	sprite;
		fixed			scaleX, scaleY;
		short			ticcount;

		short       viewx;
		word        viewheight;
		fixed       transx,transy;      // in global coord
		fixed       absx,absy;          // in global coord

		FTextureID	overheadIcon;

		uint16_t	sighttime;
		uint8_t		sightrandom;
		fixed		missilefrequency;
		uint16_t	minmissilechance;
		short		movecount; // Emulation of Doom's movecount
		fixed		meleerange;
		uint16_t	painchance;
		FNameNoInit	activesound, attacksound, deathsound, painsound, seesound;
		FNameNoInit	actionns;

		const Frame *SpawnState, *SeeState, *PathState, *PainState, *MeleeState, *MissileState, *RadiusWakeState;
		short       temp1,hidden;
		fixed		killerx,killery; // For deathcam
		const ClassDef  *killerdamagetype;
		int         picX, picY;
		int         haloLightMask;
		int         zoneLightMask;
		int         singlespawn;

		const ClassDef  *faction;
		std::pair<bool,unsigned int> spawnThingNum;
		int			activationtype;	// How the thing behaves when activated with USESPECIAL or BUMPSPECIAL

		TObjPtr<AActor> target;
		player_t	*player;	// Only valid with APlayerPawn

		TObjPtr<AActor> missileParent;

		TObjPtr<AInventory>	inventory;

		const Dialog::Page *conversation;
		const ClassDef  *damagetype;

		static EmbeddedList<AActor>::List actors;
		typedef EmbeddedList<AActor>::Iterator Iterator;
		static Iterator GetIterator() { return Iterator(actors); }
	protected:
		void	Init();

		const MapZone	*soundZone;
};

// This could easily be a bool but then it'd be much harder to find later. ;)
enum replace_t
{
	NO_REPLACE = 0,
	ALLOW_REPLACE = 1
};

template<class T>
inline T *Spawn (fixed x, fixed y, fixed z, replace_t replace)
{
	return static_cast<T *>(AActor::Spawn (RUNTIME_CLASS(T), x, y, z, replace));
}

// Old save compatibility
// FIXME: Remove for 1.4
class AActorProxy : public Thinker
{
	DECLARE_CLASS(AActorProxy, Thinker)

public:
	void Tick() {}

	void Serialize(FArchive &arc);

	TObjPtr<AActor> actualObject;
};

namespace ActorSpawnID
{
	typedef std::map<unsigned int, AActor *> ActorByKey;
	extern ActorByKey Actors;

	void Serialize(FArchive &arc);
}

#endif
