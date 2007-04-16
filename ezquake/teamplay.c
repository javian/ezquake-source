/*
Copyright (C) 2000-2003       Anton Gavrilov, A Nourai

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    $Id: teamplay.c,v 1.67.2.10 2007-04-15 20:15:16 disconn3ct Exp $
*/

#define TP_ISEYESMODEL(x) ((x) && cl.model_precache[(x)] && cl.model_precache[(x)]->modhint == MOD_EYES)

#include <time.h>
#include <string.h>
#include "quakedef.h"
#include "ignore.h"
#ifdef GLQUAKE
#include "gl_model.h"
#include "gl_local.h"
#else
#include "r_model.h"
#include "r_local.h"
#endif
#include "teamplay.h"
#include "rulesets.h"
#include "pmove.h"
#include "stats_grid.h"
#include "utils.h"
#include "qsound.h"


qbool OnChangeSkinForcing(cvar_t *var, char *string);
qbool OnChangeColorForcing(cvar_t *var, char *string);

cvar_t	cl_parseSay = {"cl_parseSay", "1"};
cvar_t	cl_parseFunChars = {"cl_parseFunChars", "1"};
cvar_t	tp_triggers = {"tp_triggers", "1"};
cvar_t	tp_msgtriggers = {"tp_msgtriggers", "1"};
cvar_t	tp_forceTriggers = {"tp_forceTriggers", "0"};
cvar_t	cl_nofake = {"cl_nofake", "2"};
cvar_t	tp_loadlocs = {"tp_loadlocs", "1"};
cvar_t  tp_pointpriorities = {"tp_pointpriorities", "0"};

cvar_t  cl_teamtopcolor = {"teamtopcolor", "-1", 0, OnChangeColorForcing};
cvar_t  cl_teambottomcolor = {"teambottomcolor", "-1", 0, OnChangeColorForcing};
cvar_t  cl_enemytopcolor = {"enemytopcolor", "-1", 0, OnChangeColorForcing};
cvar_t	cl_enemybottomcolor = {"enemybottomcolor", "-1", 0, OnChangeColorForcing};
cvar_t	cl_teamskin = {"teamskin", "", 0, OnChangeSkinForcing};
cvar_t	cl_enemyskin = {"enemyskin", "", 0, OnChangeSkinForcing};
cvar_t	cl_teamquadskin = {"teamquadskin", "", 0, OnChangeSkinForcing};
cvar_t	cl_enemyquadskin = {"enemyquadskin", "", 0, OnChangeSkinForcing};
cvar_t	cl_teampentskin = {"teampentskin", "", 0, OnChangeSkinForcing};
cvar_t	cl_enemypentskin = {"enemypentskin", "", 0, OnChangeSkinForcing};
cvar_t	cl_teambothskin = {"teambothskin", "", 0, OnChangeSkinForcing};
cvar_t	cl_enemybothskin = {"enemybothskin", "", 0, OnChangeSkinForcing};


cvar_t  tp_soundtrigger = {"tp_soundtrigger", "~"};

cvar_t	tp_name_axe = {"tp_name_axe", "axe"};
cvar_t	tp_name_sg = {"tp_name_sg", "sg"};
cvar_t	tp_name_ssg = {"tp_name_ssg", "ssg"};
cvar_t	tp_name_ng = {"tp_name_ng", "ng"};
cvar_t	tp_name_sng = {"tp_name_sng", "sng"};
cvar_t	tp_name_gl = {"tp_name_gl", "gl"};
cvar_t	tp_name_rl = {"tp_name_rl", "rl"};
cvar_t	tp_name_lg = {"tp_name_lg", "lg"};
cvar_t	tp_name_armortype_ra = {"tp_name_armortype_ra", "r"};
cvar_t	tp_name_armortype_ya = {"tp_name_armortype_ya", "y"};
cvar_t	tp_name_armortype_ga = {"tp_name_armortype_ga", "g"};
cvar_t	tp_name_ra = {"tp_name_ra", "ra"};
cvar_t	tp_name_ya = {"tp_name_ya", "ya"};
cvar_t	tp_name_ga = {"tp_name_ga", "ga"};
cvar_t	tp_name_quad = {"tp_name_quad", "quad"};
cvar_t	tp_name_pent = {"tp_name_pent", "pent"};
cvar_t	tp_name_ring = {"tp_name_ring", "ring"};
cvar_t	tp_name_suit = {"tp_name_suit", "suit"};
cvar_t	tp_name_shells = {"tp_name_shells", "shells"};
cvar_t	tp_name_nails = {"tp_name_nails", "nails"};
cvar_t	tp_name_rockets = {"tp_name_rockets", "rockets"};
cvar_t	tp_name_cells = {"tp_name_cells", "cells"};
cvar_t	tp_name_mh = {"tp_name_mh", "mega"};
cvar_t	tp_name_health = {"tp_name_health", "health"};
cvar_t	tp_name_armor = {"tp_name_armor", "armor"};
cvar_t	tp_name_weapon = {"tp_name_weapon", "weapon"};
cvar_t	tp_name_backpack = {"tp_name_backpack", "pack"};
cvar_t	tp_name_flag = {"tp_name_flag", "flag"};
cvar_t	tp_name_sentry = {"tp_name_sentry", "sentry gun"};
cvar_t	tp_name_disp = {"tp_name_disp", "dispenser"};
cvar_t	tp_name_rune1 = {"tp_name_rune1", "resistance rune"};
cvar_t	tp_name_rune2 = {"tp_name_rune2", "strength rune"};
cvar_t	tp_name_rune3 = {"tp_name_rune3", "haste rune"};
cvar_t	tp_name_rune4 = {"tp_name_rune4", "regeneration rune"};
cvar_t	tp_name_teammate = {"tp_name_teammate", ""};
cvar_t	tp_name_enemy = {"tp_name_enemy", "enemy"};
cvar_t	tp_name_eyes = {"tp_name_eyes", "eyes"};
cvar_t	tp_name_quaded = {"tp_name_quaded", "quaded"};
cvar_t	tp_name_pented = {"tp_name_pented", "pented"};
cvar_t	tp_name_nothing = {"tp_name_nothing", "nothing"};
cvar_t	tp_name_someplace = {"tp_name_someplace", "someplace"};
cvar_t	tp_name_at = {"tp_name_at", "at"};
cvar_t	tp_name_none = {"tp_name_none", ""};
cvar_t	tp_name_separator = {"tp_name_separator", "/"};
cvar_t	tp_weapon_order = {"tp_weapon_order", "78654321"};

cvar_t	tp_name_status_green = {"tp_name_status_green", "$G"};
cvar_t	tp_name_status_yellow = {"tp_name_status_yellow", "$Y"};
cvar_t	tp_name_status_red = {"tp_name_status_red", "$R"};
cvar_t	tp_name_status_blue = {"tp_name_status_blue", "$B"};

cvar_t	tp_need_ra = {"tp_need_ra", "50"};
cvar_t	tp_need_ya = {"tp_need_ya", "50"};
cvar_t	tp_need_ga = {"tp_need_ga", "50"};
cvar_t	tp_need_health = {"tp_need_health", "50"};
cvar_t	tp_need_weapon = {"tp_need_weapon", "35687"};
cvar_t	tp_need_rl = {"tp_need_rl", "1"};
cvar_t	tp_need_rockets = {"tp_need_rockets", "5"};
cvar_t	tp_need_cells = {"tp_need_cells", "20"};
cvar_t	tp_need_nails = {"tp_need_nails", "40"};
cvar_t	tp_need_shells = {"tp_need_shells", "10"};

static qbool suppress;

char *skinforcing_team = "";

void TP_FindModelNumbers (void);
void TP_FindPoint (void);
char *TP_LocationName (vec3_t location);
static void CountNearbyPlayers(qbool dead);
char *Macro_LastTookOrPointed (void);
char *Macro_LastTookOrPointed2 (void);
void R_TranslatePlayerSkin (int playernum);

#define	POINT_TYPE_ITEM			1
#define POINT_TYPE_POWERUP		2
#define POINT_TYPE_TEAMMATE		3
#define	POINT_TYPE_ENEMY		4

#define	TP_TOOK_EXPIRE_TIME		15
#define	TP_POINT_EXPIRE_TIME	TP_TOOK_EXPIRE_TIME

// this structure is cleared after entering a new map
typedef struct tvars_s
{
	int		health;
	int		items;
	int		olditems;
	int		activeweapon;
	int		stat_framecounts[MAX_CL_STATS];
	double	deathtrigger_time;
	float	f_skins_reply_time;
	float	f_version_reply_time;
	char	lastdeathloc[MAX_LOC_NAME];
	char	tookname[32];
	char	tookloc[MAX_LOC_NAME];
	double	tooktime;
	double	pointtime; // cls.realtime for which pointitem & pointloc are valid
	char	pointname[32];
	char	pointloc[MAX_LOC_NAME];
	int		pointtype;
	char	nearestitemloc[MAX_LOC_NAME];
	char	lastreportedloc[MAX_LOC_NAME];
	double	lastdrop_time;
	char	lastdroploc[MAX_LOC_NAME];
	char	lasttrigger_match[256];

	int	numenemies;
	int	numfriendlies;
	int	last_numenemies;
	int	last_numfriendlies;

	int enemy_powerups;
	double enemy_powerups_time;
} tvars_t;

tvars_t vars;

static char lastip[32]; // FIXME: remove it

// re-triggers stuff
cvar_t re_sub[10] = {{"re_trigger_match_0", "", CVAR_ROM},
                    {"re_trigger_match_1", "", CVAR_ROM},
                    {"re_trigger_match_2", "", CVAR_ROM},
                    {"re_trigger_match_3", "", CVAR_ROM},
                    {"re_trigger_match_4", "", CVAR_ROM},
                    {"re_trigger_match_5", "", CVAR_ROM},
                    {"re_trigger_match_6", "", CVAR_ROM},
                    {"re_trigger_match_7", "", CVAR_ROM},
                    {"re_trigger_match_8", "", CVAR_ROM},
                    {"re_trigger_match_9", "", CVAR_ROM}};

cvar_t re_subi[10] = {{"internal0"},
                     {"internal1"},
                     {"internal2"},
                     {"internal3"},
                     {"internal4"},
                     {"internal5"},
                     {"internal6"},
                     {"internal7"},
                     {"internal8"},
                     {"internal9"}};

static pcre_trigger_t *re_triggers;
static pcre_internal_trigger_t *internal_triggers;
/********************************** TRIGGERS **********************************/

typedef struct f_trigger_s
{
	char *name;
	qbool restricted;
	qbool teamplay;
} f_trigger_t;

f_trigger_t f_triggers[] = {
                               {"f_newmap", false, false},
                               {"f_spawn", false, false},
                               {"f_mapend", false, false},
                               {"f_reloadstart", false, false},
                               {"f_reloadend", false, false},
                               {"f_cfgload", false, false},
                               {"f_exit", false, false},
                               {"f_demostart", false, false},
                               {"f_demoend", false, false},
                               {"f_captureframe", false, false},

							   {"f_freeflyspectate", false, false},
							   {"f_trackspectate", false, false},

                               {"f_weaponchange", false, false},

                               {"f_took", true, true},
                               {"f_respawn", true, true},
                               {"f_death", true, true},
                               {"f_flagdeath", true, true},

                               {"f_conc", true, true},
                               {"f_flash", true, true},
                               {"f_bonusflash", true, true},
                           };

#define num_f_triggers	(sizeof(f_triggers) / sizeof(f_triggers[0]))

void TP_ExecTrigger (char *trigger)
{
	int i, j, numteammates = 0;
	cmd_alias_t *alias;

	if (!tp_triggers.value || ((cls.demoplayback || cl.spectator) && cl_restrictions.value))
		return;

	for (i = 0; i < num_f_triggers; i++) {
		if (!strcmp (f_triggers[i].name, trigger))
			break;
	}
	if (i == num_f_triggers)
		Sys_Error ("Unknown f_trigger \"%s\"", trigger);

	if (f_triggers[i].teamplay && !tp_forceTriggers.value) {
		if (!cl.teamplay)
			return;

		for (j = 0; j < MAX_CLIENTS; j++) {
			if (cl.players[j].name[0] && !cl.players[j].spectator && j != cl.playernum) {
				if (!strcmp(cl.players[j].team, cl.players[cl.playernum].team))
					numteammates++;
			}
		}

		if (!numteammates)
			return;
	}

	if ((alias = Cmd_FindAlias(trigger))) {
		if (!(f_triggers[i].restricted && Rulesets_RestrictTriggers())) {
			Cbuf_AddTextEx (&cbuf_main, va("%s\n", alias->value));
		}
	}
}

/*********************************** MACROS ***********************************/

#define MAX_MACRO_VALUE	256
static char	macro_buf[MAX_MACRO_VALUE] = "";

char *Macro_Lastip_f (void)
{
	snprintf (macro_buf, sizeof(macro_buf), "%s", lastip);
	return macro_buf;
}

char *Macro_Quote_f (void)
{
	return "\"";
}

char *Macro_Latency (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", Q_rint(cls.latency * 1000));
	return macro_buf;
}

char *Macro_Health (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_HEALTH]);
	return macro_buf;
}

char *Macro_Armor (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_ARMOR]);
	return macro_buf;
}

char *Macro_Shells (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_SHELLS]);
	return macro_buf;
}

char *Macro_Nails (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_NAILS]);
	return macro_buf;
}

char *Macro_Rockets (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_ROCKETS]);
	return macro_buf;
}

char *Macro_Cells (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_CELLS]);
	return macro_buf;
}

char *Macro_Ammo (void)
{
	snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_AMMO]);
	return macro_buf;
}

char *Weapon_NumToString(int num)
{
	switch (num) {
			case IT_AXE: return tp_name_axe.string;
			case IT_SHOTGUN: return tp_name_sg.string;
			case IT_SUPER_SHOTGUN: return tp_name_ssg.string;
			case IT_NAILGUN: return tp_name_ng.string;
			case IT_SUPER_NAILGUN: return tp_name_sng.string;
			case IT_GRENADE_LAUNCHER: return tp_name_gl.string;
			case IT_ROCKET_LAUNCHER: return tp_name_rl.string;
			case IT_LIGHTNING: return tp_name_lg.string;
			default: return tp_name_none.string;
	}
}

char *Macro_Weapon (void)
{
	return Weapon_NumToString(cl.stats[STAT_ACTIVEWEAPON]);
}

char *Macro_WeaponAndAmmo (void)
{
	char buf[MAX_MACRO_VALUE];
	snprintf (buf, sizeof(buf), "%s:%s", Macro_Weapon(), Macro_Ammo());
	strlcpy (macro_buf, buf, sizeof(macro_buf));
	return macro_buf;
}

char *Macro_WeaponNum (void)
{
	switch (cl.stats[STAT_ACTIVEWEAPON]) {

			case IT_AXE: return "1";
			case IT_SHOTGUN: return "2";
			case IT_SUPER_SHOTGUN: return "3";
			case IT_NAILGUN: return "4";
			case IT_SUPER_NAILGUN: return "5";
			case IT_GRENADE_LAUNCHER: return "6";
			case IT_ROCKET_LAUNCHER: return "7";
			case IT_LIGHTNING: return "8";
			default:
			return "0";
	}
}

int BestWeapon (void)
{
	return BestWeaponFromStatItems (cl.stats[STAT_ITEMS]);
}

int BestWeaponFromStatItems (int stat)
{
	int i;
	char *t[] = {tp_weapon_order.string, "78654321", NULL}, **s;

	for (s = t; *s; s++) {
		for (i = 0 ; i < strlen(*s) ; i++) {
			switch ((*s)[i]) {
					case '1': if (stat & IT_AXE) return IT_AXE; break;
					case '2': if (stat & IT_SHOTGUN) return IT_SHOTGUN; break;
					case '3': if (stat & IT_SUPER_SHOTGUN) return IT_SUPER_SHOTGUN; break;
					case '4': if (stat & IT_NAILGUN) return IT_NAILGUN; break;
					case '5': if (stat & IT_SUPER_NAILGUN) return IT_SUPER_NAILGUN; break;
					case '6': if (stat & IT_GRENADE_LAUNCHER) return IT_GRENADE_LAUNCHER; break;
					case '7': if (stat & IT_ROCKET_LAUNCHER) return IT_ROCKET_LAUNCHER; break;
					case '8': if (stat & IT_LIGHTNING) return IT_LIGHTNING; break;
			}
		}
	}
	return 0;
}

char *Macro_BestWeapon (void)
{
	switch (BestWeapon()) {
			case IT_AXE: return tp_name_axe.string;
			case IT_SHOTGUN: return tp_name_sg.string;
			case IT_SUPER_SHOTGUN: return tp_name_ssg.string;
			case IT_NAILGUN: return tp_name_ng.string;
			case IT_SUPER_NAILGUN: return tp_name_sng.string;
			case IT_GRENADE_LAUNCHER: return tp_name_gl.string;
			case IT_ROCKET_LAUNCHER: return tp_name_rl.string;
			case IT_LIGHTNING: return tp_name_lg.string;
			default: return tp_name_none.string;
	}
}

char *Macro_BestAmmo (void)
{
	switch (BestWeapon()) {
			case IT_SHOTGUN: case IT_SUPER_SHOTGUN:
			snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_SHELLS]);
			return macro_buf;

			case IT_NAILGUN: case IT_SUPER_NAILGUN:
			snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_NAILS]);
			return macro_buf;

			case IT_GRENADE_LAUNCHER: case IT_ROCKET_LAUNCHER:
			snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_ROCKETS]);
			return macro_buf;

			case IT_LIGHTNING:
			snprintf(macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_CELLS]);
			return macro_buf;

			default: return "0";
	}
}

// needed for %b parsing
char *Macro_BestWeaponAndAmmo (void)
{
	char buf[MAX_MACRO_VALUE];

	snprintf (buf, sizeof(buf), "%s:%s", Macro_BestWeapon(), Macro_BestAmmo());
	strlcpy (macro_buf, buf, sizeof(buf));
	return macro_buf;
}

char *Macro_ArmorType (void)
{
	if (cl.stats[STAT_ITEMS] & IT_ARMOR1)
		return tp_name_armortype_ga.string;
	else if (cl.stats[STAT_ITEMS] & IT_ARMOR2)
		return tp_name_armortype_ya.string;
	else if (cl.stats[STAT_ITEMS] & IT_ARMOR3)
		return tp_name_armortype_ra.string;
	else
		return tp_name_none.string;
}

char *Macro_Powerups (void)
{
	int effects;

	macro_buf[0] = 0;

	if (cl.stats[STAT_ITEMS] & IT_QUAD)
		strlcpy(macro_buf, tp_name_quad.string, sizeof(macro_buf));

	if (cl.stats[STAT_ITEMS] & IT_INVULNERABILITY) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_pent.string, sizeof(macro_buf));
	}

	if (cl.stats[STAT_ITEMS] & IT_INVISIBILITY) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_ring.string, sizeof(macro_buf));
	}

	effects = cl.frames[cl.parsecount & UPDATE_MASK].playerstate[cl.playernum].effects;
	if ( (effects & (EF_FLAG1|EF_FLAG2)) /* CTF */ ||
	        (cl.teamfortress && cl.stats[STAT_ITEMS] & (IT_KEY1|IT_KEY2)) /* TF */ ) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_flag.string, sizeof(macro_buf));
	}

	if (!macro_buf[0])
		strlcpy(macro_buf, tp_name_none.string, sizeof(macro_buf));

	return macro_buf;
}

