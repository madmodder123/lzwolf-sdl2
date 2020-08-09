/*
** c_cvars.cpp
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
#include "config.h"
#include "wl_def.h"
#include "am_map.h"
#include "id_sd.h"
#include "id_in.h"
#include "id_us.h"
#include "templates.h"
#include "wl_agent.h"
#include "wl_main.h"
#include "wl_play.h"
#include "c_console.h"
#include "c_dispatch.h"
#include "c_bind.h"
#include "colormatcher.h"
#include "cmdlib.h"

static bool doWriteConfig = false;

Aspect r_ratio = ASPECT_4_3, vid_aspect = ASPECT_NONE;
bool forcegrabmouse = false;
bool vid_fullscreen = false;
bool vid_vsync = false;
bool quitonescape = false;
fixed movebob = FRACUNIT;

bool alwaysrun;
bool mouseenabled, mouseyaxisdisabled, joystickenabled;
float localDesiredFOV = 90.0f;

#if SDL_VERSION_ATLEAST(1,3,0)
// Convert SDL1 keycode to SDL2 scancode
static const SDL_Scancode SDL2ConversionTable[323] = {
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_BACKSPACE,SDL_SCANCODE_TAB,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_CLEAR,SDL_SCANCODE_RETURN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_PAUSE,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_ESCAPE,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_SPACE,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_APOSTROPHE,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_COMMA,SDL_SCANCODE_MINUS,SDL_SCANCODE_PERIOD,SDL_SCANCODE_SLASH,
	SDL_SCANCODE_0,SDL_SCANCODE_1,SDL_SCANCODE_2,SDL_SCANCODE_3,SDL_SCANCODE_4,SDL_SCANCODE_5,SDL_SCANCODE_6,SDL_SCANCODE_7,
	SDL_SCANCODE_8,SDL_SCANCODE_9,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_SEMICOLON,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_EQUALS,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_LEFTBRACKET,SDL_SCANCODE_BACKSLASH,SDL_SCANCODE_RIGHTBRACKET,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_GRAVE,SDL_SCANCODE_A,SDL_SCANCODE_B,SDL_SCANCODE_C,SDL_SCANCODE_D,SDL_SCANCODE_E,SDL_SCANCODE_F,SDL_SCANCODE_G,
	SDL_SCANCODE_H,SDL_SCANCODE_I,SDL_SCANCODE_J,SDL_SCANCODE_K,SDL_SCANCODE_L,SDL_SCANCODE_M,SDL_SCANCODE_N,SDL_SCANCODE_O,
	SDL_SCANCODE_P,SDL_SCANCODE_Q,SDL_SCANCODE_R,SDL_SCANCODE_S,SDL_SCANCODE_T,SDL_SCANCODE_U,SDL_SCANCODE_V,SDL_SCANCODE_W,
	SDL_SCANCODE_X,SDL_SCANCODE_Y,SDL_SCANCODE_Z,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_DELETE,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_KP_0,SDL_SCANCODE_KP_1,SDL_SCANCODE_KP_2,SDL_SCANCODE_KP_3,SDL_SCANCODE_KP_4,SDL_SCANCODE_KP_5,SDL_SCANCODE_KP_6,SDL_SCANCODE_KP_7,
	SDL_SCANCODE_KP_8,SDL_SCANCODE_KP_9,SDL_SCANCODE_KP_PERIOD,SDL_SCANCODE_KP_DIVIDE,SDL_SCANCODE_KP_MULTIPLY,SDL_SCANCODE_KP_MINUS,SDL_SCANCODE_KP_PLUS,SDL_SCANCODE_KP_ENTER,
	SDL_SCANCODE_KP_EQUALS,SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,SDL_SCANCODE_RIGHT,SDL_SCANCODE_LEFT,SDL_SCANCODE_INSERT,SDL_SCANCODE_HOME,SDL_SCANCODE_END,
	SDL_SCANCODE_PAGEUP,SDL_SCANCODE_PAGEDOWN,SDL_SCANCODE_F1,SDL_SCANCODE_F2,SDL_SCANCODE_F3,SDL_SCANCODE_F4,SDL_SCANCODE_F5,SDL_SCANCODE_F6,
	SDL_SCANCODE_F7,SDL_SCANCODE_F8,SDL_SCANCODE_F9,SDL_SCANCODE_F10,SDL_SCANCODE_F11,SDL_SCANCODE_F12,SDL_SCANCODE_F13,SDL_SCANCODE_F14,
	SDL_SCANCODE_F15,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_NUMLOCKCLEAR,SDL_SCANCODE_CAPSLOCK,SDL_SCANCODE_SCROLLLOCK,SDL_SCANCODE_RSHIFT,
	SDL_SCANCODE_LSHIFT,SDL_SCANCODE_RCTRL,SDL_SCANCODE_LCTRL,SDL_SCANCODE_RALT,SDL_SCANCODE_LALT,SDL_SCANCODE_RGUI,SDL_SCANCODE_LGUI,SDL_SCANCODE_UNKNOWN,
	SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_MODE,SDL_SCANCODE_APPLICATION,SDL_SCANCODE_HELP,SDL_SCANCODE_PRINTSCREEN,SDL_SCANCODE_SYSREQ,SDL_SCANCODE_PAUSE,SDL_SCANCODE_MENU,
	SDL_SCANCODE_POWER,SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNDO,
};

int SDL2Convert(int sc)
{
	if(sc < 0)
		return sc;

	return SDL2ConversionTable[sc];
}

int SDL2Backconvert(int sc)
{
	if(sc < 0)
		return sc;

	for(unsigned int i = 0;i < 323;++i)
	{
		if(SDL2ConversionTable[i] == sc)
			return i;
	}
	return 0;
}
#else
int SDL2Convert(int sc) { return sc; }
int SDL2Backconvert(int sc) { return sc; }
#endif

void FinalReadConfig()
{
	SDMode  sd;
	SMMode  sm;
	SDSMode sds;

	sd = static_cast<SDMode> (config.GetSetting("SoundDevice")->GetInteger());
	sm = static_cast<SMMode> (config.GetSetting("MusicDevice")->GetInteger());
	sds = static_cast<SDSMode> (config.GetSetting("DigitalSoundDevice")->GetInteger());

	if ((sd == sdm_AdLib || sm == smm_AdLib) && !AdLibPresent
			&& !SoundBlasterPresent)
	{
		sd = sdm_PC;
		sm = smm_Off;
	}

	if ((sds == sds_SoundBlaster && !SoundBlasterPresent))
		sds = sds_Off;

	SD_SetMusicMode(sm);
	SD_SetSoundMode(sd);
	SD_SetDigiDevice(sds);

	AM_UpdateFlags();

	doWriteConfig = true;
}

/*
====================
=
= ReadConfig
=
====================
*/

