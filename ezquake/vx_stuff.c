//VultureIIC

#ifdef GLQUAKE

#include "quakedef.h"
#include "pmove.h"
#include "vx_stuff.h"
int GL_LoadTextureImage (char * , char *, int, int, int);int coronatexture;extern cvar_t gl_bounceparticles;
// START shaman balancing variables
cvar_t		amf_showstats = {"r_glstats", "0", CVAR_ARCHIVE};cvar_t		tei_lavafire = {"gl_surface_lava", "0", CVAR_ARCHIVE};cvar_t		tei_slime	 = {"gl_surface_slime", "0", CVAR_ARCHIVE};cvar_t		amf_coronas = {"gl_coronas", "1", CVAR_ARCHIVE};cvar_t		amf_weather_rain = {"gl_weather_rain", "0", CVAR_ARCHIVE};cvar_t		amf_weather_rain_fast = {"gl_weather_rain_fast", "0", CVAR_ARCHIVE};cvar_t		amf_extratrails = {"gl_extratrails", "1", CVAR_ARCHIVE};cvar_t		amf_detpacklights = {"gl_detpacklights", "1", CVAR_ARCHIVE};cvar_t		amf_buildingsparks = {"gl_buildingsparks", "1", CVAR_ARCHIVE};cvar_t		amf_waterripple = {"gl_waterripple", "0", CVAR_ARCHIVE};cvar_t		amf_cutf_tesla_effect = {"gl_cutf_tesla_effect", "1", CVAR_ARCHIVE};cvar_t		amf_underwater_trails = {"gl_underwater_trails", "1", CVAR_ARCHIVE};cvar_t		amf_hidenails = {"cl_hidenails", "0", CVAR_ARCHIVE};cvar_t		amf_hiderockets = {"cl_hiderockets", "0", CVAR_ARCHIVE};cvar_t		amf_stat_loss = {"r_damagestats", "0", CVAR_ARCHIVE};cvar_t		amf_debug = {"gl_amfdebug", "0", CVAR_ARCHIVE};
cvar_t		amf_lightning = {"gl_lightning", "1", CVAR_ARCHIVE};// START shaman RFE 1022310
cvar_t		amf_lightning_size = {"gl_lightning_size", "3", CVAR_ARCHIVE};// END shaman RFE 1022310
cvar_t		amf_lightning_sparks = {"gl_lightning_sparks", "0.4", CVAR_ARCHIVE};
cvar_t		amf_lighting_vertex = {"gl_lighting_vertex", "1", CVAR_ARCHIVE};cvar_t		amf_lighting_colour = {"gl_lighting_colour", "1", CVAR_ARCHIVE};cvar_t		amf_inferno_trail = {"gl_inferno_trail", "2", CVAR_ARCHIVE};cvar_t		amf_inferno_speed = {"gl_inferno_speed", "1000", CVAR_ARCHIVE};cvar_t		amf_camera_chase = {"cl_camera_chase", "0", CVAR_ARCHIVE};cvar_t		amf_camera_death = {"cl_camera_death", "1", CVAR_ARCHIVE};
cvar_t		amf_motiontrails = {"gl_motiontrails", "0", CVAR_ARCHIVE};cvar_t		amf_motiontrails_wtf = {"gl_motiontrails_wtf", "0", CVAR_ARCHIVE};
cvar_t		amf_nailtrail_water = {"gl_nailtrail_water", "0", CVAR_ARCHIVE};cvar_t		amf_nailtrail_plasma = {"gl_nailtrail_plasma", "0", CVAR_ARCHIVE};cvar_t		amf_nailtrail = {"gl_nailtrail", "1", CVAR_ARCHIVE};
cvar_t		amf_tracker_flags = {"r_tracker_flags", "1", CVAR_ARCHIVE};cvar_t		amf_tracker_frags = {"r_tracker_frags", "1", CVAR_ARCHIVE};cvar_t		amf_tracker_streaks = {"r_tracker_streaks", "1", CVAR_ARCHIVE};cvar_t		amf_tracker_time = {"r_tracker_time", "4", CVAR_ARCHIVE};cvar_t		amf_tracker_messages = {"r_tracker_messages", "10", CVAR_ARCHIVE};
cvar_t		amf_part_gunshot = {"gl_particle_gunshots", "0", CVAR_ARCHIVE};cvar_t		amf_part_gunshot_type = {"gl_particle_gunshots_type", "1", CVAR_ARCHIVE};
cvar_t		amf_part_spikes = {"gl_particle_spikes", "0.1", CVAR_ARCHIVE};cvar_t		amf_part_spikes_type = {"gl_particle_spikes_type", "1", CVAR_ARCHIVE};
cvar_t		amf_part_shockwaves = {"gl_particle_shockwaves", "1", CVAR_ARCHIVE};cvar_t		amf_part_2dshockwaves = {"gl_particle_shockwaves_flat", "0", CVAR_ARCHIVE};
cvar_t		amf_part_blood = {"gl_particle_blood", "1", CVAR_ARCHIVE};cvar_t		amf_part_blood_color = {"gl_particle_blood_color", "1", CVAR_ARCHIVE};cvar_t		amf_part_blood_type = {"gl_particle_blood_type", "1", CVAR_ARCHIVE};
cvar_t		amf_part_explosion = {"gl_particle_explosions", "0", CVAR_ARCHIVE};cvar_t		amf_part_blobexplosion = {"gl_particle_blobs", "0.1", CVAR_ARCHIVE};
cvar_t		amf_part_fasttrails = {"gl_particle_fasttrails", "0", CVAR_ARCHIVE};cvar_t		amf_part_gibtrails = {"gl_particle_gibtrails", "1", CVAR_ARCHIVE};cvar_t		amf_part_muzzleflash = {"gl_particle_muzzleflash", "0", CVAR_ARCHIVE};cvar_t		amf_part_deatheffect = {"gl_particle_deatheffect", "1", CVAR_ARCHIVE};cvar_t		amf_part_teleport = {"gl_particle_telesplash", "0", CVAR_ARCHIVE};cvar_t		amf_part_fire = {"gl_particle_fire", "1", CVAR_ARCHIVE};cvar_t		amf_part_sparks = {"gl_particle_sparks", "1", CVAR_ARCHIVE};
cvar_t		amf_part_traillen    = {"gl_particle_trail_lenght", "1", CVAR_ARCHIVE};cvar_t		amf_part_trailtime   = {"gl_particle_trail_time",   "1", CVAR_ARCHIVE};cvar_t		amf_part_trailwidth  = {"gl_particle_trail_width",  "3", CVAR_ARCHIVE};cvar_t		amf_part_traildetail = {"gl_particle_trail_detail", "1", CVAR_ARCHIVE};cvar_t		amf_part_trailtype   = {"gl_particle_trail_type",   "1", CVAR_ARCHIVE};// END shaman :: balancing variables