char *Macro_Location (void)
{
	strlcpy(vars.lastreportedloc, TP_LocationName (cl.simorg), sizeof(vars.lastreportedloc));
	return vars.lastreportedloc;
}

char *Macro_LastDeath (void)
{
	return vars.deathtrigger_time ? vars.lastdeathloc : tp_name_someplace.string;
}

char *Macro_Last_Location (void)
{
	if (vars.deathtrigger_time && cls.realtime - vars.deathtrigger_time <= 5)
		strlcpy(vars.lastreportedloc, vars.lastdeathloc, sizeof(vars.lastreportedloc));
	else
		strlcpy(vars.lastreportedloc, TP_LocationName (cl.simorg), sizeof(vars.lastreportedloc));
	return vars.lastreportedloc;
}

char *Macro_LastReportedLoc(void)
{
	if (!vars.lastreportedloc[0])
		return tp_name_someplace.string;
	return vars.lastreportedloc;
}

char *Macro_Time (void)
{
	time_t t;
	struct tm *ptm;

	time (&t);
	if (!(ptm = localtime (&t)))
		return "#bad date#";
	strftime (macro_buf, sizeof(macro_buf) - 1, "%H:%M", ptm);
	return macro_buf;
}

char *Macro_Date (void)
{
	time_t t;
	struct tm *ptm;

	time (&t);
	if (!(ptm = localtime (&t)))
		return "#bad date#";
	strftime (macro_buf, sizeof(macro_buf) - 1, "%d.%m.%y", ptm);
	return macro_buf;
}

// returns the last item picked up
char *Macro_Took (void)
{
	if (!vars.tooktime || cls.realtime > vars.tooktime + TP_TOOK_EXPIRE_TIME)
		strlcpy (macro_buf, tp_name_nothing.string, sizeof(macro_buf));
	else
		strlcpy (macro_buf, vars.tookname, sizeof(macro_buf));
	return macro_buf;
}

// returns location of the last item picked up
char *Macro_TookLoc (void)
{
	if (!vars.tooktime || cls.realtime > vars.tooktime + TP_TOOK_EXPIRE_TIME)
		strlcpy (macro_buf, tp_name_someplace.string, sizeof(macro_buf));
	else
		strlcpy (macro_buf, vars.tookloc, sizeof(macro_buf));
	return macro_buf;
}

// %i macro - last item picked up in "name at location" style
char *Macro_TookAtLoc (void)
{
	if (!vars.tooktime || cls.realtime > vars.tooktime + TP_TOOK_EXPIRE_TIME)
		strlcpy (macro_buf, tp_name_nothing.string, sizeof(macro_buf));
	else {
		strlcpy (macro_buf, va("%s %s %s", vars.tookname, tp_name_at.string, vars.tookloc), sizeof(macro_buf));
	}
	return macro_buf;
}

// pointing calculations are CPU expensive, so the results are cached
// in vars.pointname & vars.pointloc
char *Macro_PointName (void)
{
	if (flashed) // there should be more smart way to do it
		return tp_name_nothing.string;

	TP_FindPoint ();
	return vars.pointname;
}

char *Macro_PointLocation (void)
{
	if (flashed) // there should be more smart way to do it
		return tp_name_nothing.string;

	TP_FindPoint ();
	return vars.pointloc[0] ? vars.pointloc : Macro_Location();
}

char *Macro_LastPointAtLoc (void)
{
	if (!vars.pointtime || cls.realtime - vars.pointtime > TP_POINT_EXPIRE_TIME)
		strlcpy (macro_buf, tp_name_nothing.string, sizeof(macro_buf));
	else
		snprintf (macro_buf, sizeof(macro_buf), "%s %s %s", vars.pointname, tp_name_at.string, vars.pointloc[0] ? vars.pointloc : Macro_Location());
	return macro_buf;
}

char *Macro_PointNameAtLocation (void)
{
	if (flashed) // there should be more smart way to do it
		return tp_name_nothing.string;

	TP_FindPoint();
	return Macro_LastPointAtLoc();
}

char *Macro_Weapons (void)
{
	macro_buf[0] = 0;

	if (cl.stats[STAT_ITEMS] & IT_LIGHTNING)
		strlcpy(macro_buf, tp_name_lg.string, sizeof(macro_buf));

	if (cl.stats[STAT_ITEMS] & IT_ROCKET_LAUNCHER) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_rl.string, sizeof(macro_buf));
	}
	if (cl.stats[STAT_ITEMS] & IT_GRENADE_LAUNCHER) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_gl.string, sizeof(macro_buf));
	}
	if (cl.stats[STAT_ITEMS] & IT_SUPER_NAILGUN) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_sng.string, sizeof(macro_buf));
	}
	if (cl.stats[STAT_ITEMS] & IT_NAILGUN) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_ng.string, sizeof(macro_buf));
	}
	if (cl.stats[STAT_ITEMS] & IT_SUPER_SHOTGUN) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_ssg.string, sizeof(macro_buf));
	}
	if (cl.stats[STAT_ITEMS] & IT_SHOTGUN) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_sg.string, sizeof(macro_buf));
	}
	if (cl.stats[STAT_ITEMS] & IT_AXE) {
		if (macro_buf[0])
			strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat(macro_buf, tp_name_axe.string, sizeof(macro_buf));
	}

	if (!macro_buf[0])
		strlcpy(macro_buf, tp_name_none.string, sizeof(macro_buf));

	return macro_buf;
}

static char *Skin_To_TFSkin (char *myskin)
{
	if (!cl.teamfortress || cl.spectator || strncasecmp(myskin, "tf_", 3)) {
		strlcpy(macro_buf, myskin, sizeof(macro_buf));
	} else {
		if (!strcasecmp(myskin, "tf_demo"))
			strlcpy(macro_buf, "demoman", sizeof(macro_buf));
		else if (!strcasecmp(myskin, "tf_eng"))
			strlcpy(macro_buf, "engineer", sizeof(macro_buf));
		else if (!strcasecmp(myskin, "tf_snipe"))
			strlcpy(macro_buf, "sniper", sizeof(macro_buf));
		else if (!strcasecmp(myskin, "tf_sold"))
			strlcpy(macro_buf, "soldier", sizeof(macro_buf));
		else
			strlcpy(macro_buf, myskin + 3, sizeof(macro_buf));
	}
	return macro_buf;
}

char *Macro_TF_Skin (void)
{
	return Skin_To_TFSkin(Info_ValueForKey(cl.players[cl.playernum].userinfo, "skin"));
}

char *Macro_LastDrop (void)
{
	if (vars.lastdrop_time)
		return vars.lastdroploc;
	else
		return tp_name_someplace.string;
}

char *Macro_LastTrigger_Match(void)
{
	return vars.lasttrigger_match;
}

char *Macro_LastDropTime (void)
{
	if (vars.lastdrop_time)
		snprintf (macro_buf, 32, "%d", (int) (cls.realtime - vars.lastdrop_time));
	else
		snprintf (macro_buf, 32, "%d", -1);
	return macro_buf;
}

