/*
** c_bind.cpp
** Functions for using and maintaining key bindings
**
**---------------------------------------------------------------------------
** Copyright 1998-2006 Randy Heit
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
*/

#include "wl_def.h"
#include "id_in.h"
#include "cmdlib.h"
#include "c_dispatch.h"
#include "c_bind.h"
//#include "hu_stuff.h"
#include "g_mapinfo.h"
//#include "configfile.h"
//#include "i_system.h"
#include "c_event.h"
#include "w_wad.h"
#include "scanner.h"
#include "c_console.h"

#include <SDL.h>
#include <math.h>
#include <stdlib.h>

#define CARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

namespace KeyNames
{
	struct KeySymName
	{
		unsigned int keysym;
		const char *name;
		const char *settingName;
	};

	KeySymName data[] =
	{
		{ SDLx_SCANCODE(BACKSPACE), "backspace" },
		{ SDLx_SCANCODE(TAB), "tab" },
		{ SDLx_SCANCODE(CLEAR), "clear" },
		{ SDLx_SCANCODE(RETURN), "return" },
		{ SDLx_SCANCODE(PAUSE), "pause" },
		{ SDLx_SCANCODE(ESCAPE), "escape" },
		{ SDLx_SCANCODE(SPACE), "space" },
//		{ SDLx_SCANCODE(EXCLAIM), "exclaim" },
//		{ SDLx_SCANCODE(QUOTEDBL), "quotedbl" },
//		{ SDLx_SCANCODE(HASH), "hash" },
//		{ SDLx_SCANCODE(DOLLAR), "dollar" },
//		{ SDLx_SCANCODE(AMPERSAND), "ampersand" },
//		{ SDLx_SCANCODE(QUOTE), "quote" },
//		{ SDLx_SCANCODE(LEFTPAREN), "leftparen" },
//		{ SDLx_SCANCODE(RIGHTPAREN), "rightparen" },
//		{ SDLx_SCANCODE(ASTERISK), "asterisk" },
//		{ SDLx_SCANCODE(PLUS), "plus" },
		{ SDLx_SCANCODE(COMMA), ",", "comma" },
		{ SDLx_SCANCODE(MINUS), "-", "minus" },
		{ SDLx_SCANCODE(PERIOD), ".", "period" },
		{ SDLx_SCANCODE(SLASH), "/", "slash" },
		{ SDLx_SCANCODE(0), "0" },
		{ SDLx_SCANCODE(1), "1" },
		{ SDLx_SCANCODE(2), "2" },
		{ SDLx_SCANCODE(3), "3" },
		{ SDLx_SCANCODE(4), "4" },
		{ SDLx_SCANCODE(5), "5" },
		{ SDLx_SCANCODE(6), "6" },
		{ SDLx_SCANCODE(7), "7" },
		{ SDLx_SCANCODE(8), "8" },
		{ SDLx_SCANCODE(9), "9" },
//		{ SDLx_SCANCODE(COLON), "colon" },
		{ SDLx_SCANCODE(SEMICOLON), ";", "semicolon" },
//		{ SDLx_SCANCODE(LESS), "less" },
		{ SDLx_SCANCODE(EQUALS), "=", "equals" },
//		{ SDLx_SCANCODE(GREATER), "greater" },
//		{ SDLx_SCANCODE(QUESTION), "question" },
//		{ SDLx_SCANCODE(AT), "at" },
		{ SDLx_SCANCODE(LEFTBRACKET), "[", "leftbracket" },
		{ SDLx_SCANCODE(BACKSLASH), "\\", "backslash" },
		{ SDLx_SCANCODE(RIGHTBRACKET), "]", "rightbracket" },
//		{ SDLx_SCANCODE(CARET), "caret" },
//		{ SDLx_SCANCODE(UNDERSCORE), "underscore" },
//		{ SDLx_SCANCODE(BACKQUOTE), "backquote" },
		{ SDLx_SCANCODE(A), "a" },
		{ SDLx_SCANCODE(B), "b" },
		{ SDLx_SCANCODE(C), "c" },
		{ SDLx_SCANCODE(D), "d" },
		{ SDLx_SCANCODE(E), "e" },
		{ SDLx_SCANCODE(F), "f" },
		{ SDLx_SCANCODE(G), "g" },
		{ SDLx_SCANCODE(H), "h" },
		{ SDLx_SCANCODE(I), "i" },
		{ SDLx_SCANCODE(J), "j" },
		{ SDLx_SCANCODE(K), "k" },
		{ SDLx_SCANCODE(L), "l" },
		{ SDLx_SCANCODE(M), "m" },
		{ SDLx_SCANCODE(N), "n" },
		{ SDLx_SCANCODE(O), "o" },
		{ SDLx_SCANCODE(P), "p" },
		{ SDLx_SCANCODE(Q), "q" },
		{ SDLx_SCANCODE(R), "r" },
		{ SDLx_SCANCODE(S), "s" },
		{ SDLx_SCANCODE(T), "t" },
		{ SDLx_SCANCODE(U), "u" },
		{ SDLx_SCANCODE(V), "v" },
		{ SDLx_SCANCODE(W), "w" },
		{ SDLx_SCANCODE(X), "x" },
		{ SDLx_SCANCODE(Y), "y" },
		{ SDLx_SCANCODE(Z), "z" },
		{ SDLx_SCANCODE(DELETE), "delete" },
//		{ SDLx_SCANCODE(KP0), "kp0" },
//		{ SDLx_SCANCODE(KP1), "kp1" },
//		{ SDLx_SCANCODE(KP2), "kp2" },
//		{ SDLx_SCANCODE(KP3), "kp3" },
//		{ SDLx_SCANCODE(KP4), "kp4" },
//		{ SDLx_SCANCODE(KP5), "kp5" },
//		{ SDLx_SCANCODE(KP6), "kp6" },
//		{ SDLx_SCANCODE(KP7), "kp7" },
//		{ SDLx_SCANCODE(KP8), "kp8" },
//		{ SDLx_SCANCODE(KP9), "kp9" },
		{ SDLx_SCANCODE(KP_PERIOD), "kp_period" },
		{ SDLx_SCANCODE(KP_DIVIDE), "kp_divide" },
		{ SDLx_SCANCODE(KP_MULTIPLY), "kp_multiply" },
		{ SDLx_SCANCODE(KP_MINUS), "kp_minus" },
		{ SDLx_SCANCODE(KP_PLUS), "kp_plus" },
		{ SDLx_SCANCODE(KP_ENTER), "kp_enter" },
		{ SDLx_SCANCODE(KP_EQUALS), "kp_equals" },
		{ SDLx_SCANCODE(UP), "up" },
		{ SDLx_SCANCODE(DOWN), "down" },
		{ SDLx_SCANCODE(RIGHT), "right" },
		{ SDLx_SCANCODE(LEFT), "left" },
		{ SDLx_SCANCODE(INSERT), "insert" },
		{ SDLx_SCANCODE(HOME), "home" },
		{ SDLx_SCANCODE(END), "end" },
		{ SDLx_SCANCODE(PAGEUP), "pageup" },
		{ SDLx_SCANCODE(PAGEDOWN), "pagedown" },
		{ SDLx_SCANCODE(F1), "f1" },
		{ SDLx_SCANCODE(F2), "f2" },
		{ SDLx_SCANCODE(F3), "f3" },
		{ SDLx_SCANCODE(F4), "f4" },
		{ SDLx_SCANCODE(F5), "f5" },
		{ SDLx_SCANCODE(F6), "f6" },
		{ SDLx_SCANCODE(F7), "f7" },
		{ SDLx_SCANCODE(F8), "f8" },
		{ SDLx_SCANCODE(F9), "f9" },
		{ SDLx_SCANCODE(F10), "f10" },
		{ SDLx_SCANCODE(F11), "f11" },
		{ SDLx_SCANCODE(F12), "f12" },
		{ SDLx_SCANCODE(F13), "f13" },
		{ SDLx_SCANCODE(F14), "f14" },
		{ SDLx_SCANCODE(F15), "f15" },
//		{ SDLx_SCANCODE(NUMLOCK), "numlock" },
		{ SDLx_SCANCODE(CAPSLOCK), "capslock" },
//		{ SDLx_SCANCODE(SCROLLOCK), "scrollock" },
		{ SDLx_SCANCODE(RSHIFT), "rshift" },
		{ SDLx_SCANCODE(LSHIFT), "lshift" },
		{ SDLx_SCANCODE(RCTRL), "rctrl" },
		{ SDLx_SCANCODE(LCTRL), "lctrl" },
		{ SDLx_SCANCODE(RALT), "ralt" },
		{ SDLx_SCANCODE(LALT), "lalt" },
//		{ SDLx_SCANCODE(RMETA), "rmeta" },
//		{ SDLx_SCANCODE(LMETA), "lmeta" },
//		{ SDLx_SCANCODE(LSUPER), "lsuper" },
//		{ SDLx_SCANCODE(RSUPER), "rsuper" },
//		{ SDLx_SCANCODE(MODE), "mode" },
//		{ SDLx_SCANCODE(COMPOSE), "compose" },
//		{ SDLx_SCANCODE(HELP), "help" },
//		{ SDLx_SCANCODE(PRINT), "print" },
//		{ SDLx_SCANCODE(SYSREQ), "sysreq" },
//		{ SDLx_SCANCODE(BREAK), "break" },
//		{ SDLx_SCANCODE(MENU), "menu" },
//		{ SDLx_SCANCODE(POWER), "power" },
//		{ SDLx_SCANCODE(EURO), "euro" },
//		{ SDLx_SCANCODE(UNDO), "undo" },
	};

