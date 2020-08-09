/*
** mapedit.h
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

#ifndef __MAPEDIT_H__
#define __MAPEDIT_H__

#include <algorithm>
#include "gamemap.h"
#include "c_cvars.h"
#include "uwmfdoc.h"

EXTERN_CVAR(Bool, me_marker)

namespace MapEdit
{
	class GameMapEditor
	{
	public:
		typedef std::pair<fixed, fixed> LocType;

		MapSpot spot;
		fixed armlength;
		MapSector markedSector;

		GameMapEditor();

		size_t GetTileCount() const;

		size_t GetSectorCount() const;

		LocType GetCurLoc(bool center = true) const;

		MapSpot GetCurSpot() const;

		MapThing *GetCurThing() const;

		MapTrigger *GetTrigger(unsigned int ind) const;

		void InitMarkedSector();

		void ConvertToDoc(const GameMap &map, UwmfDoc::Document &doc);

		static GameMapEditor *GetInstance();
	};

	class AdjustGameMap
	{
	public:
		AdjustGameMap();
		~AdjustGameMap();
	
	private:
		MapSpot spot;
		const MapTile *tile;
		const MapSector *sector;
	};
}

#endif