char *Macro_Need (void)
{
	int i, weapon;
	char *needammo = NULL;

	macro_buf[0] = 0;

	// check armor
	if (((cl.stats[STAT_ITEMS] & IT_ARMOR1) && cl.stats[STAT_ARMOR] < tp_need_ga.value)
		|| ((cl.stats[STAT_ITEMS] & IT_ARMOR2) && cl.stats[STAT_ARMOR] < tp_need_ya.value)
		|| ((cl.stats[STAT_ITEMS] & IT_ARMOR3) && cl.stats[STAT_ARMOR] < tp_need_ra.value)
		|| (!(cl.stats[STAT_ITEMS] & (IT_ARMOR1|IT_ARMOR2|IT_ARMOR3))
		&& (tp_need_ga.value || tp_need_ya.value || tp_need_ra.value))
	   )
		strlcpy (macro_buf, tp_name_armor.string, sizeof(macro_buf));

	// check health
	if (tp_need_health.value && cl.stats[STAT_HEALTH] < tp_need_health.value) {
		if (macro_buf[0])
			strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat (macro_buf, tp_name_health.string, sizeof(macro_buf));
	}

	if (cl.teamfortress) {
		if (cl.stats[STAT_ROCKETS] < tp_need_rockets.value)	{
			if (macro_buf[0])
				strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat (macro_buf, tp_name_rockets.string, sizeof(macro_buf));
		}
		if (cl.stats[STAT_SHELLS] < tp_need_shells.value) {
			if (macro_buf[0])
				strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat (macro_buf, tp_name_shells.string, sizeof(macro_buf));
		}
		if (cl.stats[STAT_NAILS] < tp_need_nails.value)	{
			if (macro_buf[0])
				strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat (macro_buf, tp_name_nails.string, sizeof(macro_buf));
		}
		if (cl.stats[STAT_CELLS] < tp_need_cells.value)	{
			if (macro_buf[0])
				strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat (macro_buf, tp_name_cells.string, sizeof(macro_buf));
		}
		goto done;
	}
	// check weapon
	weapon = 0;
	for (i = strlen(tp_need_weapon.string) - 1 ; i >= 0 ; i--) {
		switch (tp_need_weapon.string[i]) {
				case '2': if (cl.stats[STAT_ITEMS] & IT_SHOTGUN) weapon = 2; break;
				case '3': if (cl.stats[STAT_ITEMS] & IT_SUPER_SHOTGUN) weapon = 3; break;
				case '4': if (cl.stats[STAT_ITEMS] & IT_NAILGUN) weapon = 4; break;
				case '5': if (cl.stats[STAT_ITEMS] & IT_SUPER_NAILGUN) weapon = 5; break;
				case '6': if (cl.stats[STAT_ITEMS] & IT_GRENADE_LAUNCHER) weapon = 6; break;
				case '7': if (cl.stats[STAT_ITEMS] & IT_ROCKET_LAUNCHER) weapon = 7; break;
				case '8': if (cl.stats[STAT_ITEMS] & IT_LIGHTNING) weapon = 8; break;
		}
		if (weapon)
			break;
	}

	if (!weapon) {
		if (macro_buf[0])
			strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
		strlcat (macro_buf, tp_name_weapon.string, sizeof(macro_buf));
	} else {
		if (tp_need_rl.value && !(cl.stats[STAT_ITEMS] & IT_ROCKET_LAUNCHER)) {
			if (macro_buf[0])
				strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat (macro_buf, tp_name_rl.string, sizeof(macro_buf));
		}

		switch (weapon) {
				case 2: case 3: if (cl.stats[STAT_SHELLS] < tp_need_shells.value)
					needammo = tp_name_shells.string;
				break;
				case 4: case 5: if (cl.stats[STAT_NAILS] < tp_need_nails.value)
					needammo = tp_name_nails.string;
				break;
				case 6: case 7: if (cl.stats[STAT_ROCKETS] < tp_need_rockets.value)
					needammo = tp_name_rockets .string;
				break;
				case 8: if (cl.stats[STAT_CELLS] < tp_need_cells.value)
					needammo = tp_name_cells.string;
				break;
		}

		if (needammo) {
			if (macro_buf[0])
				strlcat (macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat (macro_buf, needammo, sizeof(macro_buf));
		}
	}

done:
	if (!macro_buf[0])
		strlcpy (macro_buf, tp_name_nothing.string, sizeof(macro_buf));

	return macro_buf;
}

char *Macro_Point_LED(void)
{
	TP_FindPoint();

	if (vars.pointtype == POINT_TYPE_ENEMY)
		return tp_name_status_red.string;
	else if (vars.pointtype == POINT_TYPE_TEAMMATE)
		return tp_name_status_green.string;
	else if (vars.pointtype == POINT_TYPE_POWERUP)
		return tp_name_status_yellow.string;
	else // POINT_TYPE_ITEM
		return tp_name_status_blue.string;

	return macro_buf;
}



char *Macro_MyStatus_LED(void)
{
	int count;
	float save_need_rl;
	char *s, *save_separator;
	static char separator[] = {'/', '\0'};

	save_need_rl = tp_need_rl.value;
	save_separator = tp_name_separator.string;
	tp_need_rl.value = 0;
	tp_name_separator.string = separator;
	s = Macro_Need();
	tp_need_rl.value = save_need_rl;
	tp_name_separator.string = save_separator;

	if (!strcmp(s, tp_name_nothing.string)) {
		count = 0;
	} else  {
		for (count = 1; *s; s++)
			if (*s == separator[0])
				count++;
	}

	if (count == 0)
		snprintf(macro_buf, sizeof(macro_buf), "%s", tp_name_status_green.string);
	else if (count <= 1)
		snprintf(macro_buf, sizeof(macro_buf), "%s", tp_name_status_yellow.string);
	else
		snprintf(macro_buf, sizeof(macro_buf), "%s", tp_name_status_red.string);

	return macro_buf;
}

char *Macro_EnemyStatus_LED(void)
{
	CountNearbyPlayers(false);
	if (vars.numenemies == 0)
		snprintf(macro_buf, sizeof(macro_buf), "\xffl%s\xff", tp_name_status_green.string);
	else if (vars.numenemies <= vars.numfriendlies)
		snprintf(macro_buf, sizeof(macro_buf), "\xffl%s\xff", tp_name_status_yellow.string);
	else
		snprintf(macro_buf, sizeof(macro_buf), "\xffl%s\xff", tp_name_status_red.string);

	suppress = true;
	return macro_buf;
}


#define TP_PENT 1
#define TP_QUAD 2
#define TP_RING 4

char *Macro_LastSeenPowerup(void)
{
	if (!vars.enemy_powerups_time || cls.realtime - vars.enemy_powerups_time > 5) {
		strlcpy(macro_buf, tp_name_quad.string, sizeof(macro_buf));
	} else {
		macro_buf[0] = 0;
		if (vars.enemy_powerups & TP_QUAD)
			strlcat(macro_buf, tp_name_quad.string, sizeof(macro_buf));
		if (vars.enemy_powerups & TP_PENT) {
			if (macro_buf[0])
				strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat(macro_buf, tp_name_pent.string, sizeof(macro_buf));
		}
		if (vars.enemy_powerups & TP_RING) {
			if (macro_buf[0])
				strlcat(macro_buf, tp_name_separator.string, sizeof(macro_buf));
			strlcat(macro_buf, tp_name_ring.string, sizeof(macro_buf));
		}
	}
	return macro_buf;
}


qbool TP_SuppressMessage(char *buf)
{
	int len;
	char *s;

	if ((len = strlen(buf)) < 4)
		return false;

	s = buf + len - 4;

	if (s[0] == 0x7F && s[1] == '!' && s[3] == '\n') {
		*s++ = '\n';
		*s++ = 0;
		return (!cls.demoplayback && !cl.spectator && *s - 'A' == cl.playernum);
	}

	return false;
}

void TP_PrintHiddenMessage(char *buf, int nodisplay)
{
	qbool team, hide = false;
	char dest[4096], msg[4096], *s, *d, c, *name;
	int length, offset, flags;
	extern cvar_t con_sound_mm2_file, con_sound_mm2_volume;

	if (!buf || !(length = strlen(buf)))
		return;

	team = !strcasecmp("say_team", Cmd_Argv(0));

	if (length >= 2 && buf[0] == '\"' && buf[length - 1] == '\"') {
		memmove(buf, buf + 1, length - 2);
		buf[length - 2] = 0;
	}

	s = buf;
	d = dest;

	while ((c = *s++) && (c != '\x7f')) {
		if (c == '\xff') {
			if ((hide = !hide)) {
				*d++ = (*s == 'z') ? 'x' : 139;
				s++;
				memmove(s - 2, s, strlen(s) + 1);
				s -= 2;
			} else {
				memmove(s - 1, s, strlen(s) + 1);
				s -= 1;
			}
		} else if (!hide) {
			*d++ = c;
		}
	}
	*d = 0;

	if (cls.demoplayback)
		return;

	name = Info_ValueForKey (cl.players[cl.playernum].userinfo, "name");
	if (strlen(name) >= 32)
		name[31] = 0;

	if (team)
		snprintf(msg, sizeof(msg), "(%s): %s\n", name, TP_ParseFunChars(dest, true));
	else
		snprintf(msg, sizeof(msg), "%s: %s\n", name, TP_ParseFunChars(dest, true));

	flags = TP_CategorizeMessage (msg, &offset);

	if (flags == 2 && !TP_FilterMessage(msg + offset))
		return;

	if (con_sound_mm2_volume.value > 0 && nodisplay == 0) {
		S_LocalSoundWithVol(con_sound_mm2_file.string, con_sound_mm2_volume.value);
	}

	if (cl_nofake.value == 1 || (cl_nofake.value == 2 && flags != 2)) {
		for (s = msg; *s; s++)
			if (*s == 0x0D || (*s == 0x0A && s[1]))
				*s = ' ';
	}

	if (nodisplay == 0) {
		Com_Printf(wcs2str(TP_ParseWhiteText (str2wcs(msg), team, offset)));
	}

}

#define ISDEAD(i) ( (i) >= 41 && (i) <= 102 )

static void CountNearbyPlayers(qbool dead)
{
	int i;
	player_state_t *state;
	player_info_t *info;
	static int lastframecount = -1;

	if (cls.framecount == lastframecount)
		return;
	lastframecount = cls.framecount;

	vars.numenemies = vars.numfriendlies = 0;

	if (!cl.spectator && !dead)
		vars.numfriendlies++;

	if (!cl.oldparsecount || !cl.parsecount || cls.state < ca_active)
		return;

	state = cl.frames[cl.oldparsecount & UPDATE_MASK].playerstate;
	info = cl.players;
	for (i = 0; i < MAX_CLIENTS; i++, info++, state++) {
		if (i != cl.playernum && state->messagenum == cl.oldparsecount && !info->spectator && !ISDEAD(state->frame)) {
			if (cl.teamplay && !strcmp(info->team, TP_PlayerTeam()))
				vars.numfriendlies++;
			else
				vars.numenemies++;
		}
	}
}


char *Macro_CountNearbyEnemyPlayers (void)
{
	if(!strncasecmp(Rulesets_Ruleset(), "MTFL", 4))
		return "banned by MTFL ruleset"; // there should be more smart way to do it

	CountNearbyPlayers(false);
	sprintf(macro_buf, "\xffz%d\xff", vars.numenemies);
	suppress = true;
	return macro_buf;
}


char *Macro_Count_Last_NearbyEnemyPlayers (void)
{
	if(!strncasecmp(Rulesets_Ruleset(), "MTFL", 4))
		return "banned by MTFL ruleset"; // there should be more smart way to do it

	if (vars.deathtrigger_time && cls.realtime - vars.deathtrigger_time <= 5) {
		sprintf(macro_buf, "\xffz%d\xff", vars.last_numenemies);
	} else {
		CountNearbyPlayers(false);
		sprintf(macro_buf, "\xffz%d\xff", vars.numenemies);
	}
	suppress = true;
	return macro_buf;
}


char *Macro_CountNearbyFriendlyPlayers (void)
{
	if(!strncasecmp(Rulesets_Ruleset(), "MTFL", 4))
		return "banned by MTFL ruleset"; // there should be more smart way to do it

	CountNearbyPlayers(false);
	sprintf(macro_buf, "\xffz%d\xff", vars.numfriendlies);
	suppress = true;
	return macro_buf;
}


char *Macro_Count_Last_NearbyFriendlyPlayers (void)
{
	if(!strncasecmp(Rulesets_Ruleset(), "MTFL", 4))
		return "banned by MTFL ruleset"; // there should be more smart way to do it

	if (vars.deathtrigger_time && cls.realtime - vars.deathtrigger_time <= 5) {
		sprintf(macro_buf, "\xffz%d\xff", vars.last_numfriendlies);
	} else {
		CountNearbyPlayers(false);
		sprintf(macro_buf, "\xffz%d\xff", vars.numfriendlies);
	}
	suppress = true;
	return macro_buf;
}

// Note: longer macro names like "armortype" must be defined
// _before_ the shorter ones like "armor" to be parsed properly
void TP_AddMacros (void)
{
	int teamplay = (int) Rulesets_RestrictTriggers ();

	Cmd_AddMacro ("lastip", Macro_Lastip_f);
	Cmd_AddMacro ("qt", Macro_Quote_f);
	Cmd_AddMacro ("latency", Macro_Latency);
	Cmd_AddMacro ("ping", Macro_Latency);
	Cmd_AddMacro ("time", Macro_Time);
	Cmd_AddMacro ("date", Macro_Date);

	Cmd_AddMacroEx ("health", Macro_Health, teamplay);
	Cmd_AddMacroEx ("armortype", Macro_ArmorType, teamplay);
	Cmd_AddMacroEx ("armor", Macro_Armor, teamplay);

	Cmd_AddMacroEx ("shells", Macro_Shells, teamplay);
	Cmd_AddMacroEx ("nails", Macro_Nails, teamplay);
	Cmd_AddMacroEx ("rockets", Macro_Rockets, teamplay);
	Cmd_AddMacroEx ("cells", Macro_Cells, teamplay);

	Cmd_AddMacro ("weaponnum", Macro_WeaponNum);
	Cmd_AddMacroEx ("weapons", Macro_Weapons, teamplay);
	Cmd_AddMacro ("weapon", Macro_Weapon);

	Cmd_AddMacroEx ("ammo", Macro_Ammo, teamplay);

	Cmd_AddMacroEx ("bestweapon", Macro_BestWeapon, teamplay);
	Cmd_AddMacroEx ("bestammo", Macro_BestAmmo, teamplay);

	Cmd_AddMacroEx ("powerups", Macro_Powerups, teamplay);

	Cmd_AddMacroEx ("location", Macro_Location, teamplay);
	Cmd_AddMacroEx ("deathloc", Macro_LastDeath, teamplay);


	Cmd_AddMacroEx ("tookatloc", Macro_TookAtLoc, teamplay);
	Cmd_AddMacroEx ("tookloc", Macro_TookLoc, teamplay);
	Cmd_AddMacroEx ("took", Macro_Took, teamplay);

	Cmd_AddMacroEx ("pointatloc", Macro_PointNameAtLocation, teamplay);
	Cmd_AddMacroEx ("pointloc", Macro_PointLocation, teamplay);
	Cmd_AddMacroEx ("point", Macro_PointName, teamplay);

	Cmd_AddMacroEx ("need", Macro_Need, teamplay);

	Cmd_AddMacroEx ("droploc", Macro_LastDrop, teamplay);
	Cmd_AddMacroEx ("droptime", Macro_LastDropTime, teamplay);

	Cmd_AddMacro ("tf_skin", Macro_TF_Skin);

	Cmd_AddMacro ("triggermatch", Macro_LastTrigger_Match);


	Cmd_AddMacroEx ("ledpoint", Macro_Point_LED, teamplay);
	Cmd_AddMacroEx ("ledstatus", Macro_MyStatus_LED, teamplay);
}

/********************** MACRO/FUNCHAR/WHITE TEXT PARSING **********************/

wchar *TP_ParseWhiteText(wchar *s, qbool team, int offset)
{
	static wchar	buf[4096];
	wchar *out, *p, *p1;
	extern cvar_t	cl_parseWhiteText;
	qbool	parsewhite;

	parsewhite = cl_parseWhiteText.value == 1 || (cl_parseWhiteText.value == 2 && team);

	buf[0] = 0;
	out = buf;

	for (p = s; *p; p++) {
		if  (parsewhite && *p == '{' && p-s >= offset) {
			if ((p1 = qwcschr (p + 1, '}'))) {
				memcpy (out, p + 1, (p1 - p - 1)*sizeof(wchar));
				out += p1 - p - 1;
				p = p1;
				continue;
			}
		}
		if (*p != 10 && *p != 13 && !(p==s && (*p==1 || *p==2)))
			*out++ = *p | 128;	// convert to red
		else
			*out++ = *p;
	}
	*out = 0;
	return buf;
}

//Parses %a-like expressions
char *TP_ParseMacroString (char *s)
{
	static char	buf[MAX_MACRO_STRING];
	int i = 0;
	int pN, pn;
	char *macro_string;

	int r = 0;

	player_state_t *state;
	player_info_t *info;
	static int lastframecount = -1;

	if (!cl_parseSay.value)
		return s;

	suppress = false;
	pn = pN = 0;

	while (*s && i < MAX_MACRO_STRING - 1) {
		// check %[P], etc
		if (*s == '%' && s[1]=='[' && s[2] && s[3]==']') {
			static char mbuf[MAX_MACRO_VALUE];

			switch (s[2]) {
					case 'a':
					macro_string = Macro_ArmorType();
					if (!strcmp(macro_string, tp_name_none.string))
						macro_string = "a";
					if (cl.stats[STAT_ARMOR] < 30)
						snprintf (mbuf, sizeof(mbuf), "\x10%s:%i\x11", macro_string, cl.stats[STAT_ARMOR]);
					else
						snprintf (mbuf, sizeof(mbuf), "%s:%i", macro_string, cl.stats[STAT_ARMOR]);
					macro_string = mbuf;
					break;

					case 'h':
					if (cl.stats[STAT_HEALTH] >= 50)
						snprintf (macro_buf, sizeof(macro_buf), "%i", cl.stats[STAT_HEALTH]);
					else
						snprintf (macro_buf, sizeof(macro_buf), "\x10%i\x11", cl.stats[STAT_HEALTH]);
					macro_string = macro_buf;
					break;

					case 'p':
					case 'P':
					macro_string = Macro_Powerups();
					if (strcmp(macro_string, tp_name_none.string))
						snprintf (mbuf, sizeof(mbuf), "\x10%s\x11", macro_string);
					else
						mbuf[0] = 0;
					macro_string = mbuf;
					break;

					default:
					buf[i++] = *s++;
					continue;
			}
			if (i + strlen(macro_string) >= MAX_MACRO_STRING - 1)
				Sys_Error("TP_ParseMacroString: macro string length > MAX_MACRO_STRING)");
			strlcpy (&buf[i], macro_string, MAX_MACRO_STRING - i);
			i += strlen(macro_string);
			s += 4;	// skip %[<char>]
			continue;
		}

		// check %a, etc
		if (*s == '%') {
			switch (s[1]) {
					//case '\x7f': macro_string = ""; break;// skip cause we use this to hide mesgs
					//case '\xff': macro_string = ""; break;
					case 'n':   pn = 1; macro_string = ""; break;
					case 'N':   pN = 1; macro_string = ""; break;
					case 'a':	macro_string = Macro_Armor(); break;
					case 'A':	macro_string = Macro_ArmorType(); break;
					case 'b':	macro_string = Macro_BestWeaponAndAmmo(); break;
					case 'c':	macro_string = Macro_Cells(); break;
					case 'd':	macro_string = Macro_LastDeath(); break;
					case 'h':	macro_string = Macro_Health(); break;
					case 'i':	macro_string = Macro_TookAtLoc(); break;
					case 'j':	macro_string = Macro_LastPointAtLoc(); break;
					case 'k':	macro_string = Macro_LastTookOrPointed(); break;
					case 'l':	macro_string = Macro_Location(); break;
					case 'L':	macro_string = Macro_Last_Location(); break;
					case 'm':	macro_string = Macro_LastTookOrPointed(); break;

					case 'o':	macro_string = Macro_CountNearbyFriendlyPlayers(); break;
					case 'e':	macro_string = Macro_CountNearbyEnemyPlayers(); break;
					case 'O':	macro_string = Macro_Count_Last_NearbyFriendlyPlayers(); break;
					case 'E':	macro_string = Macro_Count_Last_NearbyEnemyPlayers(); break;

					case 'P':
					case 'p':	macro_string = Macro_Powerups(); break;
					case 'q':	macro_string = Macro_LastSeenPowerup(); break;
					case 'r':	macro_string = Macro_LastReportedLoc(); break;
					case 's':	macro_string = Macro_EnemyStatus_LED(); break;
					case 'S':	macro_string = Macro_TF_Skin(); break;
					case 't':	macro_string = Macro_PointNameAtLocation(); break;
					case 'u':	macro_string = Macro_Need(); break;
					case 'w':	macro_string = Macro_WeaponAndAmmo(); break;
					case 'x':	macro_string = Macro_PointName(); break;
					case 'X':	macro_string = Macro_Took(); break;
					case 'y':	macro_string = Macro_PointLocation(); break;
					case 'Y':	macro_string = Macro_TookLoc(); break;
					default:
					buf[i++] = *s++;
					continue;
			}
			if (i + strlen(macro_string) >= MAX_MACRO_STRING - 1)
				Sys_Error("TP_ParseMacroString: macro string length > MAX_MACRO_STRING)");
			strlcpy (&buf[i], macro_string, MAX_MACRO_STRING - i);
			i += strlen(macro_string);
			s += 2;	// skip % and letter
			continue;
		}
		buf[i++] = *s++;
	}
	buf[i] = 0;

	i = strlen(buf);

	if (pN) {
		buf[i++] = 0x7f;
		buf[i++] = '!';
		buf[i++] = 'A' + cl.playernum;
	}
	if (pn) {

		if (!pN)
			buf[i++] = 0x7f;

		if (cls.framecount != lastframecount) {

			lastframecount = cls.framecount;

			if (!(!cl.oldparsecount || !cl.parsecount || cls.state < ca_active)) {

				state = cl.frames[cl.oldparsecount & UPDATE_MASK].playerstate;
				info = cl.players;

				for (r = 0; r < MAX_CLIENTS; r++, info++, state++) {
					if (r != cl.playernum && state->messagenum == cl.oldparsecount && !info->spectator && !ISDEAD(state->frame)) {
						if (cl.teamplay && !strcmp(info->team, TP_PlayerTeam()))
							buf[i++] = 'A' + r;
					}
				}

			}

		}
	}

	if (suppress) {
		qbool quotes = false;

		TP_PrintHiddenMessage(buf,pN);

		i = strlen(buf);

		if (i > 0 && buf[i - 1] == '\"') {
			buf[i - 1] = 0;
			quotes = true;
			i--;
		}

		if (!pN) {
			if (!pn) {
				buf[i++] = 0x7f;
			}
			buf[i++] = '!';
			buf[i++] = 'A' + cl.playernum;
		}

		if (quotes)
			buf[i++] = '\"';
		buf[i] = 0;
	}


	return buf;
}

//Doesn't check for overflows, so strlen(s) should be < MAX_MACRO_STRING
char *TP_ParseFunChars (char *s, qbool chat)
{
	static char	 buf[MAX_MACRO_STRING];
	char		*out = buf;
	int			 c;

	if (!cl_parseFunChars.value)
		return s;

	while (*s) {
		if (*s == '$' && s[1] == 'x') {
			int i;
			// check for $x10, $x8a, etc
			c = tolower((int)(unsigned char)s[2]);
			if ( isdigit(c) )
				i = (c - (int)'0') << 4;
			else if ( isxdigit(c) )
				i = (c - (int)'a' + 10) << 4;
			else goto skip;
			c = tolower((int)(unsigned char)s[3]);
			if ( isdigit(c) )
				i += (c - (int)'0');
			else if ( isxdigit(c) )
				i += (c - (int)'a' + 10);
			else goto skip;
			if (!i)
				i = (int)' ';
			*out++ = (char)i;
			s += 4;
			continue;
		}
		if (*s == '$' && s[1]) {
			c = 0;
			switch (s[1]) {
					case '\\': c = 0x0D; break;
					case ':': c = 0x0A; break;
					case '[': c = 0x10; break;
					case ']': c = 0x11; break;
					case 'G': c = 0x86; break;
					case 'R': c = 0x87; break;
					case 'Y': c = 0x88; break;
					case 'B': c = 0x89; break;
					case '(': c = 0x80; break;
					case '=': c = 0x81; break;
					case ')': c = 0x82; break;
					case 'a': c = 0x83; break;
					case '<': c = 0x1d; break;
					case '-': c = 0x1e; break;
					case '>': c = 0x1f; break;
					case ',': c = 0x1c; break;
					case '.': c = 0x9c; break;
					case 'b': c = 0x8b; break;
					case 'c':
					case 'd': c = 0x8d; break;
					case '$': c = '$'; break;
					case '^': c = '^'; break;
			}
			if ( isdigit((int)(unsigned char)s[1]) )
				c = s[1] - (int)'0' + 0x12;
			if (c) {
				*out++ = (char)c;
				s += 2;
				continue;
			}
		}
		if (!chat && *s == '^' && s[1] && s[1] != ' ') {
			*out++ = s[1] | 128;
			s += 2;
			continue;
		}
	skip:
		*out++ = *s++;
	}
	*out = 0;

	return buf;
}

/************************* SKIN FORCING & REFRESHING *************************/

void MV_UpdateSkins()
{
	//
	// Multiview
	//
	char friendlyteam[MAX_INFO_STRING] = {'\0'};
	char tracked_team[MAX_INFO_STRING] = {'\0'};
	char *skinforcing_team = NULL;
	qbool trackingteam = false;
	int i;

	if (cls.state < ca_connected)
	{
		return;
	}

	//
	// Find out if we're tracking a team.
	//
	{
		// Get the team of the first slot.
		strlcpy(tracked_team, cl.players[mv_trackslots[0]].team, sizeof(tracked_team));

		for(i = 0; i < 4; i++)
		{
			// Ignore views that aren't tracking anything.
			if(mv_trackslots[i] < 0)
			{
				continue;
			}

			// Check if the team matches.
			if(!strcmp(cl.players[mv_trackslots[i]].team, tracked_team))
			{
				trackingteam = true;
				continue;
			}

			// If one of the slots team doesn't match with the others
			// then we're not tracking a team.
			trackingteam = false;
			break;
		}
	}

	//
	// There has been no skin force yet and we're not tracking a team
	// so we need to set all players colors.
	//
	if(!mv_skinsforced && !trackingteam)
	{
		// Only set the colors for all the players once, because
		// we're tracking multiple people... We can't know who's
		// a team member or an enemy.

		i = 0;

		// Pick the first team as the "team"-team. (uses teamcolor).
		while(!friendlyteam[0] && cl.players[i].team[0])
		{
			strlcpy(friendlyteam, cl.players[i].team, sizeof(friendlyteam));
			i++;
		}

		skinforcing_team = friendlyteam;
	}
	else if(trackingteam)
	{
		skinforcing_team = tracked_team;
	}

	// Set the colors based on team.
	if(skinforcing_team != NULL)
	{
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if(!cl.players[i].spectator && cl.players[i].name[0] && !cl.teamfortress && !(cl.fpd & FPD_NO_FORCE_COLOR))
			{
				if(strcmp(cl.players[i].team, skinforcing_team))
				{
					cl.players[i].topcolor = cl_enemytopcolor.value;
					cl.players[i].bottomcolor = cl_enemybottomcolor.value;
				}
				else
				{
					cl.players[i].topcolor = cl_teamtopcolor.value;
					cl.players[i].bottomcolor = cl_teambottomcolor.value;
				}
			}

			R_TranslatePlayerSkin(i);
		}

		mv_skinsforced = true;
	}
}

static qbool need_skin_refresh;
void TP_UpdateSkins(void)
{
	int slot;

	if (!need_skin_refresh)
	{
		return;
	}

	if(cls.mvdplayback && cl_multiview.value)
	{
		// Need to threat multiview completly different
		// since we are tracking more than one player
		// at once.
		MV_UpdateSkins();
	}
	else
	{
		for (slot = 0; slot < MAX_CLIENTS; slot++)
		{
			if (cl.players[slot].skin_refresh)
			{
				CL_NewTranslation(slot);
				cl.players[slot].skin_refresh = false;
			}
		}
	}

	need_skin_refresh = false;
}

qbool TP_NeedRefreshSkins(void)
{
	if (cl.teamfortress)
		return false;

	if ((cl_enemyskin.string[0] || cl_teamskin.string[0] || cl_enemypentskin.string[0] || cl_teampentskin.string[0] ||
	        cl_enemyquadskin.string[0] || cl_teamquadskin.string[0] || cl_enemybothskin.string[0] || cl_teambothskin.string[0])
	        && !(cl.fpd & FPD_NO_FORCE_SKIN))
		return true;

	if ((cl_teamtopcolor.value >= 0 || cl_enemytopcolor.value >= 0) && !(cl.fpd & FPD_NO_FORCE_COLOR))
		return true;

	return false;
}

void TP_RefreshSkin(int slot)
{
	if (cls.state < ca_connected || slot < 0 || slot >= MAX_CLIENTS || !cl.players[slot].name[0] || cl.players[slot].spectator)
		return;

	// Multiview
	// Never allow a skin refresh in multiview, since it
	// results in players getting the wrong color when
	// force colors is used (Team/enemycolor).
	// TODO: Any better solution for this?
	/*if(cls.mvdplayback && cl_multiview.value)
	{
		return;
	}*/

	cl.players[slot].skin_refresh = true;
	need_skin_refresh = true;
}

void TP_RefreshSkins(void)
{
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
		TP_RefreshSkin(i);
}

qbool OnChangeColorForcing(cvar_t *var, char *string)
{
	TP_RefreshSkins();
	return false;
}

qbool OnChangeSkinForcing(cvar_t *var, char *string)
{
	extern cvar_t noskins, cl_name_as_skin;

	if (cl.teamfortress || (cl.fpd & FPD_NO_FORCE_SKIN))
		return false;

	if (var == &cl_name_as_skin && (!cls.demoplayback && !cl.spectator))
		return false; // allow in demos or for specs

	if (cls.state == ca_active) {
		float oldskins;

		Cvar_Set(var, string);
		oldskins = noskins.value;
		noskins.value = 2;
		con_suppress = true;
		Skin_Skins_f();
		con_suppress = false;
		noskins.value = oldskins;
		return true;
	}
	return false;
}

void TP_ColorForcing (cvar_t *topcolor, cvar_t *bottomcolor)
{
	int	top, bottom;

	if (Cmd_Argc() == 1) {
		if (topcolor->value < 0)
			Com_Printf ("\"%s\" is \"off\"\n", Cmd_Argv(0));
		else
			Com_Printf ("\"%s\" is \"%i %i\"\n", Cmd_Argv(0), (int) topcolor->value, (int) bottomcolor->value);
		return;
	}

	if (!strcasecmp(Cmd_Argv(1), "off") || !strcasecmp(Cmd_Argv(1), "")) {
		topcolor->value = -1;
		TP_RefreshSkins();
		return;
	}

	if (Cmd_Argc() == 2) {
		top = bottom = atoi(Cmd_Argv(1));
	} else {
		top = atoi(Cmd_Argv(1));
		bottom = atoi(Cmd_Argv(2));
	}

	top &= 15;
	top = min(13, top);
	bottom &= 15;
	bottom = min(13, bottom);

	Cvar_SetValue(topcolor, top);
	Cvar_SetValue(bottomcolor, bottom);

	TP_RefreshSkins();
}