	typedef std::map<unsigned int, const char *> Names;
	static Names names;
	static Names settingNames;

	typedef std::map<std::string, unsigned int> KeyDict;
	static KeyDict keyDict;

	void Init()
	{
		for (unsigned int i = 0; i < CARRAY_SIZE(data); i++)
		{
			names.insert(std::make_pair(data[i].keysym, data[i].name));

			const char *settingName = data[i].settingName ?
				data[i].settingName : data[i].name;
			settingNames.insert(std::make_pair(data[i].keysym, settingName));
			keyDict.insert(std::make_pair(settingName, data[i].keysym));
		}
	}

	Names &Get()
	{
		if (names.empty())
			Init();
		return names;
	}

	Names &SettingGet()
	{
		if (settingNames.empty())
			Init();
		return settingNames;
	}

	const KeyDict &GetKeyDict()
	{
		if (keyDict.empty())
			Init();
		return keyDict;
	}
}

FKeyBindings Bindings;
FKeyBindings DoubleBindings;
FKeyBindings AutomapBindings;
FKeyBindings MapeditBindings;

static std::map<unsigned int, unsigned int> DClickTime;
static std::map<unsigned int, BYTE> DClicked;

//=============================================================================
//
//
//
//=============================================================================

static int GetKeyFromName (const char *name)
{
	unsigned int i;

	// Names of the form #xxx are translated to key xxx automatically
	if (name[0] == '#' && name[1] != 0)
	{
		return atoi (name + 1);
	}

	// Otherwise, we scan the KeyNames[] array for a matching name
	KeyNames::Names::const_iterator it;
	for (it = KeyNames::Get().begin(); it != KeyNames::Get().end(); ++it)
	{
		if (!stricmp (it->second, name))
			return it->first;
	}
	return 0;
}