int QW_strncmp (char *s1, char *s2)
{
	return strncmp(s1, s2, strlen(s2));
}

char *TP_ConvertToWhiteText(char *s)
{
	static char	buf[4096];	
	char	*out, *p;
	buf[0] = 0;
	out = buf;

	for (p = s; *p; p++) 
	{
		*out++ = *p - (*p & 128);
	}
	*out = 0;
	return buf;
}
///////////////////////////////////////
//Model Checking
typedef struct 
{
	char *number;
	char *name;
	char *description;
	qboolean notify;
} checkmodel_t;

checkmodel_t tp_checkmodels[]=
{
	{"6967", "Eyes Model", "", false},
	{"8986", "Player Model with External Skins", "Possible external fullbright skins.", true},
	{"20572", "Dox TF PAK", ".", false},
	{"37516", "Unknown non-cheating model", "I was told it wasn't a cheat model by Kay", false},
	{"13845", "TF 2.8 Standard", "", false},
	{"33168", "Q1 Standard", "", false},
	{"10560", "Ugly Quake Guy", "", false},
	{"44621", "Women's TF PAK", "", false},
	{"64351", "Razer PAK", "Glowing/Spiked plus other cheats.", true},
	{"17542", "Glow13 and Razer PAK", "Glowing/Spiked plus other cheats.", true},
	{"43672", "Modified/Original Glow", "This model is a cheat.", true},
	{"52774", "Thugguns PAK", "Spiked Player Model.", true},
	{"54273", "YourRom3ro's PAK", "Spiked Player Model.", true},
	{"47919", "DMPAK", "Spiked/Fullbright Player Model and loud water", true},
	{"60647", "Navy Seals PAK", "No Information (Considered cheat by TF Anti-Cheat", true},
	{"36870", "Clan Llama Force", "Fullbright Player Model", true},
	{"58759", "Clan LF / Acid Angel", "Fullbright Player Model plus other cheats", true},
	{"36870", "WBMod", "Hacked/Glowing Models", true},
	{"64524", "Fullbright", "Fullbright Player Model missing the head", true}
};