void TP_TeamColor_f(void)
{
	TP_ColorForcing(&cl_teamtopcolor, &cl_teambottomcolor);
}

void TP_EnemyColor_f(void)
{
	TP_ColorForcing(&cl_enemytopcolor, &cl_enemybottomcolor);
}

/********************************* .LOC FILES *********************************/

static locdata_t *locdata = NULL;
static int loc_count = 0;

static void TP_ClearLocs(void)
{
	locdata_t *node, *temp;

	for (node = locdata; node; node = temp) {
		Q_free(node->name);
		temp = node->next;
		Q_free(node);
	}

	locdata = NULL;
	loc_count = 0;
}

void TP_ClearLocs_f (void)
{
	int num_locs = 0;

	if (Cmd_Argc() > 1) {
		Com_Printf ("clearlocs : Clears all locs in memory.\n");
		return;
	}

	num_locs = loc_count;
	TP_ClearLocs ();

	Com_Printf (va("Cleared %d locs.\n", num_locs));
}

static void TP_AddLocNode(vec3_t coord, char *name)
{
	locdata_t *newnode, *node;

	newnode = (locdata_t *) Q_malloc(sizeof(locdata_t));
	newnode->name = Q_strdup(name);
	newnode->next = NULL;
	memcpy(newnode->coord, coord, sizeof(vec3_t));

	if (!locdata) {
		locdata = newnode;
		loc_count++;
		return;
	}

	for (node = locdata; node->next; node = node->next)
		;

	node->next = newnode;
	loc_count++;
}

#define SKIPBLANKS(ptr) while (*ptr == ' ' || *ptr == '\t' || *ptr == '\r') ptr++
#define SKIPTOEOL(ptr) {while (*ptr != '\n' && *ptr != 0) ptr++; if (*ptr == '\n') ptr++;}

qbool TP_LoadLocFile (char *path, qbool quiet)
{
	char *buf, *p, locname[MAX_OSPATH] = {0}, location[MAX_LOC_NAME];
	int i, n, sign, line, nameindex, mark, overflow;
	vec3_t coord;

	if (!*path)
		return false;

	strcpy (locname, "locs/");
	if (strlen(path) + strlen(locname) + 2 + 4 > MAX_OSPATH) {
		Com_Printf ("TP_LoadLocFile: path name > MAX_OSPATH\n");
		return false;
	}
	strncat (locname, path, sizeof(locname) - strlen(locname) - 1);
	COM_DefaultExtension(locname, ".loc");

	mark = Hunk_LowMark ();
	if (!(buf = (char *) FS_LoadHunkFile (locname))) {
		if (!quiet)
			Com_Printf ("Could not load %s\n", locname);
		return false;
	}

	TP_ClearLocs();

	// Parse the whole file now
	p = buf;
	line = 1;

	while (1) {
		SKIPBLANKS(p);

		if (!*p) {
			goto _endoffile;
		} else if (*p == '\n') {
			p++;
			goto _endofline;
		} else if (*p == '/' && p[1] == '/') {
			SKIPTOEOL(p);
			goto _endofline;
		}

		// parse three ints
		for (i = 0; i < 3; i++)	{
			n = 0;
			sign = 1;
			while (1) {
				switch (*p++) {
						case ' ': case '\t':
						goto _next;
						case '-':
						if (n) {
							Com_Printf ("Locfile error (line %d): unexpected '-'\n", line);
							SKIPTOEOL(p);
							goto _endofline;
						}
						sign = -1;
						break;
						case '0': case '1': case '2': case '3': case '4':
						case '5': case '6': case '7': case '8': case '9':
						n = n * 10 + (p[-1] - '0');
						break;
						default:	// including eol or eof
						Com_Printf ("Locfile error (line %d): couldn't parse coords\n", line);
						SKIPTOEOL(p);
						goto _endofline;
				}
			}
		_next:
			n *= sign;
			coord[i] = n / 8.0;

			SKIPBLANKS(p);
		}

		// parse location name
		overflow = nameindex = 0;
		while (1) {
			switch (*p)	{
					case '\r':
					p++;
					break;
					case '\n':
					case '\0':
					location[nameindex] = 0;
					TP_AddLocNode(coord, location);
					if (*p == '\n')
						p++;
					goto _endofline;
					default:
					if (nameindex < MAX_LOC_NAME - 1) {
						location[nameindex++] = *p;
					} else if (!overflow) {
						overflow = 1;
						Com_Printf ("Locfile warning (line %d): truncating loc name to %d chars\n", line, MAX_LOC_NAME - 1);
					}
					p++;
			}
		}
	_endofline:
		line++;
	}
_endoffile:

	Hunk_FreeToLowMark (mark);

	//if (loc_numentries) {
	if(loc_count) {
		if (!quiet)
			Com_Printf ("Loaded locfile \"%s\" (%i loc points)\n", COM_SkipPath(locname), loc_count); // loc_numentries);
	} else {
		TP_ClearLocs();
		if (!quiet)
			Com_Printf("Locfile \"%s\" was empty\n", COM_SkipPath(locname));
	}

	return true;
}

void TP_LoadLocFile_f (void)
{
	if (Cmd_Argc() != 2) {
		Com_Printf ("loadloc <filename> : load a loc file\n");
		return;
	}
	TP_LoadLocFile (Cmd_Argv(1), false);
}

qbool TP_SaveLocFile(char *path, qbool quiet)
{
	locdata_t	*node;
	char		*buf;
	char		locname[MAX_OSPATH];
	extern char com_gamedirfile[MAX_QPATH];

	// Make sure we have a path to work with.
	if (!*path) {
		return false;
	}

	// Check if the filename is too long.
	if(strlen(path) > MAX_LOC_NAME) {
		Com_Printf(va("TP_SaveLocFile: Filename too long. Max allowed is %d characters\n", MAX_LOC_NAME));
		return false;
	}

	// Get the default path for loc-files and make sure the path
	// won't be too long.
	strcpy (locname, va("%s/%s/", com_gamedirfile, "locs"));
	if (strlen(path) + strlen(locname) + 2 + 4 > MAX_OSPATH) {
		Com_Printf ("TP_SaveLocFile: path name > MAX_OSPATH\n");
		return false;
	}

	// Add an extension if it doesn't exist already.
	strncat (locname, path, sizeof(locname) - strlen(locname) - 1);
	COM_DefaultExtension(locname, ".loc");

	// Allocate a buffer to hold the file contents.
	buf = (char *)Q_calloc(loc_count * (MAX_LOC_NAME + 24), sizeof(char));

	if(!buf) {
		Com_Printf("TP_SaveLocFile: Could not initialize buffer.\n");
		return false;
	}

	// Write all the nodes to the buffer.
	for (node = locdata; node; node = node->next) {
		char row[2*MAX_LOC_NAME];

		sprintf(row, "%4d %4d %4d %s\n", Q_rint(8*node->coord[0]), Q_rint(8*node->coord[1]), Q_rint(8*node->coord[2]), node->name);
		strcat(buf, row);
	}

	// Try writing the buffer containing the locs to file.
	if(!COM_WriteFile(locname, buf, strlen(buf))) {
		if(!quiet) {
			Com_Printf(va("TP_SaveLocFile: Could not open %s for writing\n", locname));
		}

		// Make sure we free our buffer.
		Q_free(buf);

		return false;
	}

	// Make sure we free our buffer.
	Q_free(buf);

	return true;
}

void TP_SaveLocFile_f (void)
{
	if (Cmd_Argc() != 2) {
		Com_Printf ("saveloc <filename> : save a loc file\n");
		return;
	}
	TP_SaveLocFile (Cmd_Argv(1), false);
}

void TP_AddLoc(char *locname)
{
	vec3_t location;

	// We need to be up and running.
	if(cls.state != ca_active) {
		Com_Printf ("Need to be active to add a location.\n");
		return;
	}

	if (cl.players[cl.playernum].spectator && Cam_TrackNum() >= 0) {
		VectorCopy(cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK].playerstate[Cam_TrackNum()].origin, location);
	} else {
		VectorCopy(cl.simorg, location);
	}

	TP_AddLocNode(location, locname);

	Com_Printf (va("Added location \"%s\" at (%4.0f, %4.0f, %4.0f)\n", locname, location[0], location[1], location[2]));
}

void TP_AddLoc_f (void)
{
	if (Cmd_Argc() != 2) {
		Com_Printf ("addloc <name of location> : add a location\n");
		return;
	}
	TP_AddLoc (Cmd_Argv(1));
}

void TP_RemoveClosestLoc (vec3_t location)
{
	float dist, mindist;
	vec3_t vec;
	locdata_t *node, *best, *previous, *best_previous;

	best_previous = previous = best = NULL;
	mindist = 0;

	// Find the closest loc.
	node = locdata;
	while (node) {
		// Get the distance to the loc.
		VectorSubtract(location, node->coord, vec);
		dist = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];

		// Check if it's closer than the previously best.
		if (!best || dist < mindist) {
			best_previous = previous;
			best = node;
			mindist = dist;
		}

		// Advance and save the previous node.
		previous = node;
		node = node->next;
	}

	Com_Printf(va("Removed location \"%s\" at (%4.0f, %4.0f, %4.0f)\n", best->name, best->coord[0], best->coord[1], best->coord[2]));

	// If the node we're trying to delete has a
	// next node attached to it, copy the data from
	// that node into the node we're deleting, and then
	// delete the next node instead.
	if(best->next) {
		locdata_t *temp;

		// Copy the data from the next node into the one we're deleting.
		VectorCopy(best->next->coord, best->coord);

		Q_free(best->name);
		best->name = (char *)Q_calloc(strlen(best->next->name) + 1, sizeof(char));
		strcpy(best->name, best->next->name);

		// Save the pointer to the next node.
		temp = best->next->next;

		// Free the current next node.
		Q_free(best->next->name);
		Q_free(best->next);

		// Set the pointer to the next node.
		best->next = temp;
	} else {
		// Free the current node.
		Q_free(best->name);
		Q_free(best);
		best = NULL;

		// Make sure the previous node doesn't point to garbage.
		if(best_previous != NULL) {
			best_previous->next = NULL;
		}
	}

	// Decrease the loc count.
	loc_count--;

	// If this was the last loc, remove the entire node list.
	if(loc_count <= 0) {
		locdata = NULL;
	}
}

void TP_RemoveLoc_f (void)
{
	if (Cmd_Argc() == 1) {
		if (cl.players[cl.playernum].spectator && Cam_TrackNum() >= 0) {
			TP_RemoveClosestLoc (cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK].playerstate[Cam_TrackNum()].origin);
		} else {
			TP_RemoveClosestLoc (cl.simorg);
		}
	} else {
		Com_Printf ("removeloc : remove the closest location\n");
		return;
	}
}

typedef struct locmacro_s
{
	char *macro;
	char *val;
} locmacro_t;

static locmacro_t locmacros[] = {
                                    {"ssg", "ssg"},
                                    {"ng", "ng"},
                                    {"sng", "sng"},
                                    {"gl", "gl"},
                                    {"rl", "rl"},
                                    {"lg", "lg"},
                                    {"separator", "-"},
                                    {"ga", "ga"},
                                    {"ya", "ya"},
                                    {"ra", "ra"},
                                    {"quad", "quad"},
                                    {"pent", "pent"},
                                    {"ring", "ring"},
                                    {"suit", "suit"},
                                    {"mh", "mega"},
                                };

#define NUM_LOCMACROS	(sizeof(locmacros) / sizeof(locmacros[0]))

char *TP_LocationName(vec3_t location)
{
	char *in, *out, *value;
	int i;
	float dist, mindist;
	vec3_t vec;
	static locdata_t *node, *best;
	cvar_t *cvar;
	static qbool recursive;
	static char	buf[1024], newbuf[MAX_LOC_NAME];

	if (!locdata || cls.state != ca_active)
		return tp_name_someplace.string;

	if (recursive)
		return "";

	best = NULL;
	mindist = 0;

	for (node = locdata; node; node = node->next) {
		VectorSubtract(location, node->coord, vec);
		dist = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
		if (!best || dist < mindist) {
			best = node;
			mindist = dist;
		}
	}


	newbuf[0] = 0;
	out = newbuf;
	in = best->name;
	while (*in && out - newbuf < sizeof(newbuf) - 1) {
		if (!strncasecmp(in, "$loc_name_", 10)) {
			in += 10;
			for (i = 0; i < NUM_LOCMACROS; i++) {
				if (!strncasecmp(in, locmacros[i].macro, strlen(locmacros[i].macro))) {
					if ((cvar = Cvar_FindVar(va("loc_name_%s", locmacros[i].macro))))
						value = cvar->string;
					else
						value = locmacros[i].val;
					if (out - newbuf >= sizeof(newbuf) - strlen(value) - 1)
						goto done_locmacros;
					strcpy(out, value);
					out += strlen(value);
					in += strlen(locmacros[i].macro);
					break;
				}
			}
			if (i == NUM_LOCMACROS) {
				if (out - newbuf >= sizeof(newbuf) - 10 - 1)
					goto done_locmacros;
				strcpy(out, "$loc_name_");
				out += 10;
			}
		} else {
			*out++ = *in++;
		}
	}
done_locmacros:
	*out = 0;

	buf[0] = 0;
	recursive = true;
	Cmd_ExpandString(newbuf, buf);
	recursive = false;

	return buf;
}

/****************************** MESSAGE TRIGGERS ******************************/

typedef struct msg_trigger_s
{
	char	name[32];
	char	string[64];
	int		level;
	struct msg_trigger_s *next;
} msg_trigger_t;

static msg_trigger_t *msg_triggers;

void TP_ResetAllTriggers (void)
{
	msg_trigger_t *temp;

	while (msg_triggers) {
		temp = msg_triggers->next;
		Q_free(msg_triggers);
		msg_triggers = temp;
	}
}

void TP_DumpTriggers (FILE *f)
{
	msg_trigger_t *t;

	for (t = msg_triggers; t; t = t->next) {
		if (t->level == PRINT_HIGH)
			fprintf(f, "msg_trigger  %s \"%s\"\n", t->name, t->string);
		else
			fprintf(f, "msg_trigger  %s \"%s\" -l %c\n", t->name, t->string, t->level == 4 ? 't' : '0' + t->level);
	}
}

msg_trigger_t *TP_FindTrigger (char *name)
{
	msg_trigger_t *t;

	for (t = msg_triggers; t; t = t->next)
		if (!strcmp(t->name, name))
			return t;

	return NULL;
}

void TP_MsgTrigger_f (void)
{
	int c;
	char *name;
	msg_trigger_t *trig;

	c = Cmd_Argc();

	if (c > 5) {
		Com_Printf ("msg_trigger <trigger name> \"string\" [-l <level>]\n");
		return;
	}

	if (c == 1) {
		if (!msg_triggers)
			Com_Printf ("no triggers defined\n");
		else
			for (trig=msg_triggers; trig; trig=trig->next)
				Com_Printf ("%s : \"%s\"\n", trig->name, trig->string);
		return;
	}

	name = Cmd_Argv(1);
	if (strlen(name) > 31) {
		Com_Printf ("trigger name too long\n");
		return;
	}

	if (c == 2) {
		trig = TP_FindTrigger (name);
		if (trig)
			Com_Printf ("%s: \"%s\"\n", trig->name, trig->string);
		else
			Com_Printf ("trigger \"%s\" not found\n", name);
		return;
	}

	if (c >= 3) {
		if (strlen(Cmd_Argv(2)) > 63) {
			Com_Printf ("trigger string too long\n");
			return;
		}

		if (!(trig = TP_FindTrigger (name))) {
			// allocate new trigger
			trig = (msg_trigger_t *) Q_malloc (sizeof(msg_trigger_t));
			trig->next = msg_triggers;
			msg_triggers = trig;
			strcpy (trig->name, name);
			trig->level = PRINT_HIGH;
		}

		strlcpy (trig->string, Cmd_Argv(2), sizeof(trig->string));
		if (c == 5 && !strcasecmp (Cmd_Argv(3), "-l")) {
			if (!strcmp(Cmd_Argv(4), "t")) {
				trig->level = 4;
			} else {
				trig->level = Q_atoi (Cmd_Argv(4));
				if ((unsigned) trig->level > PRINT_CHAT)
					trig->level = PRINT_HIGH;
			}
		}
	}
}

static qbool TP_IsFlagMessage(char *message)
{
	if (strstr(message, " has your key!") ||
	        strstr(message, " has taken your Key") ||
	        strstr(message, " has your flag") ||
	        strstr(message, " took your flag!") ||
	        strstr(message, " &#65533;&#65533; &#65533;&#65533; flag!") ||
	        strstr(message, " &#65533;&#65533;&#65533;&#65533; &#65533;&#65533;") || strstr(message, " &#65533;&#65533;&#65533;&#65533;&#65533;&#65533;&#65533;") ||
	        strstr(message, " took the blue flag") || strstr(message, " took the red flag") ||
	        strstr(message, " Has the Red Flag") || strstr(message, " Has the Blue Flag")
	   )
		return true;

	return false;
}

void TP_SearchForMsgTriggers (char *s, int level)
{
	msg_trigger_t	*t;
	char *string;

	// message triggers disabled
	if (!tp_msgtriggers.value)
		return;

	// triggers banned by ruleset
	if (Rulesets_RestrictTriggers () && !cls.demoplayback && !cl.spectator)
		return;

	// we are in spec/demo mode, so play triggers if user want it
	if ((cls.demoplayback || cl.spectator) && cl_restrictions.value)
		return;

	for (t = msg_triggers; t; t = t->next) {
		if ((t->level == level || (t->level == 3 && level == 4)) && t->string[0] && strstr(s, t->string)) {
			if (level == PRINT_CHAT && (
			            strstr (s, "f_version") || strstr (s, "f_skins") || strstr(s, "f_fakeshaft") ||
			            strstr (s, "f_server") || strstr (s, "f_scripts") || strstr (s, "f_cmdline") ||
			            strstr (s, "f_system") || strstr (s, "f_speed") || strstr (s, "f_modified"))
			   )
				continue; // don't let llamas fake proxy replies

			if (cl.teamfortress && level == PRINT_HIGH && TP_IsFlagMessage(s))
				continue;

			if ((string = Cmd_AliasString (t->name))) {
				strlcpy(vars.lasttrigger_match, s, sizeof(vars.lasttrigger_match));
				Cbuf_AddTextEx (&cbuf_safe, va("%s\n", string));
			} else {
				Com_Printf ("trigger \"%s\" has no matching alias\n", t->name);
			}
		}
	}
}

/**************************** REGEXP TRIGGERS *********************************/

typedef void ReTrigger_func (pcre_trigger_t *);

static void Trig_ReSearch_do (ReTrigger_func f)
{
	pcre_trigger_t *trig;

	for( trig = re_triggers; trig; trig = trig->next) {
		if (ReSearchMatch (trig->name))
			f (trig);
	}
}