//=============================================================================
//
//
//
//=============================================================================

static int GetConfigKeyFromName (const char *key)
{
	using namespace KeyNames;

	int keynum = 0;
	KeyDict::const_iterator it = GetKeyDict().find( key );
	if( it != GetKeyDict().end() )
	{
		keynum = it->second;
	}
	return keynum;
}

//=============================================================================
//
//
//
//=============================================================================

static const char *KeyName (int key)
{
	static char name[5];

	KeyNames::Names::const_iterator it = KeyNames::Get().find(key);
	if (it != KeyNames::Get().end())
		return it->second;

	mysnprintf (name, countof(name), "#%d", key);
	return name;
}

//=============================================================================
//
//
//
//=============================================================================

void C_NameKeys (char *str, int first, int second)
{
	int c = 0;

	*str = 0;
	if (first)
	{
		c++;
		strcpy (str, KeyName (first));
		if (second)
			strcat (str, " or ");
	}

	if (second)
	{
		c++;
		strcat (str, KeyName (second));
	}

	if (!c)
		*str = '\0';
}

//=============================================================================
//
//
//
//=============================================================================

void FKeyBindings::DoBind (const char *key, const char *bind)
{
	int keynum = GetConfigKeyFromName (key);
	if (keynum != 0)
	{
		SetBind(keynum, bind);
	}
}