void ReadConfig(void)
{
	int uniScreenWidth = 0, uniScreenHeight = 0;
	SettingsData * sd = NULL;

	config.CreateSetting("ForceGrabMouse", false);
	config.CreateSetting("MouseEnabled", 1);
	config.CreateSetting("JoystickEnabled", true);
	config.CreateSetting("ViewSize", 19);
	config.CreateSetting("MouseXAdjustment", 5);
	config.CreateSetting("MouseYAdjustment", 5);
	config.CreateSetting("PanXAdjustment", 5);
	config.CreateSetting("PanYAdjustment", 5);
	config.CreateSetting("SoundDevice", sdm_AdLib);
	config.CreateSetting("MusicDevice", smm_AdLib);
	config.CreateSetting("DigitalSoundDevice", sds_SoundBlaster);
	config.CreateSetting("AlwaysRun", 0);
	config.CreateSetting("MouseYAxisDisabled", 0);
	config.CreateSetting("SoundVolume", MAX_VOLUME);
	config.CreateSetting("MusicVolume", MAX_VOLUME);
	config.CreateSetting("DigitizedVolume", MAX_VOLUME);
	config.CreateSetting("Vid_FullScreen", false);
	config.CreateSetting("Vid_Aspect", ASPECT_NONE);
	config.CreateSetting("Vid_Vsync", false);
	config.CreateSetting("FullScreenWidth", fullScreenWidth);
	config.CreateSetting("FullScreenHeight", fullScreenHeight);
	config.CreateSetting("WindowedScreenWidth", windowedScreenWidth);
	config.CreateSetting("WindowedScreenHeight", windowedScreenHeight);
	config.CreateSetting("DesiredFOV", localDesiredFOV);
	config.CreateSetting("QuitOnEscape", quitonescape);
	config.CreateSetting("MoveBob", FRACUNIT);
	config.CreateSetting("Gamma", 1.0f);

	char joySettingName[50] = {0};
	char keySettingName[50] = {0};
	char mseSettingName[50] = {0};
	forcegrabmouse = config.GetSetting("ForceGrabMouse")->GetInteger() != 0;
	mouseenabled = config.GetSetting("MouseEnabled")->GetInteger() != 0;
	joystickenabled = config.GetSetting("JoystickEnabled")->GetInteger() != 0;
	for(unsigned int i = 0;controlScheme[i].button != bt_nobutton;i++)
	{
		sprintf(joySettingName, "Joystick_%s", controlScheme[i].name);
		sprintf(keySettingName, "Keybaord_%s", controlScheme[i].name);
		sprintf(mseSettingName, "Mouse_%s", controlScheme[i].name);
		for(unsigned int j = 0;j < 50;j++)
		{
			if(joySettingName[j] == ' ')
				joySettingName[j] = '_';
			if(keySettingName[j] == ' ')
				keySettingName[j] = '_';
			if(mseSettingName[j] == ' ')
				mseSettingName[j] = '_';
		}
		config.CreateSetting(joySettingName, controlScheme[i].joystick);
		config.CreateSetting(keySettingName, SDL2Backconvert(controlScheme[i].keyboard));
		config.CreateSetting(mseSettingName, controlScheme[i].mouse);
		controlScheme[i].joystick = config.GetSetting(joySettingName)->GetInteger();
		controlScheme[i].keyboard = SDL2Convert(config.GetSetting(keySettingName)->GetInteger());
		controlScheme[i].mouse = config.GetSetting(mseSettingName)->GetInteger();
	}
	viewsize = config.GetSetting("ViewSize")->GetInteger();
	mousexadjustment = config.GetSetting("MouseXAdjustment")->GetInteger();
	mouseyadjustment = config.GetSetting("MouseYAdjustment")->GetInteger();
	panxadjustment = config.GetSetting("PanXAdjustment")->GetInteger();
	panyadjustment = config.GetSetting("PanYAdjustment")->GetInteger();
	mouseyaxisdisabled = config.GetSetting("MouseYAxisDisabled")->GetInteger() != 0;
	alwaysrun = config.GetSetting("AlwaysRun")->GetInteger() != 0;
	AdlibVolume = config.GetSetting("SoundVolume")->GetInteger();
	SD_UpdatePCSpeakerVolume();
	MusicVolume = config.GetSetting("MusicVolume")->GetInteger();
	SoundVolume = config.GetSetting("DigitizedVolume")->GetInteger();
	vid_fullscreen = config.GetSetting("Vid_FullScreen")->GetInteger() != 0;
	vid_aspect = static_cast<Aspect>(config.GetSetting("Vid_Aspect")->GetInteger());
	vid_vsync = config.GetSetting("Vid_Vsync")->GetInteger() != 0;
	fullScreenWidth = config.GetSetting("FullScreenWidth")->GetInteger();
	fullScreenHeight = config.GetSetting("FullScreenHeight")->GetInteger();
	windowedScreenWidth = config.GetSetting("WindowedScreenWidth")->GetInteger();
	windowedScreenHeight = config.GetSetting("WindowedScreenHeight")->GetInteger();
	if ((sd = config.GetSetting("ScreenWidth")) != NULL)
	{
		uniScreenWidth = sd->GetInteger();
		config.DeleteSetting("ScreenWidth");
	}

	if ((sd = config.GetSetting("ScreenHeight")) != NULL)
	{
		uniScreenHeight = sd->GetInteger();
		config.DeleteSetting("ScreenHeight");
	}
	localDesiredFOV = clamp<float>(static_cast<const float>(config.GetSetting("DesiredFOV")->GetFloat()), 45.0f, 180.0f);
	quitonescape = config.GetSetting("QuitOnEscape")->GetInteger() != 0;
	movebob = config.GetSetting("MoveBob")->GetInteger();
	screenGamma = static_cast<float>(config.GetSetting("Gamma")->GetFloat());

	char hsName[50];
	char hsScore[50];
	char hsCompleted[50];
	char hsGraphic[50];
	for(unsigned int i = 0;i < MaxScores;i++)
	{
		sprintf(hsName, "HighScore%u_Name", i);
		sprintf(hsScore, "HighScore%u_Score", i);
		sprintf(hsCompleted, "HighScore%u_Completed", i);
		sprintf(hsGraphic, "HighScore%u_Graphic", i);

		config.CreateSetting(hsName, Scores[i].name);
		config.CreateSetting(hsScore, Scores[i].score);
		config.CreateSetting(hsCompleted, Scores[i].completed);
		config.CreateSetting(hsGraphic, Scores[i].graphic);

		strcpy(Scores[i].name, config.GetSetting(hsName)->GetString());
		Scores[i].score = config.GetSetting(hsScore)->GetInteger();
		if(config.GetSetting(hsCompleted)->GetType() == SettingsData::ST_STR)
			Scores[i].completed = config.GetSetting(hsCompleted)->GetString();
		else
			Scores[i].completed.Format("%d", config.GetSetting(hsCompleted)->GetInteger());
		strncpy(Scores[i].graphic, config.GetSetting(hsGraphic)->GetString(), 8);
		Scores[i].graphic[8] = 0;
	}

	Bindings.LoadBindings("Bindings", &config);
	DoubleBindings.LoadBindings("DoubleBindings", &config);
	AutomapBindings.LoadBindings("AutomapBindings", &config);
	MapeditBindings.LoadBindings("MapeditBindings", &config);

	C_LoadCVars(&config, CVAR_ARCHIVE);

	// make sure values are correct
	if (mousexadjustment<0) mousexadjustment = 0;
	else if (mousexadjustment>20) mousexadjustment = 20;

	if (mouseyadjustment<0) mouseyadjustment = 0;
	else if (mouseyadjustment>20) mouseyadjustment = 20;

	if (panxadjustment<0) panxadjustment = 0;
	else if (panxadjustment>20) panxadjustment = 20;

	if (panyadjustment<0) panyadjustment = 0;
	else if (panyadjustment>20) panyadjustment = 20;

	if(viewsize<4) viewsize=4;
	else if(viewsize>21) viewsize=21;

	// Carry over the unified screenWidth/screenHeight from previous versions
	// Overwrite the full*/windowed* variables, because they're (most likely) defaulted anyways
	if(uniScreenWidth != 0)
	{
		fullScreenWidth = uniScreenWidth;
		windowedScreenWidth = uniScreenWidth;
	}

	if(uniScreenHeight != 0)
	{
		fullScreenHeight = uniScreenHeight;
		windowedScreenHeight = uniScreenHeight;
	}

	// Set screenHeight, screenWidth
	if(vid_fullscreen)
	{
		screenHeight = fullScreenHeight;
		screenWidth = fullScreenWidth;
	}
	else
	{
		screenHeight = windowedScreenHeight;
		screenWidth = windowedScreenWidth;
	}

		// Propogate localDesiredFOV to players[0]
	players[0].SetFOV(localDesiredFOV);
}

/*
====================
=
= WriteConfig
=
====================
*/