static pcre_trigger_t *prev;
pcre_trigger_t *CL_FindReTrigger (char *name)
{
	pcre_trigger_t *t;

	prev=NULL;
	for (t=re_triggers; t; t=t->next) {
		if (!strcmp(t->name, name))
			return t;

		prev = t;
	}
	return NULL;
}

static void DeleteReTrigger (pcre_trigger_t *t)
{
	if (t->regexp) (pcre_free)(t->regexp);
	if (t->regexp_extra) (pcre_free)(t->regexp_extra);
	if (t->regexpstr) Q_free(t->regexpstr);
	Q_free(t->name);
	Q_free(t);
}

static void RemoveReTrigger (pcre_trigger_t *t)
{
	// remove from list
	if (prev)
		prev->next = t->next;
	else
		re_triggers = t->next;
	// free memory
	DeleteReTrigger(t);
}

static void CL_RE_Trigger_f (void)
{
	int c,i,m;
	char *name;
	char *regexpstr;
	pcre_trigger_t *trig;
	pcre *re;
	pcre_extra *re_extra;
	const char *error;
	int error_offset;
	qbool newtrigger=false;
	qbool re_search = false;

	c = Cmd_Argc();
	if (c > 3) {
		Com_Printf ("re_trigger <trigger name> <regexp>\n");
		return;
	}

	if (c == 2 && IsRegexp(Cmd_Argv(1))) {
		re_search = true;
	}

	if (c == 1 || re_search) {
		if (!re_triggers) {
			Com_Printf ("no regexp_triggers defined\n");
		} else {
			if (re_search && !ReSearchInit(Cmd_Argv(1)))
				return;

			Com_Printf ("List of re_triggers:\n");

			for (trig=re_triggers, i=m=0; trig; trig=trig->next, i++) {
				if (!re_search || ReSearchMatch(trig->name)) {
					Com_Printf ("%s : \"%s\" : %d\n", trig->name, trig->regexpstr, trig->counter);
					m++;
				}
			}

			Com_Printf ("------------\n%i/%i re_triggers\n", m, i);
			if (re_search)
				ReSearchDone();
		}
		return;
	}

	name = Cmd_Argv(1);
	trig = CL_FindReTrigger (name);

	if (c == 2) {
		if (trig) {
			Com_Printf ("%s: \"%s\"\n", trig->name, trig->regexpstr);
			Com_Printf ("  options: mask=%d interval=%g%s%s%s%s%s\n", trig->flags & 0xFF,
			            trig->min_interval,
			            trig->flags & RE_FINAL ? " final" : "",
			            trig->flags & RE_REMOVESTR ? " remove" : "",
			            trig->flags & RE_NOLOG ? " nolog" : "",
			            trig->flags & RE_ENABLED ? "" : " disabled",
			            trig->flags & RE_NOACTION ? " noaction" : ""
			           );
			Com_Printf ("  matched %d times\n", trig->counter);
		} else {
			Com_Printf ("re_trigger \"%s\" not found\n", name);
		}
		return;
	}

	if (c == 3) {
		regexpstr = Cmd_Argv(2);
		if (!trig) {
			// allocate new trigger
			newtrigger = true;
			trig = (pcre_trigger_t *) Q_malloc (sizeof(pcre_trigger_t));
			trig->next = re_triggers;
			re_triggers = trig;
			trig->name = Q_strdup (name);
			trig->flags = RE_PRINT_ALL | RE_ENABLED; // catch all printed messages by default
		}

		error = NULL;
		if ((re = pcre_compile(regexpstr, 0, &error, &error_offset, NULL))) {
			error = NULL;
			re_extra = pcre_study(re, 0, &error);
			if (error) {
				Com_Printf ("Regexp study error: %s\n", &error);
			} else {
				if (!newtrigger) {
					(pcre_free)(trig->regexp);
					if (trig->regexp_extra)
						(pcre_free)(trig->regexp_extra);
					Q_free(trig->regexpstr);
				}
				trig->regexpstr = Q_strdup (regexpstr);
				trig->regexp = re;
				trig->regexp_extra = re_extra;
				return;
			}
		} else {
			Com_Printf ("Invalid regexp: %s\n", error);
		}
		prev = NULL;
		RemoveReTrigger(trig);
	}
}

static void CL_RE_Trigger_Options_f (void)
{
	int c,i;
	char* name;
	pcre_trigger_t *trig;

	c = Cmd_Argc ();
	if (c < 3) {
		Com_Printf ("re_trigger_options <trigger name> <option1> <option2>\n");
		return;
	}

	name = Cmd_Argv (1);
	trig = CL_FindReTrigger (name);

	if (!trig) {
		Com_Printf ("re_trigger \"%s\" not found\n", name);
		return;
	}

	for(i=2; i<c; i++) {
		if (!strcmp(Cmd_Argv(i), "final")) {
			trig->flags |= RE_FINAL;
		} else if (!strcmp(Cmd_Argv(i), "remove")) {
			trig->flags |= RE_REMOVESTR;
		} else if (!strcmp(Cmd_Argv(i), "notfinal")) {
			trig->flags &= ~RE_FINAL;
		} else if (!strcmp(Cmd_Argv(i), "noremove")) {
			trig->flags &= ~RE_REMOVESTR;
		} else if (!strcmp(Cmd_Argv(i), "mask")) {
			trig->flags &= ~0xFF;
			trig->flags |= 0xFF & atoi(Cmd_Argv(i+1));
			i++;
		} else if (!strcmp(Cmd_Argv(i), "interval") ) {
			trig->min_interval = atof(Cmd_Argv(i+1));
			i++;
		} else if (!strcmp(Cmd_Argv(i), "enable")) {
			trig->flags |= RE_ENABLED;
		} else if (!strcmp(Cmd_Argv(i), "disable")) {
			trig->flags &= ~RE_ENABLED;
		} else if (!strcmp(Cmd_Argv(i), "noaction")) {
			trig->flags |= RE_NOACTION;
		} else if (!strcmp(Cmd_Argv(i), "action")) {
			trig->flags &= ~RE_NOACTION;
		} else if (!strcmp(Cmd_Argv(i), "nolog")) {
			trig->flags |= RE_NOLOG;
		} else if (!strcmp(Cmd_Argv(i), "log")) {
			trig->flags &= ~RE_NOLOG;
		} else {
			Com_Printf("re_trigger_options: invalid option.\n"
			           "valid options:\n  final\n  notfinal\n  remove\n"
			           "  noremove\n  mask <trigger_mask>\n  interval <min_interval>)\n"
			           "  enable\n  disable\n  noaction\n  action\n  nolog\n  log\n");
		}
	}
}

static void CL_RE_Trigger_Delete_f (void)
{
	pcre_trigger_t *trig, *next_trig;
	char *name;
	int i;

	for (i = 1; i < Cmd_Argc(); i++) {
		name = Cmd_Argv(i);
		if (IsRegexp(name)) {
			if(!ReSearchInit(name))
				return;
			prev = NULL;
			for (trig = re_triggers; trig; ) {
				if (ReSearchMatch (trig->name)) {
					next_trig = trig->next;
					RemoveReTrigger(trig);
					trig = next_trig;
				} else {
					prev = trig;
					trig = trig->next;
				}
			}
			ReSearchDone();
		} else {
			if ((trig = CL_FindReTrigger(name)))
				RemoveReTrigger(trig);
		}
	}
}

static void Trig_Enable(pcre_trigger_t *trig)
{
	trig->flags |= RE_ENABLED;
}

static void CL_RE_Trigger_Enable_f (void)
{
	pcre_trigger_t *trig;
	char *name;
	int i;

	for (i = 1; i < Cmd_Argc(); i++) {
		name = Cmd_Argv (i);
		if (IsRegexp (name)) {
			if(!ReSearchInit (name))
				return;
			Trig_ReSearch_do (Trig_Enable);
			ReSearchDone ();
		} else {
			if ((trig = CL_FindReTrigger (name)))
				Trig_Enable (trig);
		}
	}
}

static void Trig_Disable (pcre_trigger_t *trig)
{
	trig->flags &= ~RE_ENABLED;
}

static void CL_RE_Trigger_Disable_f (void)
{
	pcre_trigger_t *trig;
	char *name;
	int i;

	for (i = 1; i < Cmd_Argc (); i++) {
		name = Cmd_Argv (i);
		if (IsRegexp (name)) {
			if(!ReSearchInit (name))
				return;
			Trig_ReSearch_do (Trig_Disable);
			ReSearchDone ();
		} else {
			if ((trig = CL_FindReTrigger (name)))
				Trig_Disable (trig);
		}
	}
}

void CL_RE_Trigger_ResetLasttime (void)
{
	pcre_trigger_t *trig;

	for (trig=re_triggers; trig; trig=trig->next)
		trig->lasttime = 0.0;
}

void Re_Trigger_Copy_Subpatterns (char *s, int* offsets, int num, cvar_t	*re_sub)
{
	int	i;
	char tmp;

	for (i=0; i < 2 * num; i += 2) {
		tmp = s[offsets[i + 1]];
		s[offsets[i + 1]] = '\0';
		Cvar_ForceSet(&re_sub[i / 2],s + offsets[i]);
		s[offsets[i + 1]] = tmp;
	}
}

static void CL_RE_Trigger_Match_f (void)
{
	int c;
	char *tr_name;
	char *s;
	pcre_trigger_t *rt;
	char *string;
	int result;
	int offsets[99];

	c = Cmd_Argc();

	if (c != 3) {
		Com_Printf ("re_trigger_match <trigger name> <string>\n");
		return;
	}

	tr_name = Cmd_Argv(1);
	s = Cmd_Argv(2);

	for (rt = re_triggers; rt; rt = rt->next)
		if (!strcmp(rt->name, tr_name)) {
			result = pcre_exec (rt->regexp, rt->regexp_extra, s, strlen(s), 0, 0, offsets, 99);

			if (result >= 0) {
				rt->lasttime = cls.realtime;
				rt->counter++;
				Re_Trigger_Copy_Subpatterns (s, offsets, min (result,10), re_sub);

				if (!(rt->flags & RE_NOACTION)) {
					string = Cmd_AliasString (rt->name);
					if (string) {
						Cbuf_InsertTextEx (&cbuf_safe, "\nwait\n");
						Cbuf_InsertTextEx (&cbuf_safe, string);
						Cbuf_ExecuteEx (&cbuf_safe);
					} else {
						Com_Printf ("re_trigger \"%s\" has no matching alias\n", rt->name);
					}
				}
			}
			return;
		}
	Com_Printf ("re_trigger \"%s\" not found\n", tr_name);
}

qbool allow_re_triggers;
qbool CL_SearchForReTriggers (char *s, unsigned trigger_type)
{
	pcre_trigger_t *rt;
	pcre_internal_trigger_t *irt;
	cmd_alias_t *trig_alias;
	qbool removestr = false;
	int result;
	int offsets[99];
	int len = strlen(s);

	// internal triggers - always enabled
	if (trigger_type < RE_PRINT_ECHO) {
		allow_re_triggers = true;
		for (irt = internal_triggers; irt; irt = irt->next) {
			if (irt->flags & trigger_type) {
				result = pcre_exec (irt->regexp, irt->regexp_extra, s, len, 0, 0, offsets, 99);
				if (result >= 0) {
					Re_Trigger_Copy_Subpatterns (s, offsets, min(result,10), re_subi);
					irt->func (s);
				}
			}
		}
		if (!allow_re_triggers)
			return false;
	}

	// message triggers disabled
	if (!tp_msgtriggers.value)
		return false;

	// triggers banned by ruleset or FPD and we are a player
	if (((cl.fpd & FPD_NO_SOUNDTRIGGERS) || (cl.fpd & FPD_NO_TIMERS) ||
	        Rulesets_RestrictTriggers ()) && !cls.demoplayback && !cl.spectator)
		return false;

	// we are in spec/demo mode, so play triggers if user want it
	if ((cls.demoplayback || cl.spectator) && cl_restrictions.value)
		return false;

	// regexp triggers
	for (rt = re_triggers; rt; rt = rt->next)
		if ( (rt->flags & RE_ENABLED) &&	// enabled
		        (rt->flags & trigger_type) &&	// mask fits
		        rt->regexp &&					// regexp not empty
		        (rt->min_interval == 0.0 ||
		         cls.realtime >= rt->min_interval + rt->lasttime)) // not too fast.
			// TODO: disable it ^^^ for FPD_NO_TIMERS case.
			// probably it dont solve re_trigger timers problem
			// you always trigger on statusbar(TF) or wp_stats (KTPro/KTX) messages and get 0.5~1.5 accuracy for your timer
		{
			result = pcre_exec (rt->regexp, rt->regexp_extra, s, len, 0, 0, offsets, 99);
			if (result >= 0) {
				rt->lasttime = cls.realtime;
				rt->counter++;
				Re_Trigger_Copy_Subpatterns (s, offsets, min(result,10), re_sub);

				if (!(rt->flags & RE_NOACTION)) {
					trig_alias = Cmd_FindAlias (rt->name);
					Print_current++;
					if (trig_alias) {
						Cbuf_InsertTextEx (&cbuf_safe, "\nwait\n");
						Cbuf_InsertTextEx (&cbuf_safe, rt->name);
						Cbuf_ExecuteEx (&cbuf_safe);
					} else
						Com_Printf ("re_trigger \"%s\" has no matching alias\n", rt->name);
					Print_current--;
				}

				if (rt->flags & RE_REMOVESTR)
					removestr = true;
				if (rt->flags & RE_NOLOG)
					Print_flags[Print_current] |= PR_LOG_SKIP;
				if (rt->flags & RE_FINAL)
					break;
			}
		}

	if (removestr)
		Print_flags[Print_current] |= PR_SKIP;

	return removestr;
}

// Internal triggers
static void AddInternalTrigger (char* regexpstr, unsigned mask, internal_trigger_func func)
{
	pcre_internal_trigger_t *trig;
	const char *error;
	int error_offset;

	trig = (pcre_internal_trigger_t *) Q_malloc (sizeof(pcre_internal_trigger_t));
	trig->next = internal_triggers;
	internal_triggers = trig;

	trig->regexp = pcre_compile (regexpstr, 0, &error, &error_offset, NULL);
	trig->regexp_extra = pcre_study (trig->regexp, 0, &error);
	trig->func = func;
	trig->flags = mask;
}

static void INTRIG_Disable (char *s)
{
	allow_re_triggers = false;
	Print_flags[Print_current] |= PR_LOG_SKIP;
}

static void INTRIG_Lastip_port (char *s)
{
	/* subpatterns of this regexp is maximum 21 chars */
	/* strlen (<3>.<3>.<3>.<3>:< 5 >) = 21 */

	// reset current lastip value
	memset (lastip, 0, sizeof (lastip));

	memcpy (lastip, va ("%s.%s.%s.%s:%s",
	                    re_subi[1].string,
	                    re_subi[2].string,
	                    re_subi[3].string,
	                    re_subi[4].string,
	                    re_subi[5].string), sizeof (lastip));
}

static void InitInternalTriggers(void)
{
	// dont allow cheating by triggering showloc command
	AddInternalTrigger("^(Location :|Angles   :)", 4, INTRIG_Disable); // showloc command
	// dont allow cheating by triggering dispenser warning
	AddInternalTrigger("^Enemies are using your dispenser!$", 16, INTRIG_Disable);
	// lastip
	AddInternalTrigger("([0-9]|1?\\d\\d|2[0-4]\\d|25[0-5])\\.([0-9]|1?\\d\\d|2[0-4]\\d|25[0-5])\\.([0-9]|1?\\d\\d|2[0-4]\\d|25[0-5])\\.([0-9]|1?\\d\\d|2[0-4]\\d|25[0-5])\\:(\\d{5})", 8, INTRIG_Lastip_port);
}

static void TP_InitReTriggers()
{
	unsigned i;

	for(i=0;i<10;i++)
		Cvar_Register (re_sub+i);

	Cmd_AddCommand ("re_trigger", CL_RE_Trigger_f);
	Cmd_AddCommand ("re_trigger_options", CL_RE_Trigger_Options_f);
	Cmd_AddCommand ("re_trigger_delete", CL_RE_Trigger_Delete_f);
	Cmd_AddCommand ("re_trigger_enable", CL_RE_Trigger_Enable_f);
	Cmd_AddCommand ("re_trigger_disable", CL_RE_Trigger_Disable_f);
	Cmd_AddCommand ("re_trigger_match", CL_RE_Trigger_Match_f);
	InitInternalTriggers();
}

/************************* BASIC MATCH INFO FUNCTIONS *************************/

char *TP_PlayerName (void)
{
	static char myname[MAX_INFO_STRING];

	strcpy (myname, Info_ValueForKey(cl.players[cl.playernum].userinfo, "name"));
	return myname;
}

char *TP_PlayerTeam (void)
{
	static char myteam[MAX_INFO_STRING];

	strcpy (myteam, cl.players[cl.playernum].team);
	return myteam;
}

int	TP_CountPlayers (void)
{
	int	i, count = 0;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (cl.players[i].name[0] && !cl.players[i].spectator)
			count++;
	}
	return count;
}

char *TP_MapName(void)
{
	return host_mapname.string;
}

// START shaman RFE 1020608
#ifdef GLQUAKE
char *MT_GetSkyGroupName(char *mapname, qbool *system);

char *TP_GetSkyGroupName(char *mapname, qbool *system)
{
	return MT_GetSkyGroupName(mapname, system);
}
#endif
// END shaman RFE 1020608

char *MT_GetMapGroupName(char *mapname, qbool *system);

char *TP_GetMapGroupName(char *mapname, qbool *system)
{
	return MT_GetMapGroupName(mapname, system);
}

/****************************** PUBLIC FUNCTIONS ******************************/

void TP_NewMap (void)
{
	static char last_map[MAX_QPATH] = {0};
	char *groupname, *mapname;
	qbool system;

	memset (&vars, 0, sizeof(vars));
	TP_FindModelNumbers ();

	mapname = TP_MapName();
	if (strcmp(mapname, last_map)) {	// map name has changed
		TP_ClearLocs();					// clear loc file
		if (tp_loadlocs.value && cl.deathmatch && !cls.demoplayback) {
			if (!TP_LoadLocFile (va("%s.loc", mapname), true)) {
				if ((groupname = TP_GetMapGroupName(mapname, &system)) && !system)
					TP_LoadLocFile (va("%s.loc", groupname), true);
			}
			strlcpy (last_map, mapname, sizeof(last_map));
		} else {
			last_map[0] = 0;
		}
	}
	TP_ExecTrigger ("f_newmap");
	if (cl.teamfortress)
		V_TF_ClearGrenadeEffects();
	Ignore_ResetFloodList();
}

