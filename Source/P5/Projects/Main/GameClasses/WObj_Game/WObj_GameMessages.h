#ifndef __WOBJ_GAMEMESSAGES_H
#define __WOBJ_GAMEMESSAGES_H

/*ŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻ*\
	File:			WObj_GameCore
					
	Author:			Jens Andersson
					
	Copyright:		
					
	Contents:		
					
	Comments:		
\*____________________________________________________________________________________________*/

#include "../../../Shared/MOS/Classes/GameWorld/WObjects/WObj_Game.h"

enum
{
	GAME_CLIENTFLAGS_PENDINGGAMEMSG = (1 << CWO_CLIENTFLAGS_USERSHIFT),

	OBJMSG_GAME_QUITTOMENU = 0x123,
	OBJMSG_GAME_PLAY2DSOUND = 0x124,
	OBJMSG_GAME_FADESCREEN = 0x12a,
	OBJMSG_GAME_FADESOUND = 0x12b,
	OBJMSG_GAME_SHOWGAMEMSG = 0x12c,
	OBJMSG_GAME_SHOWINFOSCREEN = 0x12d,
	OBJMSG_GAME_DEBUGMSG = 0x12e,
	OBJMSG_GAME_RESPAWNPLAYER = 0x12f,

	//0x130-137 is currently used by OBJMSG_GAMEP4_XXX
	OBJMSG_GAME_SCENEPOINT_SPAWN = 0x150,
	OBJMSG_GAME_SCENEPOINT_UNSPAWN = 0x151,
	OBJMSG_GAME_SCENEPOINT_ACTIVATE = 0x152,
	OBJMSG_GAME_LEVELEXITS = 0x153,
	OBJMSG_GAME_GETGAMESTYLE = 0x154,
	OBJMSG_GAME_SETGAMESTYLE = 0x155,
	OBJMSG_GAME_TWEAK_DAMAGE_MELEE = 0x156,
	OBJMSG_GAME_TWEAK_DAMAGE_RANGED = 0x157,
	OBJMSG_GAME_REMOVEGAMEMSG = 0x158,
	OBJMSG_GAME_PAUSEALLAI = 0x159, //Pauses/releases all non-scripted AI, should only be used by ogier script
	OBJMSG_GAME_CONEXECUTE = 0x15a,
	OBJMSG_GAME_SETMUSIC = 0x15b,
	OBJMSG_GAME_SCENEPOINT_RAISEPRIO = 0x15c,	// Raise prio
	OBJMSG_GAME_SCENEPOINT_RESTOREPRIO = 0x15d,	// lower prio (again)
	OBJMSG_GAME_MODIFYSPAWNFLAGS = 0x15e,
	OBJMSG_GAME_SHOWGAMESURFACE = 0x15f,
	OBJMSG_GAME_SHOWGAMESURFACEOVERLAY = 0x160,
	OBJMSG_GAME_RESET = 0x161,
	OBJMSG_GAME_ACHIEVMENT = 0x162,

	OBJMSG_GAME_CHARACTERKILLED = 0x12000,
	OBJMSG_GAME_DAMAGE,
	OBJMSG_GAME_RESPAWN,
	OBJMSG_GAME_RESOLVEANIMHANDLE,
	OBJMSG_GAME_GETANIMFROMHANDLE,
	OBJMSG_GAME_ALLOWAUTOTARGET,
	OBJMSG_GAME_GETTARGETHUDDATA,
	OBJMSG_GAME_REQUESTSUBTITLE,
	OBJMSG_GAME_ADDSUBTITLE,
	OBJMSG_GAME_SWITCH_CHAR,
	OBJMSG_GAME_GETSCREENFADE,
	OBJMSG_GAME_GETSOUNDFADE,
	OBJMSG_GAME_NOTIFYCUTSCENE,
	OBJMSG_GAME_FORCEENDTIMELEAP,
	OBJMSG_GAME_GETAIPARAM,
	OBJMSG_GAME_GETAIRESOURCES,
	OBJMSG_GAME_GETMINIMUMCLEARANCELEVEL,
	OBJMSG_GAME_GETLIGHTSOUT,
	OBJMSG_GAME_SETDIFFICULTYLEVEL,
	OBJMSG_GAME_GETDIFFICULTYLEVEL,
	OBJMSG_GAME_GETCLIENTDATA,
	OBJMSG_GAME_SHOWPENDINGGAMEMSG,
	OBJMSG_GAME_MODIFYFLAGS,

	OBJMSG_GAME_ADDSCENEPOINT,
	OBJMSG_GAME_GETSCENEPOINTMANAGER,
	OBJMSG_GAME_GETROOMMANAGER,
	OBJMSG_GAME_GETSHELLMANAGER,
	OBJMSG_GAME_GETSOUNDGROUPMANAGER,
	OBJMSG_GAME_SETVOLUMEMULTIPLIER,
	
	OBJMSG_BROKEN_LIGHT,

	OBJMSG_GAMECORE_GETDEFAULTSPAWNCLASS,
	OBJMSG_GAME_GETMUSIC,

	OBJMSG_GAME_GETISMP,
	OBJMSG_GAME_GETTENSION,
	OBJMSG_GAME_TOGGLEDARKLINGHUMAN,
	OBJMSG_GAME_SETGAMEMODE,
	OBJMSG_GAME_CANPICKUPSPAWN,
	OBJMSG_GAME_GETPLAYER,	//Object index
	OBJMSG_GAME_GETPLAYERWITHINDEX,	//Player index (in GameMod)
	OBJMSG_GAME_GETIS_P6,

	OBJMSG_GAME_GETOVERLAYSURFACE,

	OBJMSG_GAMECORE_LAST,
};


//General AI parameter id's; param0 in any OBJMSG_GAME_GETAIPARAM message
enum 
{
	GAME_AIPARAM_NUMPURSUITSALLOWED = 0,
	GAME_AIPARAM_NUMMELEEALLOWED,
		
	GAME_NUM_AIPARAM,
};

enum
{
	DIFFICULTYLEVEL_UNDEFINED = 0,
	DIFFICULTYLEVEL_EASY = 1,
	DIFFICULTYLEVEL_NORMAL,
	DIFFICULTYLEVEL_HARD,
	DIFFICULTYLEVEL_COMMENTARY,

	NUM_DIFFICULTYLEVELS,
};

#endif
