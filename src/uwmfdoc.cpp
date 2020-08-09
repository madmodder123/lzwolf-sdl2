/*
** uwmfdoc.cpp
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

#include <sstream>
#include "m_swap.h"
#include "uwmfdoc.h"
using namespace UwmfDoc;

GlobalProperties::GlobalProperties() :
	ns("Wolf3D"),
	name("MapName"),
	tileSize(64),
	width(64),
	height(64)
{
}

Tile::Tile() :
	textureEast("-"),
	textureNorth("-"),
	textureSouth("-"),
	textureWest("-"),
	blockingEast(true),
	blockingNorth(true),
	blockingSouth(true),
	blockingWest(true),
	offsetVertical(false),
	offsetHorizontal(false),
	dontOverlay(false),
	mapped(),
	comment(),
	soundSequence(),
	textureOverhead()
{
}

Sector::Sector() :
	textureCeiling("-"),
	textureFloor("-"),
	comment()
{
}

Zone::Zone() :
	comment()
{
}

Plane::Plane() :
	depth(64),
	comment()
{
}

Planemap::Spot::Spot() :
	tileind(-1),
	sectorind(-1),
	zoneind(-1),
	tag(0)
{
}

Planemap::Spot::Spot(int tileind_, int sectorind_, int zoneind_, int tag_) :
	tileind(tileind_),
	sectorind(sectorind_),
	zoneind(zoneind_),
	tag(tag_)
{
}

Thing::Thing() :
	type("unknown"),
	x(0.0),
	y(0.0),
	z(0.0),
	angle(0),
	ambush(false),
	patrol(false),
	skill1(true),
	skill2(true),
	skill3(true),
	skill4(true),
	comment()
{
}

Trigger::Trigger() :
	x(0),
	y(0),
	z(0),
	action(0),
	arg0(0),
	arg1(0),
	arg2(0),
	arg3(0),
	arg4(0),
	activateEast(true),
	activateNorth(true),
	activateSouth(true),
	activateWest(true),
	playerCross(false),
	playerUse(false),
	monsterUse(false),
	repeatable(false),
	secret(false),
	comment()
{
}

namespace UwmfDocEmitter
{
	typedef UwmfDoc::Document Map;

	class Quoted
	{
	public:
		std::string content;

		explicit Quoted(std::string content_) : content(content_)
		{
		}
	};

	class Boolean
	{
	public:
		bool content;

		explicit Boolean(bool content_) : content(content_)
		{
		}
	};

	class PropertySpacer
	{
	public:
		std::string gap;

		static PropertySpacer &getInstance();
	};

	PropertySpacer &PropertySpacer::getInstance()
	{
		static PropertySpacer inst;
		return inst;
	}

	inline std::string &propGap()
	{
		return PropertySpacer::getInstance().gap;
	}

	template <typename T>
	class Property
	{
	public:
		std::string name;
		T v;
		bool hasDefault;
		T noemit;

		Property(std::string name_, T v_) : name(name_), v(v_), hasDefault(false)
		{
		}

		Property(std::string name_, T v_, T noemit_) : name(name_), v(v_), hasDefault(true), noemit(noemit_)
		{
		}
	};

	template <typename T>
	const Property<T> MakeProperty(std::string name, const T &v)
	{
		return Property<T>(name, v);
	}

	template <typename T>
	const Property<T> MakeProperty(std::string name, const T &v, const T &noemit)
	{
		return Property<T>(name, v, noemit);
	}

	std::ostream &operator<<(std::ostream &os, const Quoted &quoted)
	{
		os << '"' << quoted.content << '"';
		return os;
	}

	std::ostream &operator<<(std::ostream &os, const Boolean &boolean)
	{
		os << (boolean.content ? "true" : "false");
		return os;
	}

	template <typename T>
	std::ostream &operator<<(std::ostream &os, const Property<T> &property)
	{
		if (!(property.hasDefault && property.noemit == property.v))
			os << propGap() << property.name << " = " << property.v << ';' << std::endl;
		return os;
	}

	std::ostream &operator<<(std::ostream &os, const Property<std::string> &property)
	{
		if (!(property.hasDefault && property.noemit == property.v))
			os << propGap() << property.name << " = " << Quoted(property.v) << ';' << std::endl;
		return os;
	}

	std::ostream &operator<<(std::ostream &os, const Property<bool> &property)
	{
		if (!(property.hasDefault && property.noemit == property.v))
			os << propGap() << property.name << " = " << Boolean(property.v) << ';' << std::endl;
		return os;
	}

	template <typename T>
	std::ostream &operator<<(std::ostream &os, const Property<UwmfDoc::OptionalProperty<T> > &property)
	{
		if (!(property.hasDefault && property.noemit == property.v))
			os << Property<T>(property.name, property.v.val());
		return os;
	}

	class EmitTile
	{
		std::ostream &os;
		const UwmfDoc::Tile dftTile;

	public:
		explicit EmitTile(std::ostream &os_) : os(os_)
		{
		}

		void operator()(const UwmfDoc::Tile &tile)
		{
			os << "tile" << std::endl;
			os << '{' << std::endl;
			os << MakeProperty("textureEast", tile.textureEast);
			os << MakeProperty("textureNorth", tile.textureNorth);
			os << MakeProperty("textureSouth", tile.textureSouth);
			os << MakeProperty("textureWest", tile.textureWest);
			os << MakeProperty("blockingEast", tile.blockingEast, dftTile.blockingEast);
			os << MakeProperty("blockingNorth", tile.blockingNorth, dftTile.blockingNorth);
			os << MakeProperty("blockingSouth", tile.blockingSouth, dftTile.blockingSouth);
			os << MakeProperty("blockingWest", tile.blockingWest, dftTile.blockingWest);
			os << MakeProperty("offsetVertical", tile.offsetVertical, dftTile.offsetVertical);
			os << MakeProperty("offsetHorizontal", tile.offsetHorizontal, dftTile.offsetHorizontal);
			os << MakeProperty("dontOverlay", tile.dontOverlay, dftTile.dontOverlay);
			os << MakeProperty("mapped", tile.mapped, dftTile.mapped);
			os << MakeProperty("comment", tile.comment, dftTile.comment);
			os << MakeProperty("soundSequence", tile.soundSequence, dftTile.soundSequence);
			os << MakeProperty("textureOverhead", tile.textureOverhead, dftTile.textureOverhead);
			os << '}' << std::endl;
		}
	};

	class EmitSector
	{
		std::ostream &os;
		const UwmfDoc::Sector dftSector;

	public:
		explicit EmitSector(std::ostream &os_) : os(os_)
		{
		}

		void operator()(const UwmfDoc::Sector &sector)
		{
			os << "sector" << std::endl;
			os << '{' << std::endl;
			os << MakeProperty("textureCeiling", sector.textureCeiling);
			os << MakeProperty("textureFloor", sector.textureFloor);
			os << MakeProperty("comment", sector.comment, dftSector.comment);
			os << '}' << std::endl;
		}
	};

	class EmitZone
	{
		std::ostream &os;
		const UwmfDoc::Zone dftZone;

	public:
		explicit EmitZone(std::ostream &os_) : os(os_)
		{
		}

		void operator()(const UwmfDoc::Zone &zone)
		{
			os << "zone" << std::endl;
			os << '{' << std::endl;
			os << MakeProperty("comment", zone.comment, dftZone.comment);
			os << '}' << std::endl;
		}
	};

	class EmitPlane
	{
		std::ostream &os;
		const UwmfDoc::Plane dftPlane;

	public:
		explicit EmitPlane(std::ostream &os_) : os(os_)
		{
		}

		void operator()(const UwmfDoc::Plane &plane)
		{
			os << "plane" << std::endl;
			os << '{' << std::endl;
			os << MakeProperty("depth", plane.depth);
			os << MakeProperty("comment", plane.comment, dftPlane.comment);
			os << '}' << std::endl;
		}
	};

	class EmitSpot
	{
		std::ostream &os;
		unsigned int &nspots;
		unsigned int totalspots;

	public:
		explicit EmitSpot(std::ostream &os_, unsigned int &nspots_,
			unsigned int totalspots_) :
			os(os_), nspots(nspots_), totalspots(totalspots_)
		{
		}

		void operator()(const UwmfDoc::Planemap::Spot &spot)
		{
			os << "    " << '{' << spot.tileind << ',' << spot.sectorind << ','
				<< spot.zoneind;
			if (spot.tag != 0)
				os << ',' << spot.tag;
			os << '}';
			nspots++;
			if (nspots != totalspots)
				os << ',';
			os << std::endl;
		}
	};

	class EmitPlanemap
	{
		std::ostream &os;

	public:
		explicit EmitPlanemap(std::ostream &os_) : os(os_)
		{
		}

		void operator()(const UwmfDoc::Planemap &planemap)
		{
			unsigned nspots = 0;
			EmitSpot emitSpot(os, nspots, planemap.spots.size());
			os << "planeMap" << std::endl;
			os << '{' << std::endl;
			std::for_each(planemap.spots.begin(), planemap.spots.end(), emitSpot);
			os << '}' << std::endl;
		}
	};

	class EmitThing
	{
		std::ostream &os;
		const UwmfDoc::Thing dftThing;

	public:
		explicit EmitThing(std::ostream &os_) : os(os_)
		{
		}

		void operator()(const UwmfDoc::Thing &thing)
		{
			os << "thing" << std::endl;
			os << '{' << std::endl;
			os << MakeProperty("type", thing.type);
			os << MakeProperty("x", thing.x);
			os << MakeProperty("y", thing.y);
			os << MakeProperty("z", thing.z);
			os << MakeProperty("angle", thing.angle, dftThing.angle);
			os << MakeProperty("ambush", thing.ambush, dftThing.ambush);
			os << MakeProperty("patrol", thing.patrol, dftThing.patrol);
			os << MakeProperty("skill1", thing.skill1);
			os << MakeProperty("skill2", thing.skill2);
			os << MakeProperty("skill3", thing.skill3);
			os << MakeProperty("skill4", thing.skill4);
			os << MakeProperty("comment", thing.comment, dftThing.comment);
			os << '}' << std::endl;
		}
	};

	class EmitTrigger
	{
		std::ostream &os;
		const UwmfDoc::Trigger dftTrigger;

	public:
		explicit EmitTrigger(std::ostream &os_) : os(os_)
		{
		}

		void operator()(const UwmfDoc::Trigger &trigger)
		{
			os << "trigger" << std::endl;
			os << '{' << std::endl;
			os << MakeProperty("x", trigger.x);
			os << MakeProperty("y", trigger.y);
			os << MakeProperty("z", trigger.z);
			os << MakeProperty("action", trigger.action);
			os << MakeProperty("arg0", trigger.arg0, dftTrigger.arg0);
			os << MakeProperty("arg1", trigger.arg1, dftTrigger.arg1);
			os << MakeProperty("arg2", trigger.arg2, dftTrigger.arg2);
			os << MakeProperty("arg3", trigger.arg3, dftTrigger.arg3);
			os << MakeProperty("arg4", trigger.arg4, dftTrigger.arg4);
			os << MakeProperty("activateEast", trigger.activateEast, dftTrigger.activateEast);
			os << MakeProperty("activateNorth", trigger.activateNorth, dftTrigger.activateNorth);
			os << MakeProperty("activateSouth", trigger.activateSouth, dftTrigger.activateSouth);
			os << MakeProperty("activateWest", trigger.activateWest, dftTrigger.activateWest);
			os << MakeProperty("playerCross", trigger.playerCross, dftTrigger.playerCross);
			os << MakeProperty("playerUse", trigger.playerUse, dftTrigger.playerUse);
			os << MakeProperty("monsterUse", trigger.monsterUse, dftTrigger.monsterUse);
			os << MakeProperty("repeatable", trigger.repeatable, dftTrigger.repeatable);
			os << MakeProperty("secret", trigger.secret, dftTrigger.secret);
			os << MakeProperty("comment", trigger.comment, dftTrigger.comment);
			os << '}' << std::endl;
		}
	};

	void Emit(const Map &map, std::ostream &os)
	{
		propGap() = "";
		os << MakeProperty("namespace", map.globProp.ns);
		os << MakeProperty("name", map.globProp.name);
		os << MakeProperty("tileSize", map.globProp.tileSize);
		os << MakeProperty("width", map.globProp.width);
		os << MakeProperty("height", map.globProp.height);
		os << std::endl;

		propGap() = "    ";
		EmitTile emitTile(os);
		std::for_each(map.tiles.begin(), map.tiles.end(), emitTile);
		os << std::endl;

		EmitSector emitSector(os);
		std::for_each(map.sectors.begin(), map.sectors.end(), emitSector);
		os << std::endl;

		EmitZone emitZone(os);
		std::for_each(map.zones.begin(), map.zones.end(), emitZone);
		os << std::endl;

		EmitPlane emitPlane(os);
		std::for_each(map.planes.begin(), map.planes.end(), emitPlane);
		os << std::endl;

		EmitPlanemap emitPlanemap(os);
		std::for_each(map.planemaps.begin(), map.planemaps.end(), emitPlanemap);
		os << std::endl;

		EmitThing emitThing(os);
		std::for_each(map.things.begin(), map.things.end(), emitThing);
		os << std::endl;

		EmitTrigger emitTrigger(os);
		std::for_each(map.triggers.begin(), map.triggers.end(), emitTrigger);
	}
}

namespace UwmfToWadWriter
{
#pragma pack(push,1)
	struct Header
	{
		char pwad[4];
		DWORD numentries;
		DWORD diroff;
	};

	struct Dirent
	{
		DWORD lumpoff;
		DWORD lumpsize;
		char name[8];
	};
#pragma pack(pop)

	std::string StringToUpper(std::string str)
	{
		std::for_each(str.begin(), str.end(), ::toupper);
		return str;
	}

	void Write(const Map &map, std::string mapname, std::string wadpath)
	{
		std::stringstream fp_ss;
		fp_ss << wadpath << "/" << mapname << ".wad";
		wadpath = fp_ss.str();
		FILE *fp = fopen(wadpath.c_str(), "wb");
		if (fp != NULL)
		{
			std::stringstream ss;
			UwmfDocEmitter::Emit(map, ss);
			const std::string textmap = ss.str();

			Header hdr;
			memcpy(hdr.pwad, "PWAD", 4);
			hdr.numentries = LittleLong(3);
			hdr.diroff = LittleLong(sizeof(Header) + textmap.size());

			Dirent dirent;
			memset(&dirent, 0, sizeof(dirent));

			fwrite(&hdr, sizeof(Header), 1, fp);
			fwrite(textmap.data(), textmap.size(), 1, fp);

			dirent.lumpoff = LittleLong(sizeof(Header));
			dirent.lumpsize = LittleLong(0);
			memset(dirent.name, '\0', 8);
			snprintf(dirent.name, 8, "%s", StringToUpper(mapname).c_str());
			fwrite(&dirent, sizeof(Dirent), 1, fp);

			dirent.lumpoff = LittleLong(sizeof(Header));
			dirent.lumpsize = LittleLong(textmap.size());
			memset(dirent.name, '\0', 8);
			snprintf(dirent.name, 8, "TEXTMAP");
			fwrite(&dirent, sizeof(Dirent), 1, fp);

			dirent.lumpoff = LittleLong(sizeof(Header) + textmap.size());
			dirent.lumpsize = LittleLong(0);
			memset(dirent.name, '\0', 8);
			snprintf(dirent.name, 8, "ENDMAP");
			fwrite(&dirent, sizeof(Dirent), 1, fp);

			fclose(fp);
		}
	}
}