//=============================================================================
//
//
//
//=============================================================================

void FKeyBindings::UnbindAll ()
{
	Binds.clear();
}

//=============================================================================
//
//
//
//=============================================================================

void FKeyBindings::UnbindKey(const char *key)
{
	int i;

	if ( (i = GetKeyFromName (key)) )
	{
		SetBind(i, "");
	}
	else
	{
		Printf ("Unknown key \"%s\"\n", key);
		return;
	}
}

//=============================================================================
//
//
//
//=============================================================================

void FKeyBindings::PerformBind(FCommandLine &argv, const char *msg)
{
	int i;

	if (argv.argc() > 1)
	{
		i = GetKeyFromName (argv[1]);
		if (!i)
		{
			Printf ("Unknown key \"%s\"\n", argv[1]);
			return;
		}
		if (argv.argc() == 2)
		{
			Printf ("\"%s\" = \"%s\"\n", argv[1], GetBinding(i).GetChars());
		}
		else
		{
			SetBind(i, argv[2]);
		}
	}
	else
	{
		Printf ("%s:\n", msg);
		
		for (BindMap::const_iterator it = Binds.begin(); it != Binds.end(); ++it)
		{
			unsigned int i = it->first;
			if (!GetBinding(i).IsEmpty())
				Printf ("%s \"%s\"\n", KeyName (i), GetBinding(i).GetChars());
		}
	}
}

//=============================================================================
//
// This function is first called for functions in custom key sections.
// In this case, matchcmd is non-NULL, and only keys bound to that command
// are stored. If a match is found, its binding is set to "\1".
// After all custom key sections are saved, it is called one more for the
// normal Bindings and DoubleBindings sections for this game. In this case
// matchcmd is NULL and all keys will be stored. The config section was not
// previously cleared, so all old bindings are still in place. If the binding
// for a key is empty, the corresponding key in the config is removed as well.
// If a binding is "\1", then the binding itself is cleared, but nothing
// happens to the entry in the config.
//
//=============================================================================

void FKeyBindings::ArchiveBindings(const char *bindingsName, Config *f)
{
	int i;
	char settingName[50];

	KeyNames::Names &settingNames = KeyNames::SettingGet();
	KeyNames::Names::const_iterator it;
	for (it = settingNames.begin(); it != settingNames.end(); ++it)
	{
		snprintf(settingName, sizeof(settingName), "%s_%s", bindingsName,
			it->second);
		f->GetSetting(settingName)->SetValue(GetBinding(it->first));
	}
}

void FKeyBindings::LoadBindings(const char *bindingsName, Config *f)
{
	int i;
	char settingName[50];

	KeyNames::Names &settingNames = KeyNames::SettingGet();
	KeyNames::Names::const_iterator it;
	for (it = settingNames.begin(); it != settingNames.end(); ++it)
	{
		snprintf(settingName, sizeof(settingName), "%s_%s", bindingsName,
			it->second);
		f->CreateSetting(settingName, "");

		SetBind(it->first, f->GetSetting(settingName)->GetString());
	}
}