/*
returns a combination of these values:
0 -- unknown (probably generated by the server)
1 -- normal
2 -- team message
4 -- spectator
8 -- spec team message
Note that sometimes we can't be sure who really sent the message,  e.g. when there's a
player "unnamed" in your team and "(unnamed)" in the enemy team. The result will be 3 (1+2)
*/
int TP_CategorizeMessage (char *s, int *offset)
{
	int i, msglen, len, flags, tracknum;
	player_info_t	*player;
	char *name, *team=NULL;


	tracknum = -1;
	if (cl.spectator && (tracknum = Cam_TrackNum()) != -1)
		team = cl.players[tracknum].team;
	else if (!cl.spectator)
		team = cl.players[cl.playernum].team;

	flags = 0;
	*offset = 0;
	if (!(msglen = strlen(s)))
		return 0;

	for (i = 0, player = cl.players; i < MAX_CLIENTS; i++, player++)	{
		if (!player->name[0])
			continue;
		name = Info_ValueForKey (player->userinfo, "name");
		len = bound(0, strlen(name), 31);
		// check messagemode1
		if (len + 2 <= msglen && s[len] == ':' && s[len + 1] == ' ' && !strncmp(name, s, len))	{
			if (player->spectator)
				flags |= 4;
			else
				flags |= 1;
			*offset = len + 2;
		}
		// check messagemode2
		else if (s[0] == '(' && len + 4 <= msglen && !strncmp(s + len + 1, "): ", 3) && !strncmp(name, s + 1, len)

		         && (!cl.spectator || tracknum != -1)
		        ) {
			// no team messages in teamplay 0, except for our own
			if (i == cl.playernum || ( cl.teamplay && !strcmp(team, player->team)) )
				flags |= 2;
			*offset = len + 4;
		}
		//check spec mm2
		else if (cl.spectator && !strncmp(s, "[SPEC] ", 7) && player->spectator &&
		         len + 9 <= msglen && s[len + 7] == ':' && s[len + 8] == ' ' && !strncmp(name, s + 7, len)) {
			flags |= 8;
			*offset = len + 9;
		}
	}
	return flags;
}

/****************************** POINTING & TOOK ******************************/

// symbolic names used in tp_took, tp_pickup, tp_point commands
char *pknames[] = {"quad", "pent", "ring", "suit", "ra", "ya",	"ga",
                   "mh", "health", "lg", "rl", "gl", "sng", "ng", "ssg", "pack",
                   "cells", "rockets", "nails", "shells", "flag",
                   "teammate", "enemy", "eyes", "sentry", "disp", "runes"};

#define it_quad		(1 << 0)
#define it_pent		(1 << 1)
#define it_ring		(1 << 2)
#define it_suit		(1 << 3)
#define it_ra		(1 << 4)
#define it_ya		(1 << 5)
#define it_ga		(1 << 6)
#define it_mh		(1 << 7)
#define it_health	(1 << 8)
#define it_lg		(1 << 9)
#define it_rl		(1 << 10)
#define it_gl		(1 << 11)
#define it_sng		(1 << 12)
#define it_ng		(1 << 13)
#define it_ssg		(1 << 14)
#define it_pack		(1 << 15)
#define it_cells	(1 << 16)
#define it_rockets	(1 << 17)
#define it_nails	(1 << 18)
#define it_shells	(1 << 19)
#define it_flag		(1 << 20)
#define it_teammate	(1 << 21)
#define it_enemy	(1 << 22)
#define it_eyes		(1 << 23)
#define it_sentry   (1 << 24)
#define it_disp		(1 << 25)
#define it_runes	(1 << 26)
#define NUM_ITEMFLAGS 27

#define it_powerups	(it_quad|it_pent|it_ring)
#define it_weapons	(it_lg|it_rl|it_gl|it_sng|it_ng|it_ssg)
#define it_armor	(it_ra|it_ya|it_ga)
#define it_ammo		(it_cells|it_rockets|it_nails|it_shells)
#define it_players	(it_teammate|it_enemy|it_eyes)

#define default_pkflags (it_powerups|it_suit|it_armor|it_weapons|it_mh| \
				it_rockets|it_pack|it_flag)

#define default_tookflags (it_powerups|it_ra|it_ya|it_lg|it_rl|it_mh|it_flag)

#define default_pointflags (it_powerups|it_suit|it_armor|it_mh| \
				it_lg|it_rl|it_gl|it_sng|it_rockets|it_pack|it_flag|it_players)

int pkflags = default_pkflags;
int tookflags = default_tookflags;
int pointflags = default_pointflags;
byte pointpriorities[NUM_ITEMFLAGS];

static void DumpFlagCommand(FILE *f, char *name, int flags, int default_flags)
{
	int i, all_flags = (1 << NUM_ITEMFLAGS) - 1;

	fprintf(f, "%s ", name);

	if (flags == 0) {
		fprintf(f, "none\n");
		return;
	}
	if (flags == all_flags) {
		fprintf(f, "all\n");
		return;
	} else if (flags == default_flags) {
		fprintf(f, "default\n");
		return;
	}

	if ((flags & it_powerups) == it_powerups) {
		fprintf(f, "powerups ");
		flags &= ~it_powerups;
	}
	if ((flags & it_weapons) == it_weapons) {
		fprintf(f, "weapons ");
		flags &= ~it_weapons;
	}
	if ((flags & it_armor) == it_armor) {
		fprintf(f, "armor ");
		flags &= ~it_armor;
	}
	if ((flags & it_ammo) == it_ammo) {
		fprintf(f, "ammo ");
		flags &= ~it_ammo;
	}
	if ((flags & it_players) == it_players) {
		fprintf(f, "players ");
		flags &= ~it_players;
	}
	for (i = 0; i < NUM_ITEMFLAGS; i++) {
		if (flags & (1 << i))
			fprintf (f, "%s ", pknames[i]);
	}
	fprintf(f, "\n");
}

void DumpFlagCommands(FILE *f)
{
	DumpFlagCommand(f, "tp_pickup   ", pkflags, default_pkflags);
	DumpFlagCommand(f, "tp_took     ", tookflags, default_tookflags);
	DumpFlagCommand(f, "tp_point    ", pointflags, default_pointflags);
}

static void FlagCommand (int *flags, int defaultflags)
{
	int i, j, c, flag;
	char *p, str[255] = {0};
	qbool removeflag = false;

	c = Cmd_Argc ();
	if (c == 1)	{
		if (!*flags)
			strcpy (str, "none");
		for (i = 0 ; i < NUM_ITEMFLAGS ; i++)
			if (*flags & (1 << i)) {
				if (*str)
					strncat (str, " ", sizeof(str) - strlen(str) - 1);
				strncat (str, pknames[i], sizeof(str) - strlen(str) - 1);
			}
		Com_Printf ("%s\n", str);
		return;
	}

	if (c == 2 && !strcasecmp(Cmd_Argv(1), "none")) {
		*flags = 0;
		return;
	}

	if (*Cmd_Argv(1) != '+' && *Cmd_Argv(1) != '-')
		*flags = 0;

	for (i = 1; i < c; i++) {
		p = Cmd_Argv (i);
		if (*p == '+') {
			removeflag = false;
			p++;
		} else if (*p == '-') {
			removeflag = true;
			p++;
		}

		flag = 0;
		for (j=0 ; j<NUM_ITEMFLAGS ; j++) {
			if (!strncasecmp (p, pknames[j], 3)) {
				flag = 1<<j;
				break;
			}
		}

		if (!flag) {
			if (!strcasecmp (p, "armor"))
				flag = it_armor;
			else if (!strcasecmp (p, "weapons"))
				flag = it_weapons;
			else if (!strcasecmp (p, "powerups"))
				flag = it_powerups;
			else if (!strcasecmp (p, "ammo"))
				flag = it_ammo;
			else if (!strcasecmp (p, "players"))
				flag = it_players;
			else if (!strcasecmp (p, "default"))
				flag = defaultflags;
			else if (!strcasecmp (p, "all"))
				flag = (1<<NUM_ITEMFLAGS)-1;
		}


		if (flags != &pointflags)
			flag &= ~(it_sentry|it_disp|it_players);

		if (removeflag)
			*flags &= ~flag;
		else {
			*flags |= flag;
			for (j = 1; j < NUM_ITEMFLAGS; j++) {
				if (flag & (1 << (j-1)))
					pointpriorities[j] = i;
			}
		}
	}
}

void TP_Took_f (void)
{
	FlagCommand (&tookflags, default_tookflags);
}

void TP_Pickup_f (void)
{
	FlagCommand (&pkflags, default_pkflags);
}

void TP_Point_f (void)
{
	FlagCommand (&pointflags, default_pointflags);
}

/*******
 * Set of inbuilt teamplay messages starts here
 */

#define HAVE_RL() (cl.stats[STAT_ITEMS] & IT_ROCKET_LAUNCHER)
#define HAVE_LG() (cl.stats[STAT_ITEMS] & IT_LIGHTNING)
#define HOLD_GL() (cl.stats[STAT_ACTIVEWEAPON] == IT_GRENADE_LAUNCHER) // only used in tp_lost
#define HOLD_RL() (cl.stats[STAT_ACTIVEWEAPON] == IT_ROCKET_LAUNCHER)
#define HOLD_LG() (cl.stats[STAT_ACTIVEWEAPON] == IT_LIGHTNING)

#define HAVE_QUAD() (cl.stats[STAT_ITEMS] & IT_QUAD) //quad
#define HAVE_PENT() (cl.stats[STAT_ITEMS] & IT_INVULNERABILITY) //pent
#define HAVE_RING() (cl.stats[STAT_ITEMS] & IT_INVISIBILITY) //ring
#define HAVE_GA() (cl.stats[STAT_ITEMS] & IT_ARMOR1) //ga
#define HAVE_YA() (cl.stats[STAT_ITEMS] & IT_ARMOR2) //ya
#define HAVE_RA() (cl.stats[STAT_ITEMS] & IT_ARMOR3) //ra

#define TOOK(x) (!strcmp(Macro_Took(), tp_name_##x.string))
#define COLORED(c,str) "{&c" #c #str "&cfff}"
#define INPOINT(thing) strstr(Macro_PointName(), tp_name_##thing.string)
#define DEAD() (cl.stats[STAT_HEALTH] < 1)

typedef char * MSGPART;

// will return short version of player's nick for teamplay messages
char *TP_ShortNick(void)
{
    extern cvar_t cl_fakename;
    static char buf[7];

    if (*(cl_fakename.string)) return "";
    else {
        snprintf(buf, sizeof(buf), "$\\%.3s:", TP_PlayerName());
        return buf;
    }
}      

// wrapper for snprintf & Cbuf_AddText that will add say_team nick part
void TP_Send_TeamSay(char *format, ...)
{
    char tp_msg_head[256], tp_msg_body[256], tp_msg[512];
    va_list argptr;

    snprintf(tp_msg_head, sizeof(tp_msg_head), "say_team %s ", TP_ShortNick());

	va_start (argptr, format);
    vsnprintf(tp_msg_body, sizeof(tp_msg_body), format, argptr);
	va_end (argptr);

    snprintf(tp_msg, sizeof(tp_msg), "%s%s\n", tp_msg_head, tp_msg_body);
    
    Cbuf_AddText(tp_msg);
}

#define tp_sep_red		"$R$R"      // enemy, lost
#define tp_sep_green	"$G$G"      // killed quad/ring/pent enemy, safe
#define tp_sep_yellow	"$Y$Y"      // help
#define tp_sep_white	"$x04$x04"  // Two white bubbles, location of item
#define tp_sep_blue		"$B$B"      // 

/*
Should we enforce special colors this way below or we just should make
these values new default?
In my opinion with new defaults we should wait for different color syntax. - johnnycz
However this brings a problem - you always have to use these and cannot
use the %-macros nor the $-macros.
*/

#define tp_ib_name_rl	    COLORED(f0f,rl)	    // purple rl
#define tp_ib_name_lg	    COLORED(f0f,lg)	    // purple lg
#define tp_ib_name_ga	    COLORED(0b0,ga)	    // green ga
#define tp_ib_name_ya	    COLORED(ff0,ya)	    // yellow ya
#define tp_ib_name_ra	    COLORED(e00,ra)	    // red ra
#define tp_ib_name_mh	    COLORED(03F,mega)	// blue mega
#define tp_ib_name_backpack	COLORED(F0F,pack)	// purple backpack
#define tp_ib_name_quad	    COLORED(03F,quad)	// blue quad
#define tp_ib_name_pent	    COLORED(e00,pent)	// red pent
#define tp_ib_name_ring	    COLORED(ff0,ring)	// yellow ring
#define tp_ib_name_eyes	    COLORED(ff0,eyes)	// yellow eyes (remember, ring is when you see the ring, eyes is when someone has rings!)
#define tp_ib_name_enemy	COLORED(e00,enemy)	// red enemy
#define tp_ib_name_team	    COLORED(0b0,team)	// green "team" (with powerup)
#define tp_ib_name_quaded	COLORED(03F,quaded)	// blue "quaded"
#define tp_ib_name_pented	COLORED(e00,pented)	// red "pented"
#define tp_ib_name_rlg      COLORED(f0f,rlg)    // purple rlg
#define tp_ib_name_safe	    COLORED(0b0,safe)	// green safe
#define tp_ib_name_help	    COLORED(ff0,help)	// yellow help



void TP_Msg_Lost_f (void)
{
    MSGPART led = tp_sep_red;
    MSGPART msg1 = "";
	MSGPART msg2 = "";

    if (TOOK(quad))
    {
        msg1 = tp_ib_name_quad " over ";
    }

    if (HOLD_RL() || HOLD_LG() || HOLD_GL()) // gl could be useful too
    {
        msg2 = "lost " COLORED(f0f,$weapon) " $[{%d}$] e:%E";
    }
    else
    {
        msg2 = "lost $[{%d}$] e:%E";
    }

    TP_Send_TeamSay("%s %s%s", led, msg1, msg2);
}



void TP_Msg_ReportComing(qbool report) // tp_report and tp_coming are similar, differences are led color, where %l is, and the word "coming"
{
    MSGPART msg1 = "";
	MSGPART msg2 = "";
	MSGPART msg3 = "";
	MSGPART msg4 = "";
	MSGPART msg5 = "";
	MSGPART msg6 = "";
	
	if (report)
	{
		msg1 = tp_sep_blue;
	}
	else
	{
		if (DEAD()) // you can't be coming if you're dead! no correct location to report either, so wait until you're alive!
		{
			TP_Msg_Lost_f();
			return;
		}
		else
		{
			msg1 = tp_sep_white " {coming}";
		}
	}
	
	
	if (DEAD()) // if player is dead, report lost
	{
		TP_Msg_Lost_f();
		return;
	}
	
	if (HAVE_GA())
		msg3 = COLORED(0b0,%a);
	else if (HAVE_YA())
		msg3 = COLORED(ff0,%a);
	else if (HAVE_RA())
		msg3 = COLORED(e00,%a);
	else
		msg3 = "0";
	
	if (HAVE_QUAD() && HAVE_PENT() && HAVE_RING())
		msg5 = " " tp_ib_name_team " " tp_ib_name_quad " " tp_ib_name_pent " " tp_ib_name_ring;
	else if (HAVE_QUAD() && HAVE_PENT())
		msg5 = " " tp_ib_name_team " " tp_ib_name_quad " " tp_ib_name_pent;
	else if (HAVE_QUAD() && HAVE_RING())
		msg5 = " " tp_ib_name_team " " tp_ib_name_quad " " tp_ib_name_ring;
	else if (HAVE_PENT() && HAVE_RING())
		msg5 = " " tp_ib_name_team " " tp_ib_name_pent " " tp_ib_name_ring;
	else if (HAVE_QUAD())
		msg5 = " " tp_ib_name_team " " tp_ib_name_quad;
	else if (HAVE_PENT())
		msg5 = " " tp_ib_name_team " " tp_ib_name_pent;
	else if (HAVE_RING())
		msg5 = " " tp_ib_name_team " " tp_ib_name_ring;
	else
		msg5 = "";
		
    if		(HAVE_RL() && HAVE_LG())	msg6 = " " tp_ib_name_rlg ":$rockets/$cells";
    else if (HAVE_RL())					msg6 = " " tp_ib_name_rl ":$rockets";
    else if (HAVE_LG())					msg6 = " " tp_ib_name_lg ":$cells";
    else								msg6 = "";
	
	msg2 = "%h";
	msg4 = "$[{%l}$]";
	
	// $B$B(1) health(2)/armor(3) location(4) powerup(5) rlg:x(6) //tp_report
	if (report)
		TP_Send_TeamSay("%s %s/%s %s%s%s", msg1, msg2, msg3, msg4, msg5, msg6);
	// $W$W(1) coming(1) location(4) health()/armor(3) powerup(5) rlg:x(6) //tp_coming
	else
		TP_Send_TeamSay("%s %s %s/%s%s%s", msg1, msg4, msg2, msg3, msg5, msg6); // notice that in tp_coming, we report our location before anything else!
}
void TP_Msg_Report_f (void) { TP_Msg_ReportComing(true); }
void TP_Msg_Coming_f (void) { TP_Msg_ReportComing(false); }



void TP_Msg_EnemyPowerup_f (void)
{
    MSGPART msg1 = "";

	if (INPOINT(quaded) && INPOINT(pented) && INPOINT(eyes))
		msg1 = tp_ib_name_quaded " " tp_ib_name_pented " " tp_ib_name_eyes " " tp_ib_name_enemy " at $[{%y}$]";
	else if (INPOINT(quaded) && INPOINT(pented))
		msg1 = tp_ib_name_quaded " " tp_ib_name_pented " " tp_ib_name_enemy " at $[{%y}$]";
	else if (INPOINT(quaded) && INPOINT(eyes))
		msg1 = tp_ib_name_quaded " " tp_ib_name_eyes " " tp_ib_name_enemy " at $[{%y}$]";
	else if (INPOINT(pented) && INPOINT(eyes))
		msg1 = tp_ib_name_pented " " tp_ib_name_eyes " " tp_ib_name_enemy " at $[{%y}$]";
	else if (INPOINT(quaded))
		msg1 = tp_ib_name_quaded " " tp_ib_name_enemy " at $[{%y}$]";
	else if (INPOINT(pented))
		msg1 = tp_ib_name_pented " " tp_ib_name_enemy " at $[{%y}$]";
	else if (INPOINT(eyes))
		msg1 = tp_ib_name_enemy " " tp_ib_name_eyes " at $[{%y}$]";
	else
		msg1 = tp_ib_name_enemy " {%q}";
	
	TP_Send_TeamSay(tp_sep_red " %s", msg1);
}



void TP_Msg_SafeHelp(qbool safe)
{
	MSGPART msg1 = "";
	MSGPART msg2 = "";
	MSGPART msg3 = "";
	MSGPART msg4 = "";
	MSGPART msg5 = "";

	if (safe && INPOINT(enemy))
		return; // if you see enemy, then it's not safe (really?)  (most of the time).
	
	if (safe)
	{
		msg1 = tp_sep_green;
		msg2 = tp_ib_name_safe;
	}
	else
	{
		msg1 = tp_sep_yellow;
		msg2 = tp_ib_name_help;
	}

	msg3 = "$[{%l}$]";

	if (HAVE_QUAD() && HAVE_PENT() && HAVE_RING())
		msg4 = tp_ib_name_team " " tp_ib_name_quad " " tp_ib_name_pent " " tp_ib_name_ring " ";
	else if (HAVE_QUAD() && HAVE_PENT())
		msg4 = tp_ib_name_team " " tp_ib_name_quad " " tp_ib_name_pent " ";
	else if (HAVE_QUAD() && HAVE_RING())
		msg4 = tp_ib_name_team " " tp_ib_name_quad " " tp_ib_name_ring " ";
	else if (HAVE_PENT() && HAVE_RING())
		msg4 = tp_ib_name_team " " tp_ib_name_pent " " tp_ib_name_ring " ";
	else if (HAVE_QUAD())
		msg4 = tp_ib_name_team " " tp_ib_name_quad " ";
	else if (HAVE_PENT())
		msg4 = tp_ib_name_team " " tp_ib_name_pent " ";
	else if (HAVE_RING())
		msg4 = tp_ib_name_team " " tp_ib_name_ring " ";
	else
		msg4 = "";
		
    if		(HAVE_RL() && HAVE_LG())	msg5 = tp_ib_name_rlg ":$rockets/$cells";
    else if (HAVE_RL())					msg5 = tp_ib_name_rl ":$rockets";
    else if (HAVE_LG())					msg5 = tp_ib_name_lg ":$cells";
    else								msg5 = "";
	//1=sep, 2=safe/help 3=loc 4=powerup 5=weap
	TP_Send_TeamSay("%s %s %s %s%s", msg1, msg2, msg3, msg4, msg5);
}
void TP_Msg_Safe_f (void) { TP_Msg_SafeHelp(true); }
void TP_Msg_Help_f (void) { TP_Msg_SafeHelp(false); }