#define NUMCHECKMODELS (sizeof(tp_checkmodels) / sizeof(tp_checkmodels[0]))

checkmodel_t *TP_GetModels(char *s)	
{
	checkmodel_t	*model;
	int i;
	char *f;

	model = tp_checkmodels;

	f=TP_ConvertToWhiteText(s);
	for (i=0, model=tp_checkmodels ; i<NUMCHECKMODELS ; i++, model++)
	{
		if (strstr(f, model->number))
		{
			return model;
		}
	}

	return NULL;
}
//I was an admin on a server for a while, but about the only real cheats I could do anything about where model hacks
//So I developed this to save me time going around user'ing everybody
void CheckModels_f(void)
{
	int i;
	checkmodel_t *mod;
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (!cl.players[i].name[0]) //not in game
			continue; 
		//check pmodel
		mod = TP_GetModels(Info_ValueForKey(cl.players[i].userinfo, "pmodel"));
		if (mod == NULL)
		{
			if (!Info_ValueForKey(cl.players[i].userinfo, "pmodel")[0])
				Com_Printf("Warning: %s has hidden pmodel\n", Info_ValueForKey(cl.players[i].userinfo, "name"));
			else
				Com_Printf("Warning: %s has suspicious player model (Unknown Model: %s)\n", Info_ValueForKey(cl.players[i].userinfo, "name"), Info_ValueForKey(cl.players[i].userinfo, "pmodel"));
		}
		else if (mod->notify || !QW_strncmp(mod->number, "6967")) //report eyes as being unusual is used as player model
		{
			Com_Printf("Warning: %s has suspicious player model (%s)\n", Info_ValueForKey(cl.players[i].userinfo, "name"), mod->name);
		}
		mod = TP_GetModels(Info_ValueForKey(cl.players[i].userinfo, "emodel"));
		if (mod == NULL)
		{
			if (!Info_ValueForKey(cl.players[i].userinfo, "emodel")[0])
				Com_Printf("Warning: %s has hidden emodel\n", Info_ValueForKey(cl.players[i].userinfo, "name"));
			else
				Com_Printf("Warning: %s has ILLEGAL eyes model (Unknown Model: %s)\n", Info_ValueForKey(cl.players[i].userinfo, "name"), Info_ValueForKey(cl.players[i].userinfo, "emodel"));
		}
		else if (mod->notify || QW_strncmp(mod->number, "6967")) //anything other 6967 is a hack
		{
			Com_Printf("Warning: %s has ILLEGAL eyes model (%s)\n", Info_ValueForKey(cl.players[i].userinfo, "name"), mod->name);
		}
	}	
};
////////////////////////////////
/// test stuff
void Draw_AlphaFill (int x, int y, int w, int h, int c, float alpha);
void Draw_AlphaWindow (int x1, int y1, int x2, int y2, int col, float alpha)
{
	int dist;
	dist = x2-x1;
	Draw_Fill (x1, y1, dist+1, 1, 2);							//Border - Top
	Draw_Fill (x1, y2, dist, 1, 1);							//Border - Bottom
	dist = y2-y1;
	Draw_Fill (x1, y1, 1, dist, 2);							//Border - Left
	Draw_Fill (x2, y1, 1, dist+1, 1);							//Border - Right
	
	Draw_AlphaFill (x1, y1, x2-x1, y2-y1, col, alpha);

}
void SCR_DrawAMFstats (void) 
{
	int x, y, numlines;
	char st[80];

	if (!amf_showstats.value)
		return;

	numlines = 6;
	numlines += 2; //margins
	
	sprintf(st, "Particle Count: %3d ", ParticleCount);
	x = vid.width - strlen(st)*8 -8;
	y = vid.height*0.1;
	Draw_AlphaWindow(x-8, y-8, x+strlen(st)*8, y+numlines*8-8, 1, 0.33);
	Draw_String(x, y, st);
	
	sprintf(st, "Highest: %3d ", ParticleCountHigh);
	x = vid.width - strlen(st) * 8 - 8;
	y = y + 8;
	Draw_String(x, y, st);

	sprintf(st, "Corona Count: %3d ", CoronaCount);
	x = vid.width - strlen(st) * 8 - 8;
	y = y + 8;
	Draw_String(x, y, st);

	sprintf(st, "Highest: %3d ", CoronaCountHigh);
	x = vid.width - strlen(st) * 8 - 8;
	y = y + 8;
	Draw_String(x, y, st);

	sprintf(st, "Motion Trails: %3d ", MotionBlurCount);
	x = vid.width - strlen(st) * 8 - 8;
	y = y + 8;
	Draw_String(x, y, st);

	sprintf(st, "Highest: %3d ", MotionBlurCountHigh);
	x = vid.width - strlen(st) * 8 - 8;
	y = y + 8;
	Draw_String(x, y, st);

}

