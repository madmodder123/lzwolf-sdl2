/*
** uwmfdoc.h
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

#ifndef __UWMF_H__
#define __UWMF_H__

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

namespace UwmfDoc
{
	template <typename T>
	class OptionalProperty
	{
		typename std::pair<bool, T> p;

	public:
		class UndefinedValueException
		{
		public:
		};

		OptionalProperty() { }

		explicit OptionalProperty(const T &x) : p(true, x) { }

		T &val()
		{
			p.first = true;
			return p.second;
		}

		bool defined() const
		{
			return p.first;
		}

		const T &val() const
		{
			if (!defined())
				throw UndefinedValueException();
			return p.second;
		}

		bool operator==(const OptionalProperty &x) const
		{
			return p == x.p;
		}

		typename std::pair<bool, T> &aspair()
		{
			return p;
		}
	};

	class GlobalProperties
	{
	public:
		std::string ns;
		std::string name;
		int tileSize;
		int width;
		int height;

		GlobalProperties();
	};

	class Tile
	{
	public:
		std::string textureEast;
		std::string textureNorth;
		std::string textureSouth;
		std::string textureWest;
		bool blockingEast;
		bool blockingNorth;
		bool blockingSouth;
		bool blockingWest;
		bool offsetVertical;
		bool offsetHorizontal;
		bool dontOverlay;
		OptionalProperty<int> mapped;
		std::string comment;
		OptionalProperty<std::string> soundSequence;
		OptionalProperty<std::string> textureOverhead;

		Tile();
	};

	class Sector
	{
	public:
		std::string textureCeiling;
		std::string textureFloor;
		std::string comment;

		Sector();
	};

	class Zone
	{
	public:
		std::string comment;

		Zone();
	};

	class Plane
	{
	public:
		int depth;
		std::string comment;

		Plane();
	};

	class Planemap
	{
	public:
		class Spot
		{
		public:
			int tileind;
			int sectorind;
			int zoneind;
			int tag;

			Spot();

			Spot(int tileind, int sectorind, int zoneind, int tag);
		};

		std::vector<Spot> spots;
	};

	class Thing
	{
	public:
		std::string type;
		double x;
		double y;
		double z;
		int angle;
		bool ambush;
		bool patrol;
		bool skill1;
		bool skill2;
		bool skill3;
		bool skill4;
		std::string comment;

		Thing();
	};

	class Trigger
	{
	public:
		int x;
		int y;
		int z;
		int action;
		int arg0;
		int arg1;
		int arg2;
		int arg3;
		int arg4;
		bool activateEast;
		bool activateNorth;
		bool activateSouth;
		bool activateWest;
		bool playerCross;
		bool playerUse;
		bool monsterUse;
		bool repeatable;
		bool secret;
		std::string comment;

		Trigger();
	};

	class Document
	{
	public:
		GlobalProperties globProp;
		std::vector<Tile> tiles;
		std::vector<Sector> sectors;
		std::vector<Zone> zones;
		std::vector<Plane> planes;
		std::vector<Planemap> planemaps;
		std::vector<Thing> things;
		std::vector<Trigger> triggers;
	};
}

namespace UwmfToWadWriter
{
	typedef UwmfDoc::Document Map;

	void Write(const Map &map, std::string mapname, std::string wadpath);
}

#endif