void TP_Msg_GetPentQuad(qbool quad)
{
	MSGPART msg1 = "";

	if (quad)
	{
		msg1 = tp_ib_name_quad;
	}
	else
	{
		msg1 = tp_ib_name_pent;
	}
	
	//$R$R get powerup(1)
	TP_Send_TeamSay(tp_sep_red " get %s", msg1);
}
void TP_Msg_GetQuad_f (void) { TP_Msg_GetPentQuad(true); }
void TP_Msg_GetPent_f (void) { TP_Msg_GetPentQuad(false); }



void TP_Msg_QuadDead_f (void)
{
    MSGPART msg1 = "";

	msg1 = tp_ib_name_quad " dead";
	
	TP_Send_TeamSay(tp_sep_green " %s", msg1);
}

/***********/


typedef struct
{
	int		itemflag;
	cvar_t	*cvar;
	char	*modelname;
	vec3_t	offset;		// offset of model graphics center
	float	radius;		// model graphics radius
	int		flags;		// TODO: "NOPICKUP" (disp), "TEAMENEMY" (flag, disp)
} item_t;

item_t	tp_items[] = {
                        {	it_quad,	&tp_name_quad,	"progs/quaddama.mdl",
                          {0, 0, 24},	25,
                        },
                        {	it_pent,	&tp_name_pent,	"progs/invulner.mdl",
                          {0, 0, 22},	25,
                        },
                        {	it_ring,	&tp_name_ring,	"progs/invisibl.mdl",
                          {0, 0, 16},	12,
                        },
                        {	it_suit,	&tp_name_suit,	"progs/suit.mdl",
                          {0, 0, 24}, 20,
                        },
                        {	it_lg,		&tp_name_lg,	"progs/g_light.mdl",
                          {0, 0, 30},	20,
                        },
                        {	it_rl,		&tp_name_rl,	"progs/g_rock2.mdl",
                          {0, 0, 30},	20,
                        },
                        {	it_gl,		&tp_name_gl,	"progs/g_rock.mdl",
                          {0, 0, 30},	20,
                        },
                        {	it_sng,		&tp_name_sng,	"progs/g_nail2.mdl",
                          {0, 0, 30},	20,
                        },
                        {	it_ng,		&tp_name_ng,	"progs/g_nail.mdl",
                          {0, 0, 30},	20,
                        },
                        {	it_ssg,		&tp_name_ssg,	"progs/g_shot.mdl",
                          {0, 0, 30},	20,
                        },
                        {	it_cells,	&tp_name_cells,	"maps/b_batt0.bsp",
                          {16, 16, 24},	18,
                        },
                        {	it_cells,	&tp_name_cells,	"maps/b_batt1.bsp",
                          {16, 16, 24},	18,
                        },
                        {	it_rockets,	&tp_name_rockets,"maps/b_rock0.bsp",
                          {8, 8, 20},	18,
                        },
                        {	it_rockets,	&tp_name_rockets,"maps/b_rock1.bsp",
                          {16, 8, 20},	18,
                        },
                        {	it_nails,	&tp_name_nails,	"maps/b_nail0.bsp",
                          {16, 16, 10},	18,
                        },
                        {	it_nails,	&tp_name_nails,	"maps/b_nail1.bsp",
                          {16, 16, 10},	18,
                        },
                        {	it_shells,	&tp_name_shells,"maps/b_shell0.bsp",
                          {16, 16, 10},	18,
                        },
                        {	it_shells,	&tp_name_shells,"maps/b_shell1.bsp",
                          {16, 16, 10},	18,
                        },
                        {	it_health,	&tp_name_health,"maps/b_bh10.bsp",
                          {16, 16, 8},	18,
                        },
                        {	it_health,	&tp_name_health,"maps/b_bh25.bsp",
                          {16, 16, 8},	18,
                        },
                        {	it_mh,		&tp_name_mh,	"maps/b_bh100.bsp",
                          {16, 16, 14},	20,
                        },
                        {	it_pack,	&tp_name_backpack, "progs/backpack.mdl",
                          {0, 0, 18},	18,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/tf_flag.mdl",
                          {0, 0, 14},	25,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/tf_stan.mdl",
                          {0, 0, 45},	40,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/w_g_key.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/w_s_key.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/m_g_key.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/m_s_key.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/b_s_key.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/b_g_key.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_flag,	&tp_name_flag,	"progs/flag.mdl",
                          {0, 0, 14},	25,
                        },
                        {	it_runes,	&tp_name_rune1,	"progs/end1.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_runes,	&tp_name_rune2,	"progs/end2.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_runes,	&tp_name_rune3,	"progs/end3.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_runes,	&tp_name_rune4,	"progs/end4.mdl",
                          {0, 0, 20},	18,
                        },
                        {	it_ra|it_ya|it_ga, NULL,	"progs/armor.mdl",
                          {0, 0, 24},	22,
                        },
                        {	it_sentry, &tp_name_sentry, "progs/turrgun.mdl",
                          {0, 0, 23},	25,
                        },
                        {	it_disp, &tp_name_disp,	"progs/disp.mdl",
                          {0, 0, 24},	25,
                        }
                    };

#define NUMITEMS (sizeof(tp_items) / sizeof(tp_items[0]))

item_t *model2item[MAX_MODELS];

void TP_FindModelNumbers (void)
{
	int i, j;
	char *s;
	item_t *item;

	for (i = 0 ; i < MAX_MODELS ; i++) {
		model2item[i] = NULL;
		s = cl.model_name[i];
		if (!s)
			continue;
		for (j = 0, item = tp_items ; j < NUMITEMS ; j++, item++)
			if (!strcmp(s, item->modelname))
				model2item[i] = item;
	}
}

// on success, result is non-zero
// on failure, result is zero
// for armors, returns skinnum+1 on success
static int FindNearestItem (int flags, item_t **pitem)
{
	frame_t *frame;
	packet_entities_t *pak;
	entity_state_t *ent, *bestent = NULL;
	int	i;
	float bestdist, dist;
	vec3_t org, v;
	item_t *item;

	VectorCopy (cl.frames[cl.validsequence & UPDATE_MASK].playerstate[cl.playernum].origin, org);

	// look in previous frame
	frame = &cl.frames[cl.oldvalidsequence&UPDATE_MASK];
	pak = &frame->packet_entities;
	bestdist = 100 * 100;
	*pitem = NULL;
	for (i = 0, ent = pak->entities; i < pak->num_entities; i++, ent++) {
		item = model2item[ent->modelindex];
		if (!item || !(item->itemflag & flags))
			continue;

		VectorSubtract (ent->origin, org, v);
		VectorAdd (v, item->offset, v);
		dist = DotProduct (v, v);

		if (dist <= bestdist) {
			bestdist = dist;
			bestent = ent;
			*pitem = item;
		}
	}

	if (bestent)
		strlcpy(vars.nearestitemloc, TP_LocationName(bestent->origin), sizeof(vars.nearestitemloc));
	else
		vars.nearestitemloc[0] = 0;

	if (bestent && (*pitem)->itemflag == it_armor)
		return bestent->skinnum + 1;	// 1 = green, 2 = yellow, 3 = red

	return bestent ? bestent->modelindex : 0;
}

char *Macro_LastTookOrPointed (void)
{
	if (vars.tooktime && vars.tooktime > vars.pointtime && cls.realtime - vars.tooktime < 5)
		return Macro_TookAtLoc();
	else if (vars.pointtime && vars.tooktime <= vars.pointtime && cls.realtime - vars.pointtime < 5)
		return Macro_LastPointAtLoc();

	snprintf(macro_buf, sizeof(macro_buf), "%s %s %s", tp_name_nothing.string, tp_name_at.string, tp_name_someplace.string);
	return macro_buf;
}

static qbool CheckTrigger (void)
{
	int	i, count;
	player_info_t *player;
	char *myteam;

	if (cl.spectator || Rulesets_RestrictTriggers())
		return false;

	if (tp_forceTriggers.value)
		return true;

	if (!cl.teamplay)
		return false;

	count = 0;
	myteam = cl.players[cl.playernum].team;
	for (i = 0, player= cl.players; i < MAX_CLIENTS; i++, player++) {
		if (player->name[0] && !player->spectator && i != cl.playernum && !strcmp(player->team, myteam))
			count++;
	}

	return count;
}

static void ExecTookTrigger (char *s, int flag, vec3_t org)
{
	int pkflags_dmm, tookflags_dmm;

	pkflags_dmm = pkflags;
	tookflags_dmm = tookflags;

	if (!cl.teamfortress && cl.deathmatch >= 1 && cl.deathmatch <= 4) {
		if (cl.deathmatch == 4) {
			pkflags_dmm &= ~(it_ammo|it_weapons);
			tookflags_dmm &= ~(it_ammo|it_weapons);
		}
	}
	if (!((pkflags_dmm|tookflags_dmm) & flag))
		return;

	vars.tooktime = cls.realtime;
	strncpy (vars.tookname, s, sizeof(vars.tookname)-1);
	strncpy (vars.tookloc, TP_LocationName (org), sizeof(vars.tookloc)-1);

	if ((tookflags_dmm & flag) && CheckTrigger())
		TP_ExecTrigger ("f_took");
}

void TP_ParsePlayerInfo(player_state_t *oldstate, player_state_t *state, player_info_t *info)
{
	if (TP_NeedRefreshSkins()) {
		if ((state->effects & (EF_BLUE|EF_RED) ) != (oldstate->effects & (EF_BLUE|EF_RED)))
			TP_RefreshSkin(info - cl.players);
	}

	if (!cl.spectator && cl.teamplay && strcmp(info->team, TP_PlayerTeam())) {
		qbool eyes;

		eyes = state->modelindex && cl.model_precache[state->modelindex] && cl.model_precache[state->modelindex]->modhint == MOD_EYES;

		if (state->effects & (EF_BLUE | EF_RED) || eyes) {
			vars.enemy_powerups = 0;
			vars.enemy_powerups_time = cls.realtime;

			if (state->effects & EF_BLUE)
				vars.enemy_powerups |= TP_QUAD;
			if (state->effects & EF_RED)
				vars.enemy_powerups |= TP_PENT;
			if (eyes)
				vars.enemy_powerups |= TP_RING;
		}
	}
	if (!cl.spectator && !cl.teamfortress && info - cl.players == cl.playernum) {
		if ((state->effects & (EF_FLAG1|EF_FLAG2)) && !(oldstate->effects & (EF_FLAG1|EF_FLAG2))) {
			ExecTookTrigger (tp_name_flag.string, it_flag, cl.frames[cl.validsequence & UPDATE_MASK].playerstate[cl.playernum].origin);
		} else if (!(state->effects & (EF_FLAG1|EF_FLAG2)) && (oldstate->effects & (EF_FLAG1|EF_FLAG2))) {
			vars.lastdrop_time = cls.realtime;
			strcpy (vars.lastdroploc, Macro_Location());
		}
	}
}

void TP_CheckPickupSound (char *s, vec3_t org)
{
	item_t *item;

	if (cl.spectator)
		return;

	if (!strcmp(s, "items/damage.wav"))
		ExecTookTrigger (tp_name_quad.string, it_quad, org);
	else if (!strcmp(s, "items/protect.wav"))
		ExecTookTrigger (tp_name_pent.string, it_pent, org);
	else if (!strcmp(s, "items/inv1.wav"))
		ExecTookTrigger (tp_name_ring.string, it_ring, org);
	else if (!strcmp(s, "items/suit.wav"))
		ExecTookTrigger (tp_name_suit.string, it_suit, org);
	else if (!strcmp(s, "items/health1.wav") || !strcmp(s, "items/r_item1.wav"))
		ExecTookTrigger (tp_name_health.string, it_health, org);
	else if (!strcmp(s, "items/r_item2.wav"))
		ExecTookTrigger (tp_name_mh.string, it_mh, org);
	else
		goto more;
	return;

more:
	if (!cl.validsequence || !cl.oldvalidsequence)
		return;

	// weapons
	if (!strcmp(s, "weapons/pkup.wav"))	{
		if (FindNearestItem (it_weapons, &item)) {
			ExecTookTrigger (item->cvar->string, item->itemflag, org);
		} else if (vars.stat_framecounts[STAT_ITEMS] == cls.framecount) {
			if (vars.items & ~vars.olditems & IT_LIGHTNING)
				ExecTookTrigger (tp_name_lg.string, it_lg, cl.simorg);
			else if (vars.items & ~vars.olditems & IT_ROCKET_LAUNCHER)
				ExecTookTrigger (tp_name_rl.string, it_rl, cl.simorg);
			else if (vars.items & ~vars.olditems & IT_GRENADE_LAUNCHER)
				ExecTookTrigger (tp_name_gl.string, it_gl, cl.simorg);
			else if (vars.items & ~vars.olditems & IT_SUPER_NAILGUN)
				ExecTookTrigger (tp_name_sng.string, it_sng, cl.simorg);
			else if (vars.items & ~vars.olditems & IT_NAILGUN)
				ExecTookTrigger (tp_name_ng.string, it_ng, cl.simorg);
			else if (vars.items & ~vars.olditems & IT_SUPER_SHOTGUN)
				ExecTookTrigger (tp_name_ssg.string, it_ssg, cl.simorg);
		}
		return;
	}

	// armor
	if (!strcmp(s, "items/armor1.wav"))	{
		qbool armor_updated;
		int armortype;

		armor_updated = (vars.stat_framecounts[STAT_ARMOR] == cls.framecount);
		armortype = FindNearestItem (it_armor, &item);
		if (armortype == 1 || (!armortype && armor_updated && cl.stats[STAT_ARMOR] == 100))
			ExecTookTrigger (tp_name_ga.string, it_ga, org);
		else if (armortype == 2 || (!armortype && armor_updated && cl.stats[STAT_ARMOR] == 150))
			ExecTookTrigger (tp_name_ya.string, it_ya, org);
		else if (armortype == 3 || (!armortype && armor_updated && cl.stats[STAT_ARMOR] == 200))
			ExecTookTrigger (tp_name_ra.string, it_ra, org);
		return;
	}

	// backpack, ammo or runes
	if (!strcmp (s, "weapons/lock4.wav")) {
		if (!FindNearestItem (it_ammo|it_pack|it_runes, &item))
			return;
		ExecTookTrigger (item->cvar->string, item->itemflag, org);
	}
}

qbool TP_IsItemVisible(item_vis_t *visitem)
{
	vec3_t end, v;
	trace_t trace;

	if (visitem->dist <= visitem->radius)
		return true;

	VectorNegate (visitem->dir, v);
	VectorNormalizeFast (v);
	VectorMA (visitem->entorg, visitem->radius, v, end);
	trace = PM_TraceLine (visitem->vieworg, end);
	if ((int)trace.fraction == 1)
		return true;

	VectorMA (visitem->entorg, visitem->radius, visitem->right, end);
	VectorSubtract (visitem->vieworg, end, v);
	VectorNormalizeFast (v);
	VectorMA (end, visitem->radius, v, end);
	trace = PM_TraceLine (visitem->vieworg, end);
	if ((int)trace.fraction == 1)
		return true;

	VectorMA(visitem->entorg, -visitem->radius, visitem->right, end);
	VectorSubtract(visitem->vieworg, end, v);
	VectorNormalizeFast(v);
	VectorMA(end, visitem->radius, v, end);
	trace = PM_TraceLine(visitem->vieworg, end);
	if ((int)trace.fraction == 1)
		return true;

	VectorMA(visitem->entorg, visitem->radius, visitem->up, end);
	VectorSubtract(visitem->vieworg, end, v);
	VectorNormalizeFast(v);
	VectorMA (end, visitem->radius, v, end);
	trace = PM_TraceLine(visitem->vieworg, end);
	if ((int)trace.fraction == 1)
		return true;

	// use half the radius, otherwise it's possible to see through floor in some places
	VectorMA(visitem->entorg, -visitem->radius / 2, visitem->up, end);
	VectorSubtract(visitem->vieworg, end, v);
	VectorNormalizeFast(v);
	VectorMA(end, visitem->radius, v, end);
	trace = PM_TraceLine(visitem->vieworg, end);
	if ((int)trace.fraction == 1)
		return true;

	return false;
}

static float TP_RankPoint(item_vis_t *visitem)
{
	vec3_t v2, v3;
	float miss;

	if (visitem->dist < 10)
		return -1;

	VectorScale (visitem->forward, visitem->dist, v2);
	VectorSubtract (v2, visitem->dir, v3);
	miss = VectorLength (v3);
	if (miss > 300)
		return -1;
	if (miss > visitem->dist * (tp_pointpriorities.value ? 0.55 : 1.7)) // for prioritized point
		return -1;		// over 60 degrees off

	if (tp_pointpriorities.value)
		return 1;
	if (visitem->dist < 3000.0 / 8.0)
		return miss * (visitem->dist * 8.0 * 0.0002f + 0.3f);
	else return miss;
}