//=============================================================================
//
//
//
//=============================================================================

int FKeyBindings::GetKeysForCommand (const char *cmd, int *first, int *second)
{
	int c, i;

	*first = *second = c = i = 0;

	BindMap::const_iterator it;
	for (it = Binds.begin(); it != Binds.end() && c < 2; ++it)
	{
		unsigned i = it->first;
		if (!GetBinding(i).IsEmpty() && stricmp (cmd, GetBinding(i)) == 0)
		{
			if (c++ == 0)
				*first = i;
			else
				*second = i;
		}
		i++;
	}
	return c;
}

//=============================================================================
//
//
//
//=============================================================================

void FKeyBindings::UnbindACommand (const char *str)
{
	int i;

	BindMap::const_iterator it;
	for (it = Binds.begin(); it != Binds.end(); ++it)
	{
		unsigned int i = it->first;
		if (!GetBinding(i).IsEmpty() && !stricmp (str, GetBinding(i)))
		{
			SetBind(i, ""); 
		}
	}
}

//=============================================================================
//
//
//
//=============================================================================

void FKeyBindings::DefaultBind(const char *keyname, const char *cmd)
{
	int key = GetKeyFromName (keyname);
	if (key == 0)
	{
		Printf ("Unknown key \"%s\"\n", keyname);
		return;
	}
	if (!GetBinding(key).IsEmpty())
	{ // This key is already bound.
		return;
	}
	BindMap::const_iterator it;
	for (it = Binds.begin(); it != Binds.end(); ++it)
	{
		if (!GetBinding(it->first).IsEmpty() && stricmp (GetBinding(it->first), cmd) == 0)
		{ // This command is already bound to a key.
			return;
		}
	}
	// It is safe to do the bind, so do it.
	SetBind(key, cmd);
}

//=============================================================================
//
//
//
//=============================================================================

void C_UnbindAll ()
{
	Bindings.UnbindAll();
	DoubleBindings.UnbindAll();
	AutomapBindings.UnbindAll();
	MapeditBindings.UnbindAll();
}

CCMD (unbindall)
{
	C_UnbindAll ();
}

//=============================================================================
//
//
//
//=============================================================================

CCMD (unbind)
{
	if (argv.argc() > 1)
	{
		Bindings.UnbindKey(argv[1]);
	}
}

CCMD (undoublebind)
{
	if (argv.argc() > 1)
	{
		DoubleBindings.UnbindKey(argv[1]);
	}
}

CCMD (unmapbind)
{
	if (argv.argc() > 1)
	{
		AutomapBindings.UnbindKey(argv[1]);
	}
}

CCMD (unmebind)
{
	if (argv.argc() > 1)
	{
		MapeditBindings.UnbindKey(argv[1]);
	}
}

//=============================================================================
//
//
//
//=============================================================================

CCMD (bind)
{
	Bindings.PerformBind(argv, "Current key bindings");
}

CCMD (doublebind)
{
	DoubleBindings.PerformBind(argv, "Current key doublebindings");
}

CCMD (mapbind)
{
	AutomapBindings.PerformBind(argv, "Current automap key bindings");
}

CCMD (mebind)
{
	MapeditBindings.PerformBind(argv, "Current mapedit key bindings");
}

//==========================================================================
//
// CCMD defaultbind
//
// Binds a command to a key if that key is not already bound and if
// that command is not already bound to another key.
//
//==========================================================================

CCMD (defaultbind)
{
	if (argv.argc() < 3)
	{
		Printf ("Usage: defaultbind <key> <command>\n");
	}
	else
	{
		Bindings.DefaultBind(argv[1], argv[2]);
	}
}

//=============================================================================
//
//
//
//=============================================================================