void WriteConfig(void)
{
	if(!doWriteConfig)
		return;

	char joySettingName[50] = {0};
	char keySettingName[50] = {0};
	char mseSettingName[50] = {0};
	config.GetSetting("ForceGrabMouse")->SetValue(forcegrabmouse);
	config.GetSetting("MouseEnabled")->SetValue(mouseenabled);
	config.GetSetting("JoystickEnabled")->SetValue(joystickenabled);
	for(unsigned int i = 0;controlScheme[i].button != bt_nobutton;i++)
	{
		sprintf(joySettingName, "Joystick_%s", controlScheme[i].name);
		sprintf(keySettingName, "Keybaord_%s", controlScheme[i].name);
		sprintf(mseSettingName, "Mouse_%s", controlScheme[i].name);
		for(unsigned int j = 0;j < 50;j++)
		{
			if(joySettingName[j] == ' ')
				joySettingName[j] = '_';
			if(keySettingName[j] == ' ')
				keySettingName[j] = '_';
			if(mseSettingName[j] == ' ')
				mseSettingName[j] = '_';
		}
		config.GetSetting(joySettingName)->SetValue(controlScheme[i].joystick);
		config.GetSetting(keySettingName)->SetValue(SDL2Backconvert(controlScheme[i].keyboard));
		config.GetSetting(mseSettingName)->SetValue(controlScheme[i].mouse);
	}
	config.GetSetting("ViewSize")->SetValue(viewsize);
	config.GetSetting("MouseXAdjustment")->SetValue(mousexadjustment);
	config.GetSetting("MouseYAdjustment")->SetValue(mouseyadjustment);
	config.GetSetting("PanXAdjustment")->SetValue(panxadjustment);
	config.GetSetting("PanYAdjustment")->SetValue(panyadjustment);
	config.GetSetting("MouseYAxisDisabled")->SetValue(mouseyaxisdisabled);
	config.GetSetting("AlwaysRun")->SetValue(alwaysrun);
	config.GetSetting("SoundDevice")->SetValue(SoundMode);
	config.GetSetting("MusicDevice")->SetValue(MusicMode);
	config.GetSetting("DigitalSoundDevice")->SetValue(DigiMode);
	config.GetSetting("SoundVolume")->SetValue(AdlibVolume);
	config.GetSetting("MusicVolume")->SetValue(MusicVolume);
	config.GetSetting("DigitizedVolume")->SetValue(SoundVolume);
	config.GetSetting("Vid_FullScreen")->SetValue(vid_fullscreen);
	config.GetSetting("Vid_Aspect")->SetValue(vid_aspect);
	config.GetSetting("Vid_Vsync")->SetValue(vid_vsync);
	config.GetSetting("FullScreenWidth")->SetValue(fullScreenWidth);
	config.GetSetting("FullScreenHeight")->SetValue(fullScreenHeight);
	config.GetSetting("WindowedScreenWidth")->SetValue(windowedScreenWidth);
	config.GetSetting("WindowedScreenHeight")->SetValue(windowedScreenHeight);
	config.GetSetting("DesiredFOV")->SetValue(localDesiredFOV);
	config.GetSetting("QuitOnEscape")->SetValue(quitonescape);
	config.GetSetting("MoveBob")->SetValue(movebob);
	config.GetSetting("Gamma")->SetValue(screenGamma);

	char hsName[50];
	char hsScore[50];
	char hsCompleted[50];
	char hsGraphic[50];
	for(unsigned int i = 0;i < MaxScores;i++)
	{
		sprintf(hsName, "HighScore%u_Name", i);
		sprintf(hsScore, "HighScore%u_Score", i);
		sprintf(hsCompleted, "HighScore%u_Completed", i);
		sprintf(hsGraphic, "HighScore%u_Graphic", i);

		config.GetSetting(hsName)->SetValue(Scores[i].name);
		config.GetSetting(hsScore)->SetValue(Scores[i].score);
		config.GetSetting(hsCompleted)->SetValue(Scores[i].completed);
		config.GetSetting(hsGraphic)->SetValue(Scores[i].graphic);
	}

	Bindings.ArchiveBindings ("Bindings", &config);
	DoubleBindings.ArchiveBindings ("DoubleBindings", &config);
	AutomapBindings.ArchiveBindings ("AutomapBindings", &config);
	MapeditBindings.ArchiveBindings ("MapeditBindings", &config);

	C_ArchiveCVars(&config, CVAR_ARCHIVE);

	config.SaveConfig();
}

/*
==========================================================

CVARS (console variables)

==========================================================
*/

struct FLatchedValue
{
	FBaseCVar *Variable;
	UCVarValue Value;
	ECVarType Type;
};

static TArray<FLatchedValue> LatchedValues;

bool FBaseCVar::m_DoNoSet = false;
bool FBaseCVar::m_UseCallback = false;

FBaseCVar *CVars = NULL;

int cvar_defflags;

FBaseCVar::FBaseCVar (const FBaseCVar &var)
{
	I_FatalError ("Use of cvar copy constructor");
}

FBaseCVar::FBaseCVar (const char *var_name, DWORD flags, void (*callback)(FBaseCVar &))
{
	FBaseCVar *var;

	var = FindCVar (var_name, NULL);

	m_Callback = callback;
	Flags = 0;
	Name = NULL;
	ArrIndex = 0;

	if (var_name)
	{
		C_AddTabCommand (var_name);
		Name = copystring (var_name);
		m_Next = CVars;
		CVars = this;
	}

	if (var)
	{
		ECVarType type;
		UCVarValue value;

		value = var->GetFavoriteRep (&type);
		ForceSet (value, type);

		if (var->Flags & CVAR_AUTO)
			delete var;
		else
			var->~FBaseCVar();

		Flags = flags;
	}
	else
	{
		Flags = flags | CVAR_ISDEFAULT;
	}
}

FBaseCVar::~FBaseCVar ()
{
	if (Name)
	{
		FBaseCVar *var, *prev;

		var = FindCVar (Name, &prev);

		if (var == this)
		{
			if (prev)
				prev->m_Next = m_Next;
			else
				CVars = m_Next;
		}
		C_RemoveTabCommand(Name);
		delete[] Name;
	}
}

void FBaseCVar::ForceSet (UCVarValue value, ECVarType type, bool nouserinfosend)
{
	DoSet (value, type);
	//if ((Flags & CVAR_USERINFO) && !nouserinfosend && !(Flags & CVAR_IGNORE))
	//	D_UserInfoChanged (this);
	if (m_UseCallback)
		Callback ();

	Flags &= ~CVAR_ISDEFAULT;
}

void FBaseCVar::SetGenericRep (UCVarValue value, ECVarType type)
{
	if ((Flags & CVAR_NOSET) && m_DoNoSet)
	{
		return;
	}
	else if ((Flags & CVAR_LATCH) && playstate != ex_fullconsole && playstate != ex_startup)
	{
		FLatchedValue latch;

		latch.Variable = this;
		latch.Type = type;
		if (type != CVAR_String)
			latch.Value = value;
		else
			latch.Value.String = copystring(value.String);
		LatchedValues.Push (latch);
	}
	else if ((Flags & CVAR_SERVERINFO) && playstate != ex_startup && !demoplayback)
	{
		//if (netgame && !players[consoleplayer].settings_controller)
		//{
		//	Printf ("Only setting controllers can change %s\n", Name);
		//	return;
		//}
		//D_SendServerInfoChange (this, value, type);
	}
	else
	{
		ForceSet (value, type);
	}
}

bool FBaseCVar::ToBool (UCVarValue value, ECVarType type)
{
	switch (type)
	{
	case CVAR_Bool:
		return value.Bool;

	case CVAR_Int:
		return !!value.Int;

	case CVAR_Float:
		return value.Float != 0.f;

	case CVAR_String:
		if (stricmp (value.String, "true") == 0)
			return true;
		else if (stricmp (value.String, "false") == 0)
			return false;
		else
			return !!strtol (value.String, NULL, 0);

	//case CVAR_GUID:
	//	return false;

	default:
		return false;
	}
}