void Amf_SetMode_f(void)
{
	char mode[10];
	if (Cmd_Argc() != 2) 
	{
		Com_Printf("Usage: %s [modename]\n", Cmd_Argv(0));
		return;
	}
	sprintf(mode, Cmd_Argv(1));
	if (!strcmp(mode, "newtrails"))
	{
			Cvar_SetValue (&amf_buildingsparks, 3);
			Cvar_SetValue (&amf_weather_rain, 0);
			Cvar_SetValue (&amf_lightning_sparks, 3);
			Cvar_SetValue (&amf_part_spikes, 5);
			Cvar_SetValue (&amf_part_gunshot, 5);
			Cvar_SetValue (&amf_part_explosion, 5);
			Cvar_SetValue (&amf_part_traillen, 1);
			Cvar_SetValue (&amf_part_trailtime, 1.5);
			Cvar_SetValue (&amf_part_sparks, 1);
			Cvar_SetValue (&amf_nailtrail_plasma, 0);
			Cvar_SetValue (&amf_part_traillen, 1);
			Cvar_SetValue (&amf_part_gunshot_type, 1);
			Cvar_SetValue (&amf_part_spikes_type, 1);
			Cvar_SetValue (&amf_part_trailwidth, 3);
			Cvar_SetValue (&amf_part_traildetail, 1);
			Cvar_SetValue (&amf_part_trailtype, 2);
			Cvar_SetValue (&gl_bounceparticles, 1);
	}
	else if (!strcmp(mode, "vultwah"))
	{
		Cvar_SetValue (&amf_showstats, 1);
		Cvar_SetValue (&amf_debug, 1);
		Cvar_SetValue (&amf_part_spikes, 1);
		Cvar_SetValue (&amf_part_gunshot, 0.5);
		Cvar_SetValue (&amf_part_explosion, 1);
		Cvar_SetValue (&amf_part_blobexplosion, 0.5);
		Cvar_SetValue (&amf_part_teleport, 0.5);
		Cvar_SetValue (&gl_bounceparticles, 0);
	}
	
}