CCMD (rebind)
{
	FKeyBindings *bindings;

	if (key == 0)
	{
		Printf ("Rebind cannot be used from the console\n");
		return;
	}

	if (key & KEY_DBLCLICKED)
	{
		bindings = &DoubleBindings;
		key &= KEY_DBLCLICKED-1;
	}
	else
	{
		bindings = &Bindings;
	}

	if (argv.argc() > 1)
	{
		bindings->SetBind(key, argv[1]);
	}
}

//=============================================================================
//
//
//
//=============================================================================

void C_BindDefaults ()
{
	int lump, lastlump = 0;

	while ((lump = Wads.FindLump("DEFBINDS", &lastlump)) != -1)
	{
		Scanner sc(lump);

		while (sc.TokensLeft())
		{
			FKeyBindings *dest = &Bindings;
			int key;

			sc.MustGetToken(TK_Identifier);

			// bind destination is optional and is the same as the console command
			if (sc->str.Compare("bind") == 0)
			{
				sc.MustGetToken(TK_StringConst);
			}
			else if (sc->str.Compare("doublebind") == 0)
			{
				dest = &DoubleBindings;
				sc.MustGetToken(TK_StringConst);
			}
			else if (sc->str.Compare("mapbind") == 0)
			{
				dest = &AutomapBindings;
				sc.MustGetToken(TK_StringConst);
			}
			else if (sc->str.Compare("mebind") == 0)
			{
				dest = &MapeditBindings;
				sc.MustGetToken(TK_StringConst);
			}
			key = GetConfigKeyFromName(sc->str);
			sc.MustGetToken(TK_StringConst);
			dest->SetBind(key, sc->str);
		}
	}
}

CCMD(binddefaults)
{
	C_BindDefaults ();
}

void C_SetDefaultBindings ()
{
	C_UnbindAll ();
	C_BindDefaults ();
}

//=============================================================================
//
//
//
//=============================================================================

bool C_DoKey (event_t *ev, FKeyBindings *binds, FKeyBindings *doublebinds)
{
	FString binding;
	bool dclick;
	int dclickspot;
	BYTE dclickmask;
	unsigned int nowtime;

	if (ConsoleState != c_up || (ev->type != EV_KeyDown && ev->type != EV_KeyUp))
		return false;

	dclickspot = ev->data1 >> 3;
	dclickmask = 1 << (ev->data1 & 7);
	dclick = false;

	// This used level.time which didn't work outside a level.
	nowtime = SDL_GetTicks ();
	if (doublebinds != NULL && DClickTime[ev->data1] > nowtime && ev->type == EV_KeyDown)
	{
		// Key pressed for a double click
		binding = doublebinds->GetBinding(ev->data1);
		DClicked[dclickspot] |= dclickmask;
		dclick = true;
	}
	else
	{
		if (ev->type == EV_KeyDown)
		{ // Key pressed for a normal press
			binding = binds->GetBinding(ev->data1);
			DClickTime[ev->data1] = nowtime + 571;
		}
		else if (doublebinds != NULL && DClicked[dclickspot] & dclickmask)
		{ // Key released from a double click
			binding = doublebinds->GetBinding(ev->data1);
			DClicked[dclickspot] &= ~dclickmask;
			DClickTime[ev->data1] = 0;
			dclick = true;
		}
		else
		{ // Key released from a normal press
			binding = binds->GetBinding(ev->data1);
		}
	}


	if (binding.IsEmpty())
	{
		binding = binds->GetBinding(ev->data1);
		dclick = false;
	}

	if (!binding.IsEmpty()/* && (chatmodeon == 0 || ev->data1 < 256)*/)
	{
		if (ev->type == EV_KeyUp && binding[0] != '+')
		{
			return false;
		}

		char *copy = binding.LockBuffer();

		if (ev->type == EV_KeyUp)
		{
			copy[0] = '-';
		}

		AddCommandString (copy, dclick ? ev->data1 | KEY_DBLCLICKED : ev->data1);
		return true;
	}
	return false;
}