int FBaseCVar::ToInt (UCVarValue value, ECVarType type)
{
	int res;
#if __GNUC__ <= 2
	float tmp;
#endif

	switch (type)
	{
	case CVAR_Bool:			res = (int)value.Bool; break;
	case CVAR_Int:			res = value.Int; break;
#if __GNUC__ <= 2
	case CVAR_Float:		tmp = value.Float; res = (int)tmp; break;
#else
	case CVAR_Float:		res = (int)value.Float; break;
#endif
	case CVAR_String:		
		{
			if (stricmp (value.String, "true") == 0)
				res = 1;
			else if (stricmp (value.String, "false") == 0)
				res = 0;
			else
				res = strtol (value.String, NULL, 0); 
			break;
		}
	//case CVAR_GUID:			res = 0; break;
	default:				res = 0; break;
	}
	return res;
}

float FBaseCVar::ToFloat (UCVarValue value, ECVarType type)
{
	switch (type)
	{
	case CVAR_Bool:
		return (float)value.Bool;

	case CVAR_Int:
		return (float)value.Int;

	case CVAR_Float:
		return value.Float;

	case CVAR_String:
		return (float)strtod (value.String, NULL);

	//case CVAR_GUID:
	//	return 0.f;

	default:
		return 0.f;
	}
}

static char cstrbuf[40];
static GUID cGUID;
static char truestr[] = "true";
static char falsestr[] = "false";

const char *FBaseCVar::ToString (UCVarValue value, ECVarType type)
{
	switch (type)
	{
	case CVAR_Bool:
		return value.Bool ? truestr : falsestr;

	case CVAR_String:
		return value.String;

	case CVAR_Int:
		mysnprintf (cstrbuf, countof(cstrbuf), "%i", value.Int);
		break;

	case CVAR_Float:
		mysnprintf (cstrbuf, countof(cstrbuf), "%g", value.Float);
		break;

	//case CVAR_GUID:
	//	FormatGUID (cstrbuf, countof(cstrbuf), *value.pGUID);
	//	break;

	default:
		strcpy (cstrbuf, "<huh?>");
		break;
	}
	return cstrbuf;
}

//const GUID *FBaseCVar::ToGUID (UCVarValue value, ECVarType type)
//{
//	UCVarValue trans;
//
//	switch (type)
//	{
//	case CVAR_String:
//		trans = FromString (value.String, CVAR_GUID);
//		return trans.pGUID;
//
//	case CVAR_GUID:
//		return value.pGUID;
//
//	default:
//		return NULL;
//	}
//}

UCVarValue FBaseCVar::FromBool (bool value, ECVarType type)
{
	UCVarValue ret;

	switch (type)
	{
	case CVAR_Bool:
		ret.Bool = value;
		break;

	case CVAR_Int:
		ret.Int = value;
		break;

	case CVAR_Float:
		ret.Float = value;
		break;

	case CVAR_String:
		ret.String = value ? truestr : falsestr;
		break;

	//case CVAR_GUID:
	//	ret.pGUID = NULL;
	//	break;

	default:
		break;
	}

	return ret;
}

UCVarValue FBaseCVar::FromInt (int value, ECVarType type)
{
	UCVarValue ret;

	switch (type)
	{
	case CVAR_Bool:
		ret.Bool = value != 0;
		break;

	case CVAR_Int:
		ret.Int = value;
		break;

	case CVAR_Float:
		ret.Float = (float)value;
		break;

	case CVAR_String:
		mysnprintf (cstrbuf, countof(cstrbuf), "%i", value);
		ret.String = cstrbuf;
		break;

	//case CVAR_GUID:
	//	ret.pGUID = NULL;
	//	break;

	default:
		break;
	}

	return ret;
}

UCVarValue FBaseCVar::FromFloat (float value, ECVarType type)
{
	UCVarValue ret;

	switch (type)
	{
	case CVAR_Bool:
		ret.Bool = value != 0.f;
		break;

	case CVAR_Int:
		ret.Int = (int)value;
		break;

	case CVAR_Float:
		ret.Float = value;
		break;

	case CVAR_String:
		mysnprintf (cstrbuf, countof(cstrbuf), "%g", value);
		ret.String = cstrbuf;
		break;

	//case CVAR_GUID:
	//	ret.pGUID = NULL;
	//	break;

	default:
		break;
	}

	return ret;
}

static BYTE HexToByte (const char *hex)
{
	BYTE v = 0;
	for (int i = 0; i < 2; ++i)
	{
		v <<= 4;
		if (hex[i] >= '0' && hex[i] <= '9')
		{
			v += hex[i] - '0';
		}
		else if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			v += hex[i] - 'A';
		}
		else // The string is already verified to contain valid hexits
		{
			v += hex[i] - 'a';
		}
	}
	return v;
}

UCVarValue FBaseCVar::FromString (const char *value, ECVarType type)
{
	UCVarValue ret;
	int i;

	switch (type)
	{
	case CVAR_Bool:
		if (stricmp (value, "true") == 0)
			ret.Bool = true;
		else if (stricmp (value, "false") == 0)
			ret.Bool = false;
		else
			ret.Bool = strtol (value, NULL, 0) != 0;
		break;

	case CVAR_Int:
		if (stricmp (value, "true") == 0)
			ret.Int = 1;
		else if (stricmp (value, "false") == 0)
			ret.Int = 0;
		else
			ret.Int = strtol (value, NULL, 0);
		break;

	case CVAR_Float:
		ret.Float = (float)strtod (value, NULL);
		break;

	case CVAR_String:
		ret.String = const_cast<char *>(value);
		break;

	//case CVAR_GUID:
	//	// {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
	//	// 01234567890123456789012345678901234567
	//	// 0         1         2         3

	//	ret.pGUID = NULL;
	//	if (value == NULL)
	//	{
	//		break;
	//	}
	//	for (i = 0; value[i] != 0 && i < 38; i++)
	//	{
	//		switch (i)
	//		{
	//		case 0:
	//			if (value[i] != '{')
	//				break;
	//		case 9:
	//		case 14:
	//		case 19:
	//		case 24:
	//			if (value[i] != '-')
	//				break;
	//		case 37:
	//			if (value[i] != '}')
	//				break;
	//		default:
	//			if (value[i] < '0' || 
	//				(value[i] > '9' && value[i] < 'A') || 
	//				(value[i] > 'F' && value[i] < 'a') || 
	//				value[i] > 'f')
	//				break;
	//		}
	//	}
	//	if (i == 38 && value[i] == 0)
	//	{
	//		cGUID.Data1 = strtoul (value + 1, NULL, 16);
	//		cGUID.Data2 = (WORD)strtoul (value + 10, NULL, 16);
	//		cGUID.Data3 = (WORD)strtoul (value + 15, NULL, 16);
	//		cGUID.Data4[0] = HexToByte (value + 20);
	//		cGUID.Data4[1] = HexToByte (value + 22);
	//		cGUID.Data4[2] = HexToByte (value + 25);
	//		cGUID.Data4[3] = HexToByte (value + 27);
	//		cGUID.Data4[4] = HexToByte (value + 29);
	//		cGUID.Data4[5] = HexToByte (value + 31);
	//		cGUID.Data4[6] = HexToByte (value + 33);
	//		cGUID.Data4[7] = HexToByte (value + 35);
	//		ret.pGUID = &cGUID;
	//	}
	//	break;

	default:
		break;
	}

	return ret;
}

//UCVarValue FBaseCVar::FromGUID (const GUID &guid, ECVarType type)
//{
//	UCVarValue ret;
//
//	switch (type)
//	{
//	case CVAR_Bool:
//		ret.Bool = false;
//		break;
//
//	case CVAR_Int:
//		ret.Int = 0;
//		break;
//
//	case CVAR_Float:
//		ret.Float = 0.f;
//		break;
//
//	case CVAR_String:
//		ret.pGUID = &guid;
//		ret.String = ToString (ret, CVAR_GUID);
//		break;
//
//	case CVAR_GUID:
//		ret.pGUID = &guid;
//		break;
//
//	default:
//		break;
//	}
//
//	return ret;
//}
FBaseCVar *cvar_set (const char *var_name, const char *val)
{
	FBaseCVar *var;

	if ( (var = FindCVar (var_name, NULL)) )
	{
		UCVarValue value;
		value.String = const_cast<char *>(val);
		var->SetGenericRep (value, CVAR_String);
	}

	return var;
}