void InitVXStuff(void)
{
	coronatexture = GL_LoadTextureImage ("textures/flash", NULL, 0, 0,  7);
	gunflashtexture = GL_LoadTextureImage ("textures/gunflash", NULL, 0, 0,  7);
	explosionflashtexture1 = GL_LoadTextureImage ("textures/explosionflash1", NULL, 0, 0,  7);
	explosionflashtexture2 = GL_LoadTextureImage ("textures/explosionflash2", NULL, 0, 0,  7);
	explosionflashtexture3 = GL_LoadTextureImage ("textures/explosionflash3", NULL, 0, 0,  7);
	explosionflashtexture4 = GL_LoadTextureImage ("textures/explosionflash4", NULL, 0, 0,  7);
	explosionflashtexture5 = GL_LoadTextureImage ("textures/explosionflash5", NULL, 0, 0,  7);
	explosionflashtexture6 = GL_LoadTextureImage ("textures/explosionflash6", NULL, 0, 0,  7);
	explosionflashtexture7 = GL_LoadTextureImage ("textures/explosionflash7", NULL, 0, 0,  7);
	InitCoronas();

	Init_VLights();
	Cvar_SetCurrentGroup(CVAR_GROUP_SBAR);	Cvar_Register (&amf_stat_loss);	Cvar_ResetCurrentGroup();	Cvar_SetCurrentGroup(CVAR_GROUP_SPECTATOR);
	Cvar_Register (&amf_camera_chase);	Cvar_Register (&amf_camera_death);	Cvar_ResetCurrentGroup();		Cvar_SetCurrentGroup(CVAR_GROUP_SCREEN);
	Cvar_Register (&amf_showstats);	Cvar_Register (&amf_tracker_frags);	Cvar_Register (&amf_tracker_flags);	Cvar_Register (&amf_tracker_streaks);	Cvar_Register (&amf_tracker_messages);	Cvar_Register (&amf_tracker_time);	Cvar_ResetCurrentGroup();	Cvar_SetCurrentGroup(CVAR_GROUP_EYECANDY);	Cvar_Register (&amf_hiderockets);	Cvar_Register (&amf_hidenails);	Cvar_ResetCurrentGroup();
	Cvar_SetCurrentGroup(CVAR_GROUP_LIGHTING);	Cvar_Register (&amf_coronas);	Cvar_Register (&amf_lighting_vertex);	Cvar_Register (&amf_lighting_colour);
	Cvar_ResetCurrentGroup();

	Cvar_SetCurrentGroup(CVAR_GROUP_PARTICLES);	Cvar_Register (&amf_part_blood);	Cvar_Register (&amf_part_blood_color);	Cvar_Register (&amf_part_blood_type);	Cvar_Register (&amf_part_muzzleflash);	Cvar_Register (&amf_part_fire);	Cvar_Register (&amf_part_deatheffect);	Cvar_Register (&amf_part_gunshot);	Cvar_Register (&amf_part_gunshot_type);	Cvar_Register (&amf_part_spikes);	Cvar_Register (&amf_part_spikes_type);	Cvar_Register (&amf_part_explosion);	Cvar_Register (&amf_part_blobexplosion);// START shaman :: balancing variables
//	Cvar_Register (&amf_part_teleport);// END shaman :: balancing variables
	Cvar_Register (&amf_part_sparks);	Cvar_Register (&amf_part_2dshockwaves);	Cvar_Register (&amf_part_shockwaves);	Cvar_Register (&amf_part_gibtrails);	Cvar_Register (&amf_part_fasttrails);
	Cvar_Register (&amf_part_traillen);	Cvar_Register (&amf_part_trailtime);	Cvar_Register (&amf_part_traildetail);
	Cvar_Register (&amf_part_trailwidth);
	Cvar_Register (&amf_part_trailtype);

	Cvar_ResetCurrentGroup();

	Cvar_SetCurrentGroup(CVAR_GROUP_OPENGL);	Cvar_Register (&tei_lavafire);	Cvar_Register (&tei_slime);	Cvar_Register (&amf_weather_rain);	Cvar_Register (&amf_weather_rain_fast);	Cvar_Register (&amf_nailtrail);	Cvar_Register (&amf_nailtrail_plasma);	Cvar_Register (&amf_nailtrail_water);	Cvar_Register (&amf_extratrails);	Cvar_Register (&amf_underwater_trails);	Cvar_Register (&amf_detpacklights);	Cvar_Register (&amf_buildingsparks);	Cvar_Register (&amf_lightning);// START shaman RFE 1022310
	Cvar_Register (&amf_lightning_size);
// END shaman RFE 1022310
	Cvar_Register (&amf_lightning_sparks);	Cvar_Register (&amf_motiontrails);	Cvar_Register (&amf_motiontrails_wtf);	Cvar_Register (&amf_waterripple);	Cvar_Register (&amf_debug);	Cvar_Register (&amf_inferno_trail);	Cvar_Register (&amf_inferno_speed);	Cvar_Register (&amf_cutf_tesla_effect);
	Cvar_ResetCurrentGroup();
	Cmd_AddCommand ("gl_checkmodels", CheckModels_f);
	Cmd_AddCommand ("gl_inferno", InfernoFire_f);
	Cmd_AddCommand ("gl_setmode", Amf_SetMode_f);
}

#endif
