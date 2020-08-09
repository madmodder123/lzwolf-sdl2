/*
** a_action.cpp
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

#include "c_cvars.h"
#include "thingdef/thingdef.h"
#include "wl_agent.h"
#include "wl_game.h"
#include "wl_main.h"
#include "wl_play.h"

#include <climits>

// SwitchableDecoration: Activate and Deactivate change state ---------------

class ASwitchableDecoration : public AActor
{
	DECLARE_CLASS (ASwitchableDecoration, AActor)
public:
	void Activate (AActor *activator);
	void Deactivate (AActor *activator);
};

IMPLEMENT_CLASS (SwitchableDecoration)

void ASwitchableDecoration::Activate (AActor *activator)
{
	SetState (FindState(NAME_Active));
}

void ASwitchableDecoration::Deactivate (AActor *activator)
{
	SetState (FindState(NAME_Inactive));
}

// SwitchingDecoration: Only Activate changes state -------------------------

class ASwitchingDecoration : public ASwitchableDecoration
{
	DECLARE_CLASS (ASwitchingDecoration, ASwitchableDecoration)
public:
	void Deactivate (AActor *activator) {}
};

IMPLEMENT_CLASS (SwitchingDecoration)