FBaseCVar *cvar_forceset (const char *var_name, const char *val)
{
	FBaseCVar *var;
	UCVarValue vval;

	if ( (var = FindCVar (var_name, NULL)) )
	{
		vval.String = const_cast<char *>(val);
		var->ForceSet (vval, CVAR_String);
	}

	return var;
}

void FBaseCVar::EnableNoSet ()
{
	m_DoNoSet = true;
}

void FBaseCVar::EnableCallbacks ()
{
	m_UseCallback = true;
	FBaseCVar *cvar = CVars;

	while (cvar)
	{
		if (!(cvar->Flags & CVAR_NOINITCALL))
		{
			cvar->Callback ();
		}
		cvar = cvar->m_Next;
	}
}

void FBaseCVar::DisableCallbacks ()
{
	m_UseCallback = false;
}

//
// Boolean cvar implementation
//

FBoolCVar::FBoolCVar (const char *name, bool def, DWORD flags, void (*callback)(FBoolCVar &), DynamicAccessorType *dynamicAccessor)
: FBaseCVar (name, flags, reinterpret_cast<void (*)(FBaseCVar &)>(callback)), DynamicAccessor(dynamicAccessor)
{
	DefaultValue = def;
	if (Flags & CVAR_ISDEFAULT)
		Value = def;
}

ECVarType FBoolCVar::GetRealType () const
{
	return CVAR_Bool;
}

UCVarValue FBoolCVar::GetGenericRep (ECVarType type) const
{
	return FromBool (ReadValue(), type);
}

UCVarValue FBoolCVar::GetFavoriteRep (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Bool;
	ret.Bool = ReadValue();
	return ret;
}

UCVarValue FBoolCVar::GetGenericRepDefault (ECVarType type) const
{
	return FromBool (DefaultValue, type);
}

UCVarValue FBoolCVar::GetFavoriteRepDefault (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Bool;
	ret.Bool = DefaultValue;
	return ret;
}

void FBoolCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	DefaultValue = ToBool (value, type);
	if (Flags & CVAR_ISDEFAULT)
	{
		SetGenericRep (value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void FBoolCVar::DoSet (UCVarValue value, ECVarType type)
{
	WriteValue(ToBool (value, type));
}

//
// Integer cvar implementation
//

FIntCVar::FIntCVar (const char *name, int def, DWORD flags, void (*callback)(FIntCVar &), DynamicAccessorType *dynamicAccessor)
: FBaseCVar (name, flags, reinterpret_cast<void (*)(FBaseCVar &)>(callback)), DynamicAccessor(dynamicAccessor)
{
	DefaultValue = def;
	if ((Flags & CVAR_ISDEFAULT) && dynamicAccessor == NULL)
		WriteValue(def);
}

ECVarType FIntCVar::GetRealType () const
{
	return CVAR_Int;
}

UCVarValue FIntCVar::GetGenericRep (ECVarType type) const
{
	return FromInt (ReadValue(), type);
}

UCVarValue FIntCVar::GetFavoriteRep (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Int;
	ret.Int = ReadValue();
	return ret;
}

UCVarValue FIntCVar::GetGenericRepDefault (ECVarType type) const
{
	return FromInt (DefaultValue, type);
}

UCVarValue FIntCVar::GetFavoriteRepDefault (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Int;
	ret.Int = DefaultValue;
	return ret;
}

void FIntCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	DefaultValue = ToInt (value, type);
	if (Flags & CVAR_ISDEFAULT)
	{
		SetGenericRep (value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void FIntCVar::DoSet (UCVarValue value, ECVarType type)
{
	WriteValue(ToInt (value, type));
}

//
// Floating point cvar implementation
//

FFloatCVar::FFloatCVar (const char *name, float def, DWORD flags, void (*callback)(FFloatCVar &))
: FBaseCVar (name, flags, reinterpret_cast<void (*)(FBaseCVar &)>(callback))
{
	DefaultValue = def;
	if (Flags & CVAR_ISDEFAULT)
		Value = def;
}

ECVarType FFloatCVar::GetRealType () const
{
	return CVAR_Float;
}

UCVarValue FFloatCVar::GetGenericRep (ECVarType type) const
{
	return FromFloat (Value, type);
}

UCVarValue FFloatCVar::GetFavoriteRep (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Float;
	ret.Float = Value;
	return ret;
}

UCVarValue FFloatCVar::GetGenericRepDefault (ECVarType type) const
{
	return FromFloat (DefaultValue, type);
}

UCVarValue FFloatCVar::GetFavoriteRepDefault (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Float;
	ret.Float = DefaultValue;
	return ret;
}

void FFloatCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	DefaultValue = ToFloat (value, type);
	if (Flags & CVAR_ISDEFAULT)
	{
		SetGenericRep (value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void FFloatCVar::DoSet (UCVarValue value, ECVarType type)
{
	Value = ToFloat (value, type);
}

//
// String cvar implementation
//

FStringCVar::FStringCVar (const char *name, const char *def, DWORD flags, void (*callback)(FStringCVar &), DynamicAccessorType *dynamicAccessor)
: FBaseCVar (name, flags, reinterpret_cast<void (*)(FBaseCVar &)>(callback)), DynamicAccessor(dynamicAccessor)
{
	DefaultValue = copystring (def);
	if (Flags & CVAR_ISDEFAULT)
		Value = copystring (def);
	else
		Value = NULL;
}

FStringCVar::~FStringCVar ()
{
	if (Value != NULL)
	{
		delete[] Value;
	}
	delete[] DefaultValue;
}

ECVarType FStringCVar::GetRealType () const
{
	return CVAR_String;
}

UCVarValue FStringCVar::GetGenericRep (ECVarType type) const
{
	return FromString (ReadValue(), type);
}

UCVarValue FStringCVar::GetFavoriteRep (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_String;
	ret.String = ReadValue();
	return ret;
}

UCVarValue FStringCVar::GetGenericRepDefault (ECVarType type) const
{
	return FromString (DefaultValue, type);
}

UCVarValue FStringCVar::GetFavoriteRepDefault (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_String;
	ret.String = DefaultValue;
	return ret;
}

void FStringCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	ReplaceString(DefaultValue, ToString(value, type));
	if (Flags & CVAR_ISDEFAULT)
	{
		SetGenericRep (value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void FStringCVar::DoSet (UCVarValue value, ECVarType type)
{
	ReplaceString (Value, ToString (value, type));
	if (DynamicAccessor)
		DynamicAccessor->SetElemValue(ArrIndex, Value);
}

//
// Color cvar implementation
//

FColorCVar::FColorCVar (const char *name, int def, DWORD flags, void (*callback)(FColorCVar &))
: FIntCVar (name, def, flags, reinterpret_cast<void (*)(FIntCVar &)>(callback))
{
}

ECVarType FColorCVar::GetRealType () const
{
	return CVAR_Color;
}

UCVarValue FColorCVar::GetGenericRep (ECVarType type) const
{
	return FromInt2 (Value, type);
}

UCVarValue FColorCVar::GetGenericRepDefault (ECVarType type) const
{
	return FromInt2 (DefaultValue, type);
}

void FColorCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	DefaultValue = ToInt2 (value, type);
	if (Flags & CVAR_ISDEFAULT)
	{
		SetGenericRep (value, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

void FColorCVar::DoSet (UCVarValue value, ECVarType type)
{
	Value = ToInt2 (value, type);
	if (screen)
		Index = ColorMatcher.Pick (RPART(Value), GPART(Value), BPART(Value));
}

UCVarValue FColorCVar::FromInt2 (int value, ECVarType type)
{
	if (type == CVAR_String)
	{
		UCVarValue ret;
		mysnprintf (cstrbuf, countof(cstrbuf), "%02x %02x %02x",
			RPART(value), GPART(value), BPART(value));
		ret.String = cstrbuf;
		return ret;
	}
	return FromInt (value, type);
}

int FColorCVar::ToInt2 (UCVarValue value, ECVarType type)
{
	int ret;

	if (type == CVAR_String)
	{
		FString string;
		// Only allow named colors after the screen exists (i.e. after
		// we've got some lumps loaded, so X11R6RGB can be read). Since
		// the only time this might be called before that is when loading
		// zdoom.ini, this shouldn't be a problem.
		if (screen && !(string = V_GetColorStringByName (value.String)).IsEmpty() )
		{
			ret = V_GetColorFromString (NULL, string);
		}
		else
		{
			ret = V_GetColorFromString (NULL, value.String);
		}
	}
	else
	{
		ret = ToInt (value, type);
	}
	return ret;
}

//
// GUID cvar implementation
//

#ifdef NOTYET
FGUIDCVar::FGUIDCVar (const char *name, const GUID *def, DWORD flags, void (*callback)(FGUIDCVar &))
: FBaseCVar (name, flags, reinterpret_cast<void (*)(FBaseCVar &)>(callback))
{
	if (def != NULL)
	{
		DefaultValue = *def;
		if (Flags & CVAR_ISDEFAULT)
			Value = *def;
	}
	else
	{
		memset (&Value, 0, sizeof(DefaultValue));
		memset (&DefaultValue, 0, sizeof(DefaultValue));
	}
}

ECVarType FGUIDCVar::GetRealType () const
{
	return CVAR_GUID;
}

UCVarValue FGUIDCVar::GetGenericRep (ECVarType type) const
{
	return FromGUID (Value, type);
}

UCVarValue FGUIDCVar::GetFavoriteRep (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_GUID;
	ret.pGUID = &Value;
	return ret;
}

UCVarValue FGUIDCVar::GetGenericRepDefault (ECVarType type) const
{
	return FromGUID (DefaultValue, type);
}

UCVarValue FGUIDCVar::GetFavoriteRepDefault (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_GUID;
	ret.pGUID = &DefaultValue;
	return ret;
}

void FGUIDCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	const GUID *guid = ToGUID (value, type);
	if (guid != NULL)
	{
		Value = *guid;
		if (Flags & CVAR_ISDEFAULT)
		{
			SetGenericRep (value, type);
			Flags |= CVAR_ISDEFAULT;
		}
	}
}

void FGUIDCVar::DoSet (UCVarValue value, ECVarType type)
{
	const GUID *guid = ToGUID (value, type);
	if (guid != NULL)
	{
		Value = *guid;
	}
}
#endif

//
// More base cvar stuff
//

void FBaseCVar::ResetColors ()
{
	FBaseCVar *var = CVars;

	while (var)
	{
		if (var->GetRealType () == CVAR_Color)
		{
			var->DoSet (var->GetGenericRep (CVAR_Int), CVAR_Int);
		}
		var = var->m_Next;
	}
}

void FBaseCVar::ResetToDefault ()
{
	if (!(Flags & CVAR_ISDEFAULT))
	{
		UCVarValue val;
		ECVarType type;

		val = GetFavoriteRepDefault (&type);
		SetGenericRep (val, type);
		Flags |= CVAR_ISDEFAULT;
	}
}

//
// Flag cvar implementation
//
// This type of cvar is not a "real" cvar. Instead, it gets and sets
// the value of a FIntCVar, modifying it bit-by-bit. As such, it has
// no default, and is not written to the .cfg or transferred around
// the network. The "host" cvar is responsible for that.
//

FFlagCVar::FFlagCVar (const char *name, FIntCVar &realvar, DWORD bitval)
: FBaseCVar (name, 0, NULL),
ValueVar (realvar),
BitVal (bitval)
{
	int bit;

	Flags &= ~CVAR_ISDEFAULT;

	assert (bitval != 0);

	bit = 0;
	while ((bitval >>= 1) != 0)
	{
		++bit;
	}
	BitNum = bit;

	assert ((1u << BitNum) == BitVal);
}

ECVarType FFlagCVar::GetRealType () const
{
	return CVAR_Dummy;
}

UCVarValue FFlagCVar::GetGenericRep (ECVarType type) const
{
	return FromBool ((ValueVar & BitVal) != 0, type);
}

UCVarValue FFlagCVar::GetFavoriteRep (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Bool;
	ret.Bool = (ValueVar & BitVal) != 0;
	return ret;
}

UCVarValue FFlagCVar::GetGenericRepDefault (ECVarType type) const
{
	ECVarType dummy;
	UCVarValue def;
	def = ValueVar.GetFavoriteRepDefault (&dummy);
	return FromBool ((def.Int & BitVal) != 0, type);
}

UCVarValue FFlagCVar::GetFavoriteRepDefault (ECVarType *type) const
{
	ECVarType dummy;
	UCVarValue def;
	def = ValueVar.GetFavoriteRepDefault (&dummy);
	def.Bool = (def.Int & BitVal) != 0;
	*type = CVAR_Bool;
	return def;
}

void FFlagCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	bool newdef = ToBool (value, type);
	ECVarType dummy;
	UCVarValue def;
	def = ValueVar.GetFavoriteRepDefault (&dummy);
	if (newdef)
		def.Int |= BitVal;
	else
		def.Int &= ~BitVal;
	ValueVar.SetGenericRepDefault (def, CVAR_Int);
}

void FFlagCVar::DoSet (UCVarValue value, ECVarType type)
{
	bool newval = ToBool (value, type);

	// Server cvars that get changed by this need to use a special message, because
	// changes are not processed until the next net update. This is a problem with
	// exec scripts because all flags will base their changes off of the value of
	// the "master" cvar at the time the script was run, overriding any changes
	// another flag might have made to the same cvar earlier in the script.
	if ((ValueVar.GetFlags() & CVAR_SERVERINFO) && playstate != ex_startup && !demoplayback)
	{
		//if (netgame && !players[consoleplayer].settings_controller)
		//{
		//	Printf ("Only setting controllers can change %s\n", Name);
		//	return;
		//}
		//D_SendServerFlagChange (&ValueVar, BitNum, newval);
	}
	else
	{
		int val = *ValueVar;
		if (newval)
			val |= BitVal;
		else
			val &= ~BitVal;
		ValueVar = val;
	}
}

//
// Mask cvar implementation
//
// Similar to FFlagCVar but can have multiple bits
//

FMaskCVar::FMaskCVar (const char *name, FIntCVar &realvar, DWORD bitval)
: FBaseCVar (name, 0, NULL),
ValueVar (realvar),
BitVal (bitval)
{
	int bit;

	Flags &= ~CVAR_ISDEFAULT;

	assert (bitval != 0);

	bit = 0;
	while ((bitval & 1) == 0)
	{
		++bit;
		bitval >>= 1;
	}
	BitNum = bit;
}

ECVarType FMaskCVar::GetRealType () const
{
	return CVAR_Dummy;
}

UCVarValue FMaskCVar::GetGenericRep (ECVarType type) const
{
	return FromInt ((ValueVar & BitVal) >> BitNum, type);
}

UCVarValue FMaskCVar::GetFavoriteRep (ECVarType *type) const
{
	UCVarValue ret;
	*type = CVAR_Int;
	ret.Int = (ValueVar & BitVal) >> BitNum;
	return ret;
}

UCVarValue FMaskCVar::GetGenericRepDefault (ECVarType type) const
{
	ECVarType dummy;
	UCVarValue def;
	def = ValueVar.GetFavoriteRepDefault (&dummy);
	return FromInt ((def.Int & BitVal) >> BitNum, type);
}

UCVarValue FMaskCVar::GetFavoriteRepDefault (ECVarType *type) const
{
	ECVarType dummy;
	UCVarValue def;
	def = ValueVar.GetFavoriteRepDefault (&dummy);
	def.Int = (def.Int & BitVal) >> BitNum;
	*type = CVAR_Int;
	return def;
}

void FMaskCVar::SetGenericRepDefault (UCVarValue value, ECVarType type)
{
	int val = ToInt(value, type) << BitNum;
	ECVarType dummy;
	UCVarValue def;
	def = ValueVar.GetFavoriteRepDefault (&dummy);
	def.Int &= ~BitVal;
	def.Int |= val;
	ValueVar.SetGenericRepDefault (def, CVAR_Int);
}

void FMaskCVar::DoSet (UCVarValue value, ECVarType type)
{
	int val = ToInt(value, type) << BitNum;

	// Server cvars that get changed by this need to use a special message, because
	// changes are not processed until the next net update. This is a problem with
	// exec scripts because all flags will base their changes off of the value of
	// the "master" cvar at the time the script was run, overriding any changes
	// another flag might have made to the same cvar earlier in the script.
	if ((ValueVar.GetFlags() & CVAR_SERVERINFO) && playstate != ex_startup && !demoplayback)
	{
		//if (netgame && !players[consoleplayer].settings_controller)
		//{
		//	Printf ("Only setting controllers can change %s\n", Name);
		//	return;
		//}
		//// Ugh...
		//for(int i = 0; i < 32; i++)
		//{
		//	if (BitVal & (1<<i))
		//	{
		//		D_SendServerFlagChange (&ValueVar, i, !!(val & (1<<i)));
		//	}
		//}
	}
	else
	{
		int vval = *ValueVar;
		vval &= ~BitVal;
		vval |= val;
		ValueVar = vval;
	}
}


////////////////////////////////////////////////////////////////////////
static int STACK_ARGS sortcvars (const void *a, const void *b)
{
	return strcmp (((*(FBaseCVar **)a))->GetName(), ((*(FBaseCVar **)b))->GetName());
}

void FilterCompactCVars (TArray<FBaseCVar *> &cvars, DWORD filter)
{
	// Accumulate all cvars that match the filter flags.
	for (FBaseCVar *cvar = CVars; cvar != NULL; cvar = cvar->m_Next)
	{
		if ((cvar->Flags & filter) && !(cvar->Flags & CVAR_IGNORE))
			cvars.Push(cvar);
	}
	// Now sort them, so they're in a deterministic order and not whatever
	// order the linker put them in.
	if (cvars.Size() > 0)
	{
		qsort(&cvars[0], cvars.Size(), sizeof(FBaseCVar *), sortcvars);
	}
}

void C_WriteCVars (BYTE **demo_p, DWORD filter, bool compact)
{
	FString dump = C_GetMassCVarString(filter, compact);
	size_t dumplen = dump.Len() + 1;	// include terminating \0
	memcpy(*demo_p, dump.GetChars(), dumplen);
	*demo_p += dumplen;
}

FString C_GetMassCVarString (DWORD filter, bool compact)
{
	FBaseCVar *cvar;
	FString dump;

	if (compact)
	{
		TArray<FBaseCVar *> cvars;
		dump.AppendFormat("\\\\%ux", filter);
		FilterCompactCVars(cvars, filter);
		while (cvars.Pop (cvar))
		{
			UCVarValue val = cvar->GetGenericRep(CVAR_String);
			dump << '\\' << val.String;
		}
	}
	else
	{
		for (cvar = CVars; cvar != NULL; cvar = cvar->m_Next)
		{
			if ((cvar->Flags & filter) && !(cvar->Flags & (CVAR_NOSAVE|CVAR_IGNORE)))
			{
				UCVarValue val = cvar->GetGenericRep(CVAR_String);
				dump << '\\' << cvar->GetName() << '\\' << val.String;
			}
		}
	}
	return dump;
}

void C_ReadCVars (BYTE **demo_p)
{
	char *ptr = *((char **)demo_p);
	char *breakpt;

	if (*ptr++ != '\\')
		return;

	if (*ptr == '\\')
	{       // compact mode
		TArray<FBaseCVar *> cvars;
		FBaseCVar *cvar;
		DWORD filter;

		ptr++;
		breakpt = strchr (ptr, '\\');
		*breakpt = 0;
		filter = strtoul (ptr, NULL, 16);
		*breakpt = '\\';
		ptr = breakpt + 1;

		FilterCompactCVars (cvars, filter);

		while (cvars.Pop (cvar))
		{
			UCVarValue val;
			breakpt = strchr (ptr, '\\');
			if (breakpt)
				*breakpt = 0;
			val.String = ptr;
			cvar->ForceSet (val, CVAR_String);
			if (breakpt)
			{
				*breakpt = '\\';
				ptr = breakpt + 1;
			}
			else
				break;
		}
	}
	else
	{
		char *value;

		while ( (breakpt = strchr (ptr, '\\')) )
		{
			*breakpt = 0;
			value = breakpt + 1;
			if ( (breakpt = strchr (value, '\\')) )
				*breakpt = 0;

			cvar_set (ptr, value);

			*(value - 1) = '\\';
			if (breakpt)
			{
				*breakpt = '\\';
				ptr = breakpt + 1;
			}
			else
			{
				break;
			}
		}
	}
	*demo_p += strlen (*((char **)demo_p)) + 1;
}

struct FCVarBackup
{
	FString Name, String;
};
static TArray<FCVarBackup> CVarBackups;

void C_BackupCVars (void)
{
	assert(CVarBackups.Size() == 0);
	CVarBackups.Clear();

	FCVarBackup backup;

	for (FBaseCVar *cvar = CVars; cvar != NULL; cvar = cvar->m_Next)
	{
		if ((cvar->Flags & (CVAR_SERVERINFO|CVAR_DEMOSAVE)) && !(cvar->Flags & CVAR_LATCH))
		{
			backup.Name = cvar->GetName();
			backup.String = cvar->GetGenericRep(CVAR_String).String;
			CVarBackups.Push(backup);
		}
	}
}

void C_RestoreCVars (void)
{
	for (unsigned int i = 0; i < CVarBackups.Size(); ++i)
	{
		cvar_set(CVarBackups[i].Name, CVarBackups[i].String);
	}
	C_ForgetCVars();
}

void C_ForgetCVars (void)
{
	CVarBackups.Clear();
}

FBaseCVar *FindCVar (const char *var_name, FBaseCVar **prev)
{
	FBaseCVar *var;
	FBaseCVar *dummy;

	if (var_name == NULL)
		return NULL;

	if (prev == NULL)
		prev = &dummy;

	var = CVars;
	*prev = NULL;
	while (var)
	{
		if (stricmp (var->GetName (), var_name) == 0)
			break;
		*prev = var;
		var = var->m_Next;
	}
	return var;
}

int ExtractArrIndex(const char *var_name, int namelen, int &arrind)
{
	const char *s1 = strchr(var_name, '[');
	const char *s2 = strchr(var_name, ']');
	if (s1 != NULL && s2 != NULL && s2 > s1 && s1 - var_name < namelen &&
		s2 - var_name < namelen)
	{
		namelen = s1 - var_name;

		char *sub = new char[(s2 - s1) + 1];
		memcpy(sub, s1+1, s2 - s1);
		sub[s2-s1] = '\0';
		arrind = atoi(sub);
		delete[] sub;
	}

	return namelen;
}

FBaseCVar *FindCVarSub (const char *var_name, int namelen)
{
	FBaseCVar *var;

	if (var_name == NULL)
		return NULL;
	
	int arrind = 0;
	namelen = ExtractArrIndex(var_name, namelen, arrind);

	var = CVars;
	while (var)
	{
		const char *probename = var->GetName ();

		if (strnicmp (probename, var_name, namelen) == 0 &&
			probename[namelen] == 0)
		{
			var->ArrIndex = arrind;
			break;
		}
		var = var->m_Next;
	}
	return var;
}

//===========================================================================
//
// C_CreateCVar
//
// Create a new cvar with the specified name and type. It should not already
// exist.
//
//===========================================================================

FBaseCVar *C_CreateCVar(const char *var_name, ECVarType var_type, DWORD flags)
{
	assert(FindCVar(var_name, NULL) == NULL);
	flags |= CVAR_AUTO;
	switch (var_type)
	{
	case CVAR_Bool:		return new FBoolCVar(var_name, 0, flags);
	case CVAR_Int:		return new FIntCVar(var_name, 0, flags);
	case CVAR_Float:	return new FFloatCVar(var_name, 0, flags);
	case CVAR_String:	return new FStringCVar(var_name, NULL, flags);
	case CVAR_Color:	return new FColorCVar(var_name, 0, flags);
	default:			return NULL;
	}
}

void UnlatchCVars (void)
{
	FLatchedValue var;

	while (LatchedValues.Pop (var))
	{
		DWORD oldflags = var.Variable->Flags;
		var.Variable->Flags &= ~(CVAR_LATCH | CVAR_SERVERINFO);
		var.Variable->SetGenericRep (var.Value, var.Type);
		if (var.Type == CVAR_String)
			delete[] var.Value.String;
		var.Variable->Flags = oldflags;
	}
}

void DestroyCVarsFlagged (DWORD flags)
{
	FBaseCVar *cvar = CVars;
	FBaseCVar *next = cvar;

	while(cvar)
	{
		next = cvar->m_Next;

		if(cvar->Flags & flags)
			delete cvar;

		cvar = next;
	}
}

void C_SetCVarsToDefaults (void)
{
	FBaseCVar *cvar = CVars;

	while (cvar)
	{
		// Only default save-able cvars
		if (cvar->Flags & CVAR_ARCHIVE)
		{
			UCVarValue val;
			ECVarType type;
			val = cvar->GetFavoriteRepDefault (&type);
			cvar->SetGenericRep (val, type);
		}
		cvar = cvar->m_Next;
	}
}

void C_ArchiveCVars (Config *f, uint32 filter)
{
	FBaseCVar *cvar = CVars;
	char settingName[50];

	while (cvar)
	{
		if ((cvar->Flags &
			(CVAR_GLOBALCONFIG|CVAR_ARCHIVE|CVAR_MOD|CVAR_AUTO|CVAR_USERINFO|CVAR_SERVERINFO|CVAR_NOSAVE))
			== filter)
		{
			snprintf(settingName, sizeof(settingName),
				"ConsoleVar_%s", cvar->GetName ());

			UCVarValue val;
			val = cvar->GetGenericRep (CVAR_String);
			f->GetSetting(settingName)->SetValue(val.String);
		}
		cvar = cvar->m_Next;
	}
}

void C_LoadCVars (Config *f, uint32 filter)
{
	FBaseCVar *cvar = CVars;
	char settingName[50];

	while (cvar)
	{
		if ((cvar->Flags &
			(CVAR_GLOBALCONFIG|CVAR_ARCHIVE|CVAR_MOD|CVAR_AUTO|CVAR_USERINFO|CVAR_SERVERINFO|CVAR_NOSAVE))
			== filter)
		{
			snprintf(settingName, sizeof(settingName),
				"ConsoleVar_%s", cvar->GetName ());

			UCVarValue val;

			if (f->GetSetting(settingName) == NULL)
			{
				val = cvar->GetGenericRep(CVAR_String);
				f->CreateSetting(settingName, val.String);
			}

			val.String = f->GetSetting(settingName)->GetString();
			cvar->SetGenericRep(val, CVAR_String);
		}
		cvar = cvar->m_Next;
	}
}

void FBaseCVar::CmdSet (const char *newval)
{
	UCVarValue val;

	// Casting away the const is safe in this case.
	val.String = const_cast<char *>(newval);        
	SetGenericRep (val, CVAR_String);

	if (GetFlags() & CVAR_NOSET)
		Printf ("%s is write protected.\n", GetName());
	else if (GetFlags() & CVAR_LATCH)
		Printf ("%s will be changed for next game.\n", GetName());
}

CCMD (set)
{
	if (argv.argc() != 3)
	{
		Printf ("usage: set <variable> <value>\n");
	}
	else
	{
		FBaseCVar *var;

		var = FindCVar (argv[1], NULL);
		if (var == NULL)
			var = new FStringCVar (argv[1], NULL, CVAR_AUTO | CVAR_UNSETTABLE | cvar_defflags);

		var->CmdSet (argv[2]);
	}
}

CCMD (unset)
{
	if (argv.argc() != 2)
	{
		Printf ("usage: unset <variable>\n");
	}
	else
	{
		FBaseCVar *var = FindCVar (argv[1], NULL);
		if (var != NULL)
		{
			if (var->GetFlags() & CVAR_UNSETTABLE)
			{
				delete var;
			}
			else
			{
				Printf ("Cannot unset %s\n", argv[1]);
			}
		}
	}
}

CCMD (get)
{
	FBaseCVar *var, *prev;

	if (argv.argc() >= 2)
	{
		if ( (var = FindCVar (argv[1], &prev)) )
		{
			UCVarValue val;
			val = var->GetGenericRep (CVAR_String);
			Printf ("\"%s\" is \"%s\"\n", var->GetName(), val.String);
		}
		else
		{
			Printf ("\"%s\" is unset\n", argv[1]);
		}
	}
	else
	{
		Printf ("get: need variable name\n");
	}
}

CCMD (toggle)
{
	FBaseCVar *var, *prev;
	UCVarValue val;

	if (argv.argc() > 1)
	{
		if ( (var = FindCVar (argv[1], &prev)) )
		{
			val = var->GetGenericRep (CVAR_Bool);
			val.Bool = !val.Bool;
			var->SetGenericRep (val, CVAR_Bool);
			Printf ("\"%s\" is \"%s\"\n", var->GetName(),
				val.Bool ? "true" : "false");
		}
	}
}

void FBaseCVar::ListVars (const char *filter, bool plain)
{
	FBaseCVar *var = CVars;
	int count = 0;

	while (var)
	{
		if (CheckWildcards (filter, var->GetName()))
		{
			DWORD flags = var->GetFlags();
			if (plain)
			{ // plain formatting does not include user-defined cvars
				if (!(flags & CVAR_UNSETTABLE))
				{
					++count;
					Printf ("%s : %s\n", var->GetName(), var->GetGenericRep(CVAR_String).String);
				}
			}
			else
			{
				++count;
				Printf ("%c%c%c%c%c %s = %s\n",
					flags & CVAR_ARCHIVE ? 'A' : ' ',
					flags & CVAR_USERINFO ? 'U' :
						flags & CVAR_SERVERINFO ? 'S' :
						flags & CVAR_AUTO ? 'C' : ' ',
					flags & CVAR_NOSET ? '-' :
						flags & CVAR_LATCH ? 'L' :
						flags & CVAR_UNSETTABLE ? '*' : ' ',
					flags & CVAR_MOD ? 'M' : ' ',
					flags & CVAR_IGNORE ? 'X' : ' ',
					var->GetName(),
					var->GetGenericRep(CVAR_String).String);
			}
		}
		var = var->m_Next;
	}
	Printf ("%d cvars\n", count);
}

CCMD (cvarlist)
{
	if (argv.argc() == 1)
	{
		FBaseCVar::ListVars (NULL, false);
	}
	else
	{
		FBaseCVar::ListVars (argv[1], false);
	}
}

CCMD (cvarlistplain)
{
	FBaseCVar::ListVars (NULL, true);
}

CCMD (archivecvar)
{

	if (argv.argc() == 1)
	{
		Printf ("Usage: archivecvar <cvar>\n");
	}
	else
	{
		FBaseCVar *var = FindCVar (argv[1], NULL);

		if (var != NULL && (var->GetFlags() & CVAR_AUTO))
		{
			var->SetArchiveBit ();
		}
	}
}
