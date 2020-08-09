/*
** mapedit.cpp
**
**---------------------------------------------------------------------------
** Copyright 2020 Linux Wolf
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

#include "wl_def.h"
#include "id_ca.h"
#include "id_in.h"
#include "id_vl.h"
#include "id_vh.h"
#include "mapedit.h"
#include "wl_agent.h"
#include "wl_draw.h"
#include "wl_play.h"
#include "c_dispatch.h"
#include "v_text.h"
#include "thingdef/thingdef.h"
#include "am_map.h"
#include "uwmfdoc.h"
using namespace MapEdit;

CVAR(Bool, me_marker, false, CVAR_ARCHIVE)

GameMapEditor::GameMapEditor() : spot(NULL), armlength(TILEGLOBAL*2)
{
	InitMarkedSector();
}

size_t GameMapEditor::GetTileCount() const
{
	return map->tilePalette.Size();
}

size_t GameMapEditor::GetSectorCount() const
{
	return map->sectorPalette.Size();
}

GameMapEditor::LocType GameMapEditor::GetCurLoc(bool center) const
{
	fixed x, y;
	if(automap == AMA_Normal)
	{
		x = viewx;
		y = viewy;
	}
	else
	{
		x = viewx + FixedMul(armlength,viewcos);
		y = viewy - FixedMul(armlength,viewsin);
	}
	if(center)
	{
		x = (x & ~(TILEGLOBAL-1)) + (TILEGLOBAL/2);
		y = (y & ~(TILEGLOBAL-1)) + (TILEGLOBAL/2);
	}
	return std::make_pair(x, y);
}

MapSpot GameMapEditor::GetCurSpot() const
{
	int tilex,tiley;
	tilex = GetCurLoc().first >> TILESHIFT;
	tiley = GetCurLoc().second >> TILESHIFT;
	return map->IsValidTileCoordinate(tilex, tiley, 0) ?
		map->GetSpot(tilex, tiley, 0) : NULL;
}

MapThing *GameMapEditor::GetCurThing() const
{
	int tilex,tiley,thingx,thingy;
	fixed dx,dy;
	fixed dist,mindist;
	MapThing *minthing;

	LocType loc = GetCurLoc(false);
	tilex = loc.first >> TILESHIFT;
	tiley = loc.second >> TILESHIFT;

	mindist = -1;
	minthing = NULL;

	for(unsigned i = 0; i < map->things.Size(); i++)
	{
		MapThing &mapThing = map->things[i];

		thingx = mapThing.x >> TILESHIFT;
		thingy = mapThing.y >> TILESHIFT;

		if(thingx == tilex && thingy == tiley)
		{
			dx = mapThing.x - loc.first;
			dy = mapThing.y - loc.second;
			dist = FixedMul(dx,dx) + FixedMul(dy,dy);
			if(mindist < 0 || dist < mindist)
			{
				mindist = dist;
				minthing = &mapThing;
			}
		}
	}

	return minthing;
}

MapTrigger *GameMapEditor::GetTrigger(unsigned int ind) const
{
	MapSpot spot = mapeditor->GetCurSpot();
	if (spot != NULL && ind < spot->triggers.Size())
	{
		return &spot->triggers[ind];
	}
	return NULL;
}

void GameMapEditor::InitMarkedSector()
{
	FTextureID defaultMarked = TexMan.GetTexture("#ffff00", FTexture::TEX_Flat);
	markedSector.texture[MapSector::Floor] = defaultMarked;
	markedSector.texture[MapSector::Ceiling] = defaultMarked;
	markedSector.overhead = defaultMarked;
}

void GameMapEditor::ConvertToDoc(const GameMap &map, UwmfDoc::Document &doc)
{
	doc.globProp.ns = "Wolf3D";
	//doc.globProp.name;
	doc.globProp.tileSize = map.header.tileSize;
	doc.globProp.width = map.header.width;
	doc.globProp.height = map.header.height;

	for (unsigned i = 0; i < map.tilePalette.Size(); i++)
	{
		const MapTile &mapTile = map.tilePalette[i];

		UwmfDoc::Tile tile;
		FTexture *tex;

		tex = TexMan(mapTile.texture[MapTile::East]);
		if (tex != NULL)
			tile.textureEast = tex->Name;

		tex = TexMan(mapTile.texture[MapTile::West]);
		if (tex != NULL)
			tile.textureWest = tex->Name;

		tex = TexMan(mapTile.texture[MapTile::South]);
		if (tex != NULL)
			tile.textureSouth = tex->Name;

		tex = TexMan(mapTile.texture[MapTile::North]);
		if (tex != NULL)
			tile.textureNorth = tex->Name;

		tile.blockingEast = mapTile.sideSolid[MapTile::East];
		tile.blockingWest = mapTile.sideSolid[MapTile::West];
		tile.blockingSouth = mapTile.sideSolid[MapTile::South];
		tile.blockingNorth = mapTile.sideSolid[MapTile::North];
		tile.offsetVertical = mapTile.offsetVertical;
		tile.offsetHorizontal = mapTile.offsetHorizontal;
		tile.dontOverlay = mapTile.dontOverlay;
		if (mapTile.mapped != 0)
			tile.mapped.val() = mapTile.mapped;

		if (mapTile.soundSequence.IsValidName() &&
			mapTile.soundSequence != NAME_None)
		{
			tile.soundSequence.val() = mapTile.soundSequence.GetChars();
		}

		tex = TexMan(mapTile.overhead);
		if (tex != NULL)
			tile.textureOverhead.val() = tex->Name;

		doc.tiles.push_back(tile);
	}

	for (unsigned i = 0; i < map.sectorPalette.Size(); i++)
	{
		const MapSector &mapSector = map.sectorPalette[i];

		UwmfDoc::Sector sector;
		FTexture *tex;

		tex = TexMan(mapSector.texture[MapSector::Floor]);
		if (tex != NULL)
			sector.textureFloor = tex->Name;

		tex = TexMan(mapSector.texture[MapSector::Ceiling]);
		if (tex != NULL)
			sector.textureCeiling = tex->Name;

		doc.sectors.push_back(sector);
	}

	for (unsigned i = 0; i < map.zonePalette.Size(); i++)
	{
		const MapZone &mapZone = map.zonePalette[i];

		UwmfDoc::Zone zone;

		doc.zones.push_back(zone);
	}

	for (unsigned z = 0; z < map.planes.Size(); z++)
	{
		const MapPlane &mapPlane = map.planes[z];

		UwmfDoc::Plane plane;
		plane.depth = mapPlane.depth;

		doc.planes.push_back(plane);

		doc.planemaps.push_back(UwmfDoc::Planemap());
		UwmfDoc::Planemap &planemap = doc.planemaps.back();

		unsigned int x,y;
		for (y = 0; y < map.header.height; y++)
		{
			for (x = 0; x < map.header.width; x++)
			{
				MapSpot mapSpot = map.GetSpot(x,y,z);

				UwmfDoc::Planemap::Spot spot;
				if (mapSpot->tile != NULL)
					spot.tileind = mapSpot->tile - &map.tilePalette[0];
				if (mapSpot->sector != NULL)
					spot.sectorind = mapSpot->sector - &map.sectorPalette[0];
				if (mapSpot->zone != NULL)
					spot.zoneind = mapSpot->zone - &map.zonePalette[0];
				spot.tag = mapSpot->tag;

				planemap.spots.push_back(spot);

				for (unsigned k = 0; k < mapSpot->triggers.Size(); k++)
				{
					const MapTrigger &mapTrigger = mapSpot->triggers[k];

					UwmfDoc::Trigger trigger;
					trigger.x = x;
					trigger.y = y;
					trigger.z = z;
					trigger.action = mapTrigger.action;
					trigger.arg0 = mapTrigger.arg[0];
					trigger.arg1 = mapTrigger.arg[1];
					trigger.arg2 = mapTrigger.arg[2];
					trigger.arg3 = mapTrigger.arg[3];
					trigger.arg4 = mapTrigger.arg[4];
					trigger.activateEast = mapTrigger.activate[MapTrigger::East];
					trigger.activateWest = mapTrigger.activate[MapTrigger::West];
					trigger.activateSouth = mapTrigger.activate[MapTrigger::South];
					trigger.activateNorth = mapTrigger.activate[MapTrigger::North];
					trigger.playerUse = mapTrigger.playerUse;
					trigger.playerCross = mapTrigger.playerCross;
					trigger.monsterUse = mapTrigger.monsterUse;
					trigger.repeatable = mapTrigger.repeatable;
					trigger.secret = mapTrigger.isSecret;

					doc.triggers.push_back(trigger);
				}
			}
		}
	}

	for (unsigned i = 0; i < map.things.Size(); i++)
	{
		const MapThing &mapThing = map.things[i];

		UwmfDoc::Thing thing;
		if (mapThing.type.IsValidName())
			thing.type = mapThing.type.GetChars();
		thing.x = static_cast<double>(mapThing.x) / FRACUNIT;
		thing.y = static_cast<double>(mapThing.y) / FRACUNIT;
		thing.z = static_cast<double>(mapThing.z) / FRACUNIT;
		thing.angle = mapThing.angle;
		thing.ambush = mapThing.ambush;
		thing.skill1 = mapThing.skill[0];
		thing.skill2 = mapThing.skill[1];
		thing.skill3 = mapThing.skill[2];
		thing.skill4 = mapThing.skill[3];

		doc.things.push_back(thing);
	}
}

AdjustGameMap::AdjustGameMap() : spot(NULL), tile(NULL), sector(NULL)
{
	if (me_marker)
	{
		spot = mapeditor->GetCurSpot();
		if (spot != NULL)
		{
			tile = spot->tile;
			sector = spot->sector;
			spot->SetTile(NULL);
			spot->sector = &mapeditor->markedSector;
		}
	}
}

AdjustGameMap::~AdjustGameMap()
{
	if (spot)
	{
		spot->SetTile(tile);
		spot->sector = sector;
	}
}

CCMD(spotinfo)
{
	MapSpot spot = mapeditor->GetCurSpot();

	size_t tileind = map->GetTileIndex(spot->tile);
	size_t sectorind = map->GetSectorIndex(spot->sector);
	if (map->GetTile(tileind) != NULL)
		Printf("tile = %lu\n", tileind);
	if (map->GetSector(sectorind) != NULL)
		Printf("sector = %lu\n", sectorind);
}

CCMD(thinginfo)
{
	MapThing *mapThing = mapeditor->GetCurThing();
	if (mapThing == NULL)
	{
		Printf(TEXTCOLOR_RED " Cannot find thing at current spot!\n");
		return;
	}
}

CCMD(spawnactor)
{
	if (argv.argc() < 2)
	{
		Printf("Usage: spawnactor <type>\n");
		return;
	}

	const ClassDef *cls = ClassDef::FindClass(argv[1]);
	if(cls == NULL)
	{
		Printf(TEXTCOLOR_RED " Unknown thing\n");
		return;
	}

	AActor *actor = AActor::Spawn(cls, mapeditor->GetCurLoc().first, mapeditor->GetCurLoc().second, 0, SPAWN_AllowReplacement);
	actor->angle = players[ConsolePlayer].camera->angle;
	actor->dir = nodir;
}

CCMD(savemap)
{
	if (argv.argc() < 3)
	{
		Printf("Usage: savemap <mapname> <wadpath>\n");
		return;
	}

	const char *mapname = argv[1];
	const char *wadpath = argv[2];

	UwmfDoc::Document uwmfdoc;
	mapeditor->ConvertToDoc(*map, uwmfdoc);
	uwmfdoc.globProp.name = mapname;

	UwmfToWadWriter::Write(uwmfdoc, mapname, wadpath);
}

class FCVar_me_tile : public FDynamicCVarAccess<int>
{
public:
	int GetValue() const
	{
		MapSpot spot = mapeditor->GetCurSpot();
		return (spot != NULL && spot->tile != NULL ?
			map->GetTileIndex(spot->tile) : -1);
	}

	bool SetValue(int value) const
	{
		const MapTile *tile = map->GetTile(value);

		MapSpot spot = mapeditor->GetCurSpot();
		if (spot == NULL)
		{
			Printf(TEXTCOLOR_RED " Invalid spot!\n");
			return false;
		}

		spot->SetTile(tile);
		return true;
	}
};

class FCVar_me_sector : public FDynamicCVarAccess<int>
{
public:
	int GetValue() const
	{
		MapSpot spot = mapeditor->GetCurSpot();
		return (spot != NULL && spot->sector != NULL ?
			map->GetSectorIndex(spot->sector) : -1);
	}

	bool SetValue(int value) const
	{
		const MapSector *sector = map->GetSector(value);

		MapSpot spot = mapeditor->GetCurSpot();
		if (spot == NULL)
		{
			Printf(TEXTCOLOR_RED " Invalid spot!\n");
			return false;
		}

		spot->sector = sector;
		return true;
	}
};

class FCVarThingCommon
{
public:
	bool GetCurThingAndActor(MapThing *&mapThing, AActor *&actor) const
	{
		mapThing = mapeditor->GetCurThing();
		if (mapThing == NULL)
		{
			Printf(TEXTCOLOR_RED " Cannot find thing at current spot!\n");
			return false;
		}

		actor = NULL;
		for(AActor::Iterator iter = AActor::GetIterator();iter.Next();)
		{
			AActor *check = iter;
			if (check->spawnThingNum.first &&
				check->spawnThingNum.second == map->GetThingIndex(mapThing))
			{
				actor = check;
				break;
			}
		}

		if(actor == NULL)
		{
			Printf(TEXTCOLOR_RED " No associated actor for thing");
			return false;
		}

		return true;
	};
};

class FCVar_me_thingtype : public FDynamicCVarAccess<const char *>
{
public:
	const char *GetValue() const
	{
		MapThing *mapThing = mapeditor->GetCurThing();
		return (mapThing != NULL && mapThing->type.IsValidName() ?
			mapThing->type.GetChars() : "");
	}

	bool SetValue(const char *value) const
	{
		MapThing *mapThing;
		AActor *actor;
		if (!FCVarThingCommon().GetCurThingAndActor(mapThing, actor))
			return false;

		const ClassDef *cls = ClassDef::FindClass(value);
		if(cls == NULL)
		{
			Printf(TEXTCOLOR_RED " Unknown thing type:\"%s\"\n", value);
			return false;
		}

		actor->Destroy();

		mapThing->type = value;

		actor = AActor::Spawn(cls, mapThing->x, mapThing->y, mapThing->z, SPAWN_AllowReplacement|(mapThing->patrol ? SPAWN_Patrol : 0));
		// This forumla helps us to avoid errors in roundoffs.
		actor->angle = (mapThing->angle/45)*ANGLE_45 + (mapThing->angle%45)*ANGLE_1;
		actor->dir = nodir;
		if(mapThing->ambush)
			actor->flags |= FL_AMBUSH;
		if(mapThing->patrol)
			actor->dir = dirtype(actor->angle/ANGLE_45);
		if(mapThing->holo)
			actor->flags &= ~(FL_SOLID);
		return true;
	}
};

class FCVar_me_thingambush : public FDynamicCVarAccess<bool>
{
public:
	bool GetValue() const
	{
		MapThing *mapThing = mapeditor->GetCurThing();
		return (mapThing != NULL ? mapThing->ambush : false);
	}

	bool SetValue(bool value) const
	{
		MapThing *mapThing;
		AActor *actor;
		if (!FCVarThingCommon().GetCurThingAndActor(mapThing, actor))
			return false;

		mapThing->ambush = value;
		actor->flags &= ~FL_AMBUSH;
		actor->flags |= (value ? FL_AMBUSH : FL_NONE);
		return true;
	}
};

class FCVar_me_thingangle : public FDynamicCVarAccess<int>
{
public:
	int GetValue() const
	{
		MapThing *mapThing = mapeditor->GetCurThing();
		return (mapThing != NULL ? mapThing->angle : 0);
	}

	bool SetValue(int value) const
	{
		MapThing *mapThing;
		AActor *actor;
		if (!FCVarThingCommon().GetCurThingAndActor(mapThing, actor))
			return false;
		MapThing &thing = *mapThing;

		thing.angle = value;
		actor->angle = (thing.angle/45)*ANGLE_45 + (thing.angle%45)*ANGLE_1;
		return true;
	}
};

class FCVar_me_thingpatrol : public FDynamicCVarAccess<bool>
{
public:
	bool GetValue() const
	{
		MapThing *mapThing = mapeditor->GetCurThing();
		return (mapThing != NULL ? mapThing->patrol : false);
	}

	bool SetValue(bool value) const
	{
		MapThing *mapThing;
		AActor *actor;
		if (!FCVarThingCommon().GetCurThingAndActor(mapThing, actor))
			return false;
		MapThing &thing = *mapThing;

		thing.patrol = value;
		if(thing.patrol)
			actor->dir = dirtype(actor->angle/ANGLE_45);
		else
			actor->dir = nodir;
		return true;
	}
};

class FCVar_me_thingholo : public FDynamicCVarAccess<bool>
{
public:
	bool GetValue() const
	{
		MapThing *mapThing = mapeditor->GetCurThing();
		return (mapThing != NULL ? mapThing->holo : false);
	}

	bool SetValue(bool value) const
	{
		MapThing *mapThing;
		AActor *actor;
		if (!FCVarThingCommon().GetCurThingAndActor(mapThing, actor))
			return false;
		MapThing &thing = *mapThing;

		thing.holo = value;
		if(thing.holo)
			actor->flags &= ~(FL_SOLID);
		else
			actor->flags |= FL_SOLID;
		return true;
	}
};

class FCVar_me_thingskill : public FDynamicCVarAccess<int>
{
public:
	int GetValue() const
	{
		MapThing *mapThing = mapeditor->GetCurThing();
		return (mapThing != NULL ?
			(mapThing->skill[3] ? 3 :
				(mapThing->skill[2] ? 2 :
					(mapThing->skill[1] ? 1 : 0)
				)
			) : false);
	}

	bool SetValue(int value) const
	{
		MapThing *mapThing;
		AActor *actor;
		if (!FCVarThingCommon().GetCurThingAndActor(mapThing, actor))
			return false;
		MapThing &thing = *mapThing;

		std::fill(&thing.skill[0], &thing.skill[4], false);
		while (value >= 0 && value < 4)
		{
			thing.skill[value] = true;
			value++;
		}
		return true;
	}
};

class FRefCVarAccess_me_triggerplayeruse : public FRefCVarAccess<bool>
{
public:
	bool &GetElemValueRef(int arrind, bool &valid) const
	{
		MapTrigger *mapTrigger = mapeditor->GetTrigger(arrind);
		if (mapTrigger != NULL)
		{
			valid = true;
			return mapTrigger->playerUse;
		}
		static bool unused = false;
		return unused;
	}
};

class FRefCVarAccess_me_triggerrepeatable : public FRefCVarAccess<bool>
{
public:
	bool &GetElemValueRef(int arrind, bool &valid) const
	{
		MapTrigger *mapTrigger = mapeditor->GetTrigger(arrind);
		if (mapTrigger != NULL)
		{
			valid = true;
			return mapTrigger->repeatable;
		}
		static bool unused = false;
		return unused;
	}
};

DYNAMIC_CVAR (Int, me_tile, 0, CVAR_NOFLAGS)
DYNAMIC_CVAR (Int, me_sector, 0, CVAR_NOFLAGS)
DYNAMIC_CVAR (String, me_thingtype, "", CVAR_NOFLAGS)
DYNAMIC_CVAR (Int, me_thingangle, false, CVAR_NOFLAGS)
DYNAMIC_CVAR (Bool, me_thingambush, false, CVAR_NOFLAGS)
DYNAMIC_CVAR (Bool, me_thingpatrol, false, CVAR_NOFLAGS)
DYNAMIC_CVAR (Bool, me_thingholo, false, CVAR_NOFLAGS)
DYNAMIC_CVAR (Int, me_thingskill, 0, CVAR_NOFLAGS)
REF_CVAR (Bool, me_triggerplayeruse, false, CVAR_NOFLAGS)
REF_CVAR (Bool, me_triggerrepeatable, false, CVAR_NOFLAGS)