void TP_FindPoint (void)
{
	packet_entities_t *pak;
	entity_state_t *ent;
	int	i, j, tempflags;
	unsigned int pointflags_dmm;
	float best = -1, rank;
	entity_state_t *bestent = NULL;
	vec3_t ang;
	item_t *item, *bestitem = NULL;
	player_state_t *state, *beststate = NULL;
	player_info_t *info, *bestinfo = NULL;
	item_vis_t visitem;
	extern cvar_t v_viewheight;

	if (vars.pointtime == cls.realtime)
		return;

	if (!cl.validsequence)
		goto nothing;

	ang[0] = cl.viewangles[0]; ang[1] = cl.viewangles[1]; ang[2] = 0;
	AngleVectors (ang, visitem.forward, visitem.right, visitem.up);
	VectorCopy (cl.simorg, visitem.vieworg);
	visitem.vieworg[2] += 22 + (v_viewheight.value ? bound (-7, v_viewheight.value, 4) : 0);

	pointflags_dmm = pointflags;
	if (!cl.teamfortress && cl.deathmatch >= 1 && cl.deathmatch <= 4) {
		if (cl.deathmatch == 4)
			pointflags_dmm &= ~it_ammo;
		if (cl.deathmatch != 1)
			pointflags_dmm &= ~it_weapons;
	}

	pak = &cl.frames[cl.validsequence & UPDATE_MASK].packet_entities;
	for (i = 0,ent = pak->entities; i < pak->num_entities; i++, ent++) {
		item = model2item[ent->modelindex];
		if (!item || !(item->itemflag & pointflags_dmm))
			continue;
		// special check for armors
		if (item->itemflag == (it_ra|it_ya|it_ga)) {
			switch (ent->skinnum) {
					case 0: if (!(pointflags_dmm & it_ga)) continue; break;
					case 1: if (!(pointflags_dmm & it_ya)) continue; break;
					case 2: if (!(pointflags_dmm & it_ra)) continue; break;
			}
		}

		VectorAdd (ent->origin, item->offset, visitem.entorg);
		VectorSubtract (visitem.entorg, visitem.vieworg, visitem.dir);
		visitem.dist = DotProduct (visitem.dir, visitem.forward);
		visitem.radius = ent->effects & (EF_BLUE|EF_RED|EF_DIMLIGHT|EF_BRIGHTLIGHT) ? 200 : item->radius;

		if ((rank = TP_RankPoint(&visitem)) < 0)
			continue;

		if (tp_pointpriorities.value && rank != -1) {
			tempflags = item->itemflag;
			for (j = 1; j < NUM_ITEMFLAGS; j++)
				if (!(tempflags & 1))
					tempflags >>= 1;
				else
					break;

			rank = pointpriorities[j];
		}

		// check if we can actually see the object
		if ((rank < best || best < 0) && TP_IsItemVisible(&visitem)) {
			best = rank;
			bestent = ent;
			bestitem = item;
		}
	}

	state = cl.frames[cl.parsecount & UPDATE_MASK].playerstate;
	info = cl.players;
	for (j = 0; j < MAX_CLIENTS; j++, info++, state++) {
		if (state->messagenum != cl.parsecount || j == cl.playernum || info->spectator)
			continue;

		if (
		    ( state->modelindex == cl_modelindices[mi_player] && ISDEAD(state->frame) ) ||
		    ( state->modelindex == cl_modelindices[mi_h_player] )
		)
			continue;

		VectorCopy (state->origin, visitem.entorg);
		visitem.entorg[2] += 30;
		VectorSubtract (visitem.entorg, visitem.vieworg, visitem.dir);
		visitem.dist = DotProduct (visitem.dir, visitem.forward);
		visitem.radius = (state->effects & (EF_BLUE|EF_RED|EF_DIMLIGHT|EF_BRIGHTLIGHT) ) ? 200 : 27;

		if ((rank = TP_RankPoint(&visitem)) < 0)
			continue;

		// check if we can actually see the object
		if ((rank < best || best < 0) && TP_IsItemVisible(&visitem)) {
			qbool teammate, eyes = false;

			eyes = state->modelindex && cl.model_precache[state->modelindex] && cl.model_precache[state->modelindex]->modhint == MOD_EYES;
			teammate = !!(cl.teamplay && !strcmp(info->team, TP_PlayerTeam()));

			if (eyes && !(pointflags_dmm & it_eyes))
				continue;
			else if (teammate && !(pointflags_dmm & it_teammate))
				continue;
			else if (!(pointflags_dmm & it_enemy))
				continue;

			best = rank;
			bestinfo = info;
			beststate = state;
		}
	}

	if (best >= 0 && bestinfo) {
		qbool teammate, eyes;
		char *name, buf[256] = {0};

		eyes = beststate->modelindex && cl.model_precache[beststate->modelindex] &&
		       cl.model_precache[beststate->modelindex]->modhint == MOD_EYES;
		if (cl.teamfortress) {
			teammate = !strcmp(Utils_TF_ColorToTeam(bestinfo->real_bottomcolor), TP_PlayerTeam());

			if (eyes)
				name = tp_name_eyes.string;		//duck on 2night2
			else if (cl.spectator)
				name = bestinfo->name;
			else if (teammate)
				name = tp_name_teammate.string[0] ? tp_name_teammate.string : "teammate";
			else
				name = tp_name_enemy.string;

			if (!eyes)
				name = va("%s%s%s", name, name[0] ? " " : "", Skin_To_TFSkin(Info_ValueForKey(bestinfo->userinfo, "skin")));
		} else {
			teammate = (cl.teamplay && !strcmp(bestinfo->team, TP_PlayerTeam()));

			if (eyes)
				name = tp_name_eyes.string;
			else if (cl.spectator || (teammate && !tp_name_teammate.string[0]))
				name = bestinfo->name;
			else
				name = teammate ? tp_name_teammate.string : tp_name_enemy.string;
		}
		if (beststate->effects & EF_BLUE)
			strncat(buf, tp_name_quaded.string, sizeof(buf) - strlen(buf) - 1);
		if (beststate->effects & EF_RED)
			strncat(buf, va("%s%s", buf[0] ? " " : "", tp_name_pented.string), sizeof(buf) - strlen(buf) - 1);
		strncat(buf, va("%s%s", buf[0] ? " " : "", name), sizeof(buf) - strlen(buf) - 1);
		strlcpy (vars.pointname, buf, sizeof(vars.pointname));
		strlcpy (vars.pointloc, TP_LocationName (beststate->origin), sizeof(vars.pointloc));

		vars.pointtype = (teammate && !eyes) ? POINT_TYPE_TEAMMATE : POINT_TYPE_ENEMY;
	} else if (best >= 0) {
		char *p;

		if (!bestitem->cvar) {
			// armors are special
			switch (bestent->skinnum) {
					case 0: p = tp_name_ga.string; break;
					case 1: p = tp_name_ya.string; break;
					default: p = tp_name_ra.string;
			}
		} else {
			p = bestitem->cvar->string;
		}

		vars.pointtype = (bestitem->itemflag & (it_powerups|it_flag)) ? POINT_TYPE_POWERUP : POINT_TYPE_ITEM;
		strlcpy (vars.pointname, p, sizeof(vars.pointname));
		strlcpy (vars.pointloc, TP_LocationName (bestent->origin), sizeof(vars.pointloc));
	} else {
	nothing:
		strlcpy (vars.pointname, tp_name_nothing.string, sizeof(vars.pointname));
		vars.pointloc[0] = 0;
		vars.pointtype = POINT_TYPE_ITEM;
	}
	vars.pointtime = cls.realtime;
}

void TP_ParseWeaponModel(model_t *model)
{
	static model_t *last_model = NULL;

	if (cl.teamfortress && (!cl.spectator || Cam_TrackNum() != -1)) {
		if (model && !last_model)
			TP_ExecTrigger ("f_reloadend");
		else if (!model && last_model)
			TP_ExecTrigger ("f_reloadstart");
	}
	last_model = model;
}

void TP_StatChanged (int stat, int value)
{
	int effects;

	effects = cl.frames[cl.parsecount & UPDATE_MASK].playerstate[cl.playernum].effects;

	switch (stat) {
			case STAT_HEALTH:
			if (value > 0) {
				if (vars.health <= 0) {
					extern cshift_t	cshift_empty;
					if (cl.teamfortress)
						memset (&cshift_empty, 0, sizeof(cshift_empty));
					if (CheckTrigger())
						TP_ExecTrigger ("f_respawn");
				}
				vars.health = value;
				return;
			}
			if (vars.health > 0) {		// We have just died
				vars.deathtrigger_time = cls.realtime;
				strcpy (vars.lastdeathloc, Macro_Location());

				CountNearbyPlayers(true);
				vars.last_numenemies = vars.numenemies;
				vars.last_numfriendlies = vars.numfriendlies;

				if (CheckTrigger()) {
					if (cl.teamfortress && (cl.stats[STAT_ITEMS] & (IT_KEY1|IT_KEY2)))
						TP_ExecTrigger ("f_flagdeath");
					else if (effects & (EF_FLAG1|EF_FLAG2))
						TP_ExecTrigger ("f_flagdeath");
					else
						TP_ExecTrigger ("f_death");
				}
			}
			vars.health = value;
			break;
			case STAT_ITEMS:
			if (value & ~vars.items & (IT_KEY1|IT_KEY2)) {
				if (cl.teamfortress && !cl.spectator)
					ExecTookTrigger (tp_name_flag.string, it_flag, cl.frames[cl.validsequence & UPDATE_MASK].playerstate[cl.playernum].origin);
			}
			if (!cl.spectator && cl.teamfortress && ~value & vars.items & (IT_KEY1|IT_KEY2)) {
				vars.lastdrop_time = cls.realtime;
				strcpy (vars.lastdroploc, Macro_Location());
			}
			vars.olditems = vars.items;
			vars.items = value;
			break;
			case STAT_ACTIVEWEAPON:
			if (cl.stats[STAT_ACTIVEWEAPON] != vars.activeweapon) {
				TP_ExecTrigger ("f_weaponchange");
				vars.activeweapon = cl.stats[STAT_ACTIVEWEAPON];
			}
			break;
	}
	vars.stat_framecounts[stat] = cls.framecount;
}

/******************************* SOUND TRIGGERS *******************************/

//Find and execute sound triggers. A sound trigger must be terminated by either a CR or LF.
//Returns true if a sound was found and played
qbool TP_CheckSoundTrigger (char *str)
{
	int i, j, start, length;
	char soundname[MAX_OSPATH];
	FILE *f;

	if (!*str)
		return false;

	if (!tp_soundtrigger.string[0])
		return false;

	for (i = strlen(str) - 1; i; i--)	{
		if (str[i] != 0x0A && str[i] != 0x0D)
			continue;

		for (j = i - 1; j >= 0; j--) {
			// quick check for chars that cannot be used
			// as sound triggers but might be part of a file name
			if (isalnum((unsigned char)str[j]))
				continue;	// file name or chat

			if (strchr(tp_soundtrigger.string, str[j]))	{
				// this might be a sound trigger

				start = j + 1;
				length = i - start;

				if (!length)
					break;
				if (length >= MAX_QPATH)
					break;

				strlcpy (soundname, str + start, length + 1);
				if (strstr(soundname, ".."))
					break;	// no thank you

				// clean up the message
				strcpy (str + j, str + i);

				if (!snd_initialized)
					return false;

				COM_DefaultExtension (soundname, ".wav");

				// make sure we have it on disk (FIXME)
				FS_FOpenFile (va("sound/%s", soundname), &f);
				if (!f)
					return false;
				fclose (f);

				// now play the sound
				S_LocalSound (soundname);
				return true;
			}
			if (str[j] == '\\')
				str[j] = '/';
			if (str[j] <= ' ' || strchr("\"&'*,:;<>?\\|\x7f", str[j]))
				break;	// we don't allow these in a file name
		}
	}

	return false;
}

/****************************** MESSAGE FILTERS ******************************/

#define MAX_FILTER_LENGTH 4
char filter_strings[8][MAX_FILTER_LENGTH + 1];
int	num_filters = 0;

//returns false if the message shouldn't be printed.
//Matching filters are stripped from the message
qbool TP_FilterMessage (char *s)
{
	int i, j, len, maxlen;

	if (!num_filters)
		return true;

	len = strlen (s);
	if (len < 2 || s[len - 1] != '\n' || s[len - 2] == '#')
		return true;

	maxlen = MAX_FILTER_LENGTH + 1;
	for (i = len - 2 ; i >= 0 && maxlen > 0 ; i--, maxlen--) {
		if (s[i] == ' ')
			return true;
		if (s[i] == '#')
			break;
	}
	if (i < 0 || !maxlen)
		return true;	// no filter at all

	s[len - 1] = 0;	// so that strcmp works properly

	for (j = 0; j < num_filters; j++)
		if (!strcasecmp(s + i + 1, filter_strings[j])) {
			// strip the filter from message
			if (i && s[i - 1] == ' ')	{
				// there's a space just before the filter, remove it
				// so that soundtriggers like ^blah #att work
				s[i - 1] = '\n';
				s[i] = 0;
			} else {
				s[i] = '\n';
				s[i + 1] = 0;
			}
			return true;
		}

	s[len - 1] = '\n';
	return false;	// this message is not for us, don't print it
}

void TP_MsgFilter_f (void)
{
	int c, i;
	char *s;

	if ((c = Cmd_Argc()) == 1) {
		if (!num_filters) {
			Com_Printf ("No filters defined\n");
			return;
		}
		for (i = 0; i < num_filters; i++)
			Com_Printf ("%s#%s", i ? " " : "", filter_strings[i]);
		Com_Printf ("\n");
		return;
	}

	if (c == 2 && (Cmd_Argv(1)[0] == 0 || !strcasecmp(Cmd_Argv(1), "clear"))) {
		num_filters = 0;
		return;
	}

	num_filters = 0;
	for (i = 1; i < c; i++) {
		s = Cmd_Argv(i);
		if (*s != '#') {
			Com_Printf ("A filter must start with \"#\"\n");
			return;
		}
		if (strchr(s + 1, ' ')) {
			Com_Printf ("A filter may not contain spaces\n");
			return;
		}
		strlcpy (filter_strings[num_filters], s + 1, sizeof(filter_strings[0]));
		num_filters++;
		if (num_filters >= 8)
			break;
	}
}

void TP_DumpMsgFilters(FILE *f)
{
	int i;

	fprintf(f, "filter       ");
	if (!num_filters)
		fprintf(f, "clear");

	for (i = 0; i < num_filters; i++)
		fprintf (f, "#%s ", filter_strings[i]);

	fprintf(f, "\n");
}

/************************************ INIT ************************************/

void TP_Init (void)
{
	TP_InitReTriggers();
	TP_AddMacros();

	Cvar_SetCurrentGroup(CVAR_GROUP_CHAT);
	Cvar_Register (&cl_parseFunChars);
	Cvar_Register (&cl_parseSay);
	Cvar_Register (&cl_nofake);

	Cvar_SetCurrentGroup(CVAR_GROUP_SKIN);
	Cvar_Register (&cl_teamtopcolor);
	Cvar_Register (&cl_teambottomcolor);
	Cvar_Register (&cl_enemytopcolor);
	Cvar_Register (&cl_enemybottomcolor);
	Cvar_Register (&cl_enemybothskin);
	Cvar_Register (&cl_teambothskin);
	Cvar_Register (&cl_enemypentskin);
	Cvar_Register (&cl_teampentskin);
	Cvar_Register (&cl_enemyquadskin);
	Cvar_Register (&cl_teamquadskin);
	Cvar_Register (&cl_enemyskin);
	Cvar_Register (&cl_teamskin);

	Cvar_SetCurrentGroup(CVAR_GROUP_COMMUNICATION);
	Cvar_Register (&tp_triggers);
	Cvar_Register (&tp_msgtriggers);
	Cvar_Register (&tp_forceTriggers);
	Cvar_Register (&tp_loadlocs);
	Cvar_Register (&tp_pointpriorities);
	Cvar_Register (&tp_soundtrigger);
	Cvar_Register (&tp_weapon_order);

	Cvar_SetCurrentGroup(CVAR_GROUP_ITEM_NAMES);
	Cvar_Register (&tp_name_separator);
	Cvar_Register (&tp_name_none);
	Cvar_Register (&tp_name_nothing);
	Cvar_Register (&tp_name_at);
	Cvar_Register (&tp_name_someplace);

	Cvar_Register (&tp_name_rune1);
	Cvar_Register (&tp_name_rune2);
	Cvar_Register (&tp_name_rune3);
	Cvar_Register (&tp_name_rune4);

	Cvar_Register (&tp_name_status_blue);
	Cvar_Register (&tp_name_status_red);
	Cvar_Register (&tp_name_status_yellow);
	Cvar_Register (&tp_name_status_green);

	Cvar_Register (&tp_name_pented);
	Cvar_Register (&tp_name_quaded);
	Cvar_Register (&tp_name_eyes);
	Cvar_Register (&tp_name_enemy);
	Cvar_Register (&tp_name_teammate);
	Cvar_Register (&tp_name_disp);
	Cvar_Register (&tp_name_sentry);
	Cvar_Register (&tp_name_flag);
	Cvar_Register (&tp_name_backpack);
	Cvar_Register (&tp_name_weapon);
	Cvar_Register (&tp_name_armor);
	Cvar_Register (&tp_name_health);
	Cvar_Register (&tp_name_mh);
	Cvar_Register (&tp_name_suit);
	Cvar_Register (&tp_name_ring);
	Cvar_Register (&tp_name_pent);
	Cvar_Register (&tp_name_quad);
	Cvar_Register (&tp_name_cells);
	Cvar_Register (&tp_name_rockets);
	Cvar_Register (&tp_name_nails);
	Cvar_Register (&tp_name_shells);
	Cvar_Register (&tp_name_armortype_ra);
	Cvar_Register (&tp_name_armortype_ya);
	Cvar_Register (&tp_name_armortype_ga);
	Cvar_Register (&tp_name_ra);
	Cvar_Register (&tp_name_ya);
	Cvar_Register (&tp_name_ga);
	Cvar_Register (&tp_name_lg);
	Cvar_Register (&tp_name_rl);
	Cvar_Register (&tp_name_gl);
	Cvar_Register (&tp_name_sng);
	Cvar_Register (&tp_name_ng);
	Cvar_Register (&tp_name_ssg);
	Cvar_Register (&tp_name_sg);
	Cvar_Register (&tp_name_axe);

	Cvar_SetCurrentGroup(CVAR_GROUP_ITEM_NEED);
	Cvar_Register (&tp_need_health);
	Cvar_Register (&tp_need_cells);
	Cvar_Register (&tp_need_rockets);
	Cvar_Register (&tp_need_nails);
	Cvar_Register (&tp_need_shells);
	Cvar_Register (&tp_need_ra);
	Cvar_Register (&tp_need_ya);
	Cvar_Register (&tp_need_ga);
	Cvar_Register (&tp_need_weapon);
	Cvar_Register (&tp_need_rl);

	Cvar_ResetCurrentGroup();

	Cmd_AddCommand ("loadloc", TP_LoadLocFile_f);
	Cmd_AddCommand ("saveloc", TP_SaveLocFile_f);
	Cmd_AddCommand ("addloc", TP_AddLoc_f);
	Cmd_AddCommand ("removeloc", TP_RemoveLoc_f);
	Cmd_AddCommand ("clearlocs", TP_ClearLocs_f);
	Cmd_AddCommand ("filter", TP_MsgFilter_f);
	Cmd_AddCommand ("msg_trigger", TP_MsgTrigger_f);
	Cmd_AddCommand ("teamcolor", TP_TeamColor_f);
	Cmd_AddCommand ("enemycolor", TP_EnemyColor_f);
	Cmd_AddCommand ("tp_took", TP_Took_f);
	Cmd_AddCommand ("tp_pickup", TP_Pickup_f);
	Cmd_AddCommand ("tp_point", TP_Point_f);

	Cmd_AddCommand ("tp_report", TP_Msg_Report_f);
	Cmd_AddCommand ("tp_coming", TP_Msg_Coming_f);
    Cmd_AddCommand ("tp_lost", TP_Msg_Lost_f);
    Cmd_AddCommand ("tp_enemypwr", TP_Msg_EnemyPowerup_f);
	Cmd_AddCommand ("tp_quaddead", TP_Msg_QuadDead_f);
    Cmd_AddCommand ("tp_safe", TP_Msg_Safe_f);
    Cmd_AddCommand ("tp_help", TP_Msg_Help_f);
	Cmd_AddCommand ("tp_getquad", TP_Msg_GetQuad_f);
	Cmd_AddCommand ("tp_getpent", TP_Msg_GetPent_f);
    
}