#include "retro_common.h"
#include "retro_input.h"

struct RomBiosInfo mvs_bioses[] = {
	{"asia-s3.rom",       0x91b64be3, 0x00, "MVS Asia/Europe ver. 6 (1 slot)",  1 },
	{"sp-s2.sp1",         0x9036d879, 0x01, "MVS Asia/Europe ver. 5 (1 slot)",  2 },
	{"sp-s.sp1",          0xc7f2fa45, 0x02, "MVS Asia/Europe ver. 3 (4 slot)",  3 },
	{"usa_2slt.bin",      0xe72943de, 0x03, "MVS USA ver. 5 (2 slot)"        ,  4 },
	{"sp-e.sp1",          0x2723a5b5, 0x04, "MVS USA ver. 5 (6 slot)"        ,  5 },
	{"vs-bios.rom",       0xf0e8f27d, 0x05, "MVS Japan ver. 6 (? slot)"      ,  6 },
	{"sp-j2.sp1",         0xacede59C, 0x06, "MVS Japan ver. 5 (? slot)"      ,  7 },
	{"sp1.jipan.1024",    0x9fb0abe4, 0x07, "MVS Japan ver. 3 (4 slot)"      ,  8 },
	{"sp-45.sp1",         0x03cc9f6a, 0x08, "NEO-MVH MV1C (Asia)"            ,  9 },
	{"japan-j3.bin",      0xdff6d41f, 0x09, "MVS Japan (J3)"                 , 10 },
	{"sp-1v1_3db8c.bin",  0x162f0ebe, 0x0d, "Deck ver. 6 (Git Ver 1.3)"      , 11 },
	{NULL, 0, 0, NULL, 0 }
};

struct RomBiosInfo aes_bioses[] = {
	{"neo-po.bin",        0x16d0c132, 0x0a, "AES Japan"                      ,  1 },
	{"neo-epo.bin",       0xd27a71f1, 0x0b, "AES Asia"                       ,  2 },
	{"neodebug.bin",      0x698ebb7d, 0x0c, "Development Kit"                ,  3 },
	{NULL, 0, 0, NULL, 0 }
};

struct RomBiosInfo uni_bioses[] = {
	{"uni-bios_4_0.rom",  0xa7aab458, 0x0e, "Universe BIOS ver. 4.0"         ,  1 },
	{"uni-bios_3_3.rom",  0x24858466, 0x0f, "Universe BIOS ver. 3.3"         ,  2 },
	{"uni-bios_3_2.rom",  0xa4e8b9b3, 0x10, "Universe BIOS ver. 3.2"         ,  3 },
	{"uni-bios_3_1.rom",  0x0c58093f, 0x11, "Universe BIOS ver. 3.1"         ,  4 },
	{"uni-bios_3_0.rom",  0xa97c89a9, 0x12, "Universe BIOS ver. 3.0"         ,  5 },
	{"uni-bios_2_3.rom",  0x27664eb5, 0x13, "Universe BIOS ver. 2.3"         ,  6 },
	{"uni-bios_2_3o.rom", 0x601720ae, 0x14, "Universe BIOS ver. 2.3 (alt)"   ,  7 },
	{"uni-bios_2_2.rom",  0x2d50996a, 0x15, "Universe BIOS ver. 2.2"         ,  8 },
	{"uni-bios_2_1.rom",  0x8dabf76b, 0x16, "Universe BIOS ver. 2.1"         ,  9 },
	{"uni-bios_2_0.rom",  0x0c12c2ad, 0x17, "Universe BIOS ver. 2.0"         , 10 },
	{"uni-bios_1_3.rom",  0xb24b44a0, 0x18, "Universe BIOS ver. 1.3"         , 11 },
	{"uni-bios_1_2.rom",  0x4fa698e9, 0x19, "Universe BIOS ver. 1.2"         , 12 },
	{"uni-bios_1_2o.rom", 0xe19d3ce9, 0x1a, "Universe BIOS ver. 1.2 (alt)"   , 13 },
	{"uni-bios_1_1.rom",  0x5dda0d84, 0x1b, "Universe BIOS ver. 1.1"         , 14 },
	{"uni-bios_1_0.rom",  0x0ce453a0, 0x1c, "Universe BIOS ver. 1.0"         , 15 },
	{NULL, 0, 0, NULL, 0 }
};

RomBiosInfo *available_mvs_bios = NULL;
RomBiosInfo *available_aes_bios = NULL;
RomBiosInfo *available_uni_bios = NULL;
std::vector<dipswitch_core_option> dipswitch_core_options;
struct GameInp *pgi_reset;
struct GameInp *pgi_diag;
bool is_neogeo_game = false;
bool allow_neogeo_mode = true;
bool bVerticalMode = false;
bool bAllowDepth32 = false;
UINT32 nFrameskip = 1;
#if !defined(SF2000)
INT32 g_audio_samplerate = 48000;
#else
INT32 g_audio_samplerate = 11025;
#endif
UINT8 *diag_input;
neo_geo_modes g_opt_neo_geo_mode = NEO_GEO_MODE_MVS;

#ifdef USE_CYCLONE
// 0 - c68k, 1 - m68k
// we don't use cyclone by default because it breaks savestates cross-platform compatibility (including netplay)
int nSekCpuCore = 1;
static bool bCycloneEnabled = false;
#endif

static UINT8 diag_input_start[] =       {RETRO_DEVICE_ID_JOYPAD_START,  RETRO_DEVICE_ID_JOYPAD_EMPTY };
static UINT8 diag_input_start_a_b[] =   {RETRO_DEVICE_ID_JOYPAD_START,  RETRO_DEVICE_ID_JOYPAD_A, RETRO_DEVICE_ID_JOYPAD_B, RETRO_DEVICE_ID_JOYPAD_EMPTY };
static UINT8 diag_input_start_l_r[] =   {RETRO_DEVICE_ID_JOYPAD_START,  RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_R, RETRO_DEVICE_ID_JOYPAD_EMPTY };
static UINT8 diag_input_select[] =      {RETRO_DEVICE_ID_JOYPAD_SELECT, RETRO_DEVICE_ID_JOYPAD_EMPTY };
static UINT8 diag_input_select_a_b[] =  {RETRO_DEVICE_ID_JOYPAD_SELECT, RETRO_DEVICE_ID_JOYPAD_A, RETRO_DEVICE_ID_JOYPAD_B, RETRO_DEVICE_ID_JOYPAD_EMPTY };
static UINT8 diag_input_select_l_r[] =  {RETRO_DEVICE_ID_JOYPAD_SELECT, RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_R, RETRO_DEVICE_ID_JOYPAD_EMPTY };

// Global core options
static const struct retro_variable var_empty = { NULL, NULL };
static const struct retro_variable var_fbneo_allow_depth_32 = { "fbneo-allow-depth-32", "Use 32-bits color depth when available; disabled|enabled" };
static const struct retro_variable var_fbneo_vertical_mode = { "fbneo-vertical-mode", "Vertical mode; disabled|enabled" };
static const struct retro_variable var_fbneo_frameskip = { "fbneo-frameskip", "Frameskip; 0|1|2|3|4|5" };
static const struct retro_variable var_fbneo_cpu_speed_adjust = { "fbneo-cpu-speed-adjust", "CPU overclock; 100|110|120|130|140|150|160|170|180|190|200" };
static const struct retro_variable var_fbneo_diagnostic_input = { "fbneo-diagnostic-input", "Diagnostic Input; None|Hold Start|Start + A + B|Hold Start + A + B|Start + L + R|Hold Start + L + R|Hold Select|Select + A + B|Hold Select + A + B|Select + L + R|Hold Select + L + R" };
static const struct retro_variable var_fbneo_hiscores = { "fbneo-hiscores", "Hiscores; enabled|disabled" };
static const struct retro_variable var_fbneo_samplerate = { "fbneo-samplerate", "Samplerate (need to quit retroarch); 48000|44100|22050|11025" };
static const struct retro_variable var_fbneo_sample_interpolation = { "fbneo-sample-interpolation", "Sample Interpolation; 4-point 3rd order|2-point 1st order|disabled" };
static const struct retro_variable var_fbneo_fm_interpolation = { "fbneo-fm-interpolation", "FM Interpolation; 4-point 3rd order|disabled" };
static const struct retro_variable var_fbneo_analog_speed = { "fbneo-analog-speed", "Analog Speed; 10|9|8|7|6|5|4|3|2|1" };
#ifdef USE_CYCLONE
static const struct retro_variable var_fbneo_cyclone = { "fbneo-cyclone", "Cyclone (need to quit retroarch, change savestate format, use at your own risk); disabled|enabled" };
#endif

// Neo Geo core options
static const struct retro_variable var_fbneo_neogeo_mode = { "fbneo-neogeo-mode", "Force Neo Geo mode (if available); MVS|AES|UNIBIOS|DIPSWITCH" };

// Replace the char c_find by the char c_replace in the destination c string
char* str_char_replace(char* destination, char c_find, char c_replace)
{
	for (unsigned str_idx = 0; str_idx < strlen(destination); str_idx++)
	{
		if (destination[str_idx] == c_find)
			destination[str_idx] = c_replace;
	}

	return destination;
}

void set_neo_system_bios()
{
	if (g_opt_neo_geo_mode == NEO_GEO_MODE_DIPSWITCH)
	{
		// Nothing to do in DIPSWITCH mode because the NeoSystem variable is changed by the DIP Switch core option
		log_cb(RETRO_LOG_INFO, "DIPSWITCH Neo Geo Mode selected => NeoSystem: 0x%02x.\n", NeoSystem);
	}
	else if (g_opt_neo_geo_mode == NEO_GEO_MODE_MVS)
	{
		NeoSystem &= ~(UINT8)0x1f;
		if (available_mvs_bios)
		{
			NeoSystem |= available_mvs_bios->NeoSystem;
			log_cb(RETRO_LOG_INFO, "MVS Neo Geo Mode selected => Set NeoSystem: 0x%02x (%s [0x%08x] (%s)).\n", NeoSystem, available_mvs_bios->filename, available_mvs_bios->crc, available_mvs_bios->friendly_name);
		}
		else
		{
			// fallback to another bios type if we didn't find the bios selected by the user
			available_mvs_bios = (available_aes_bios) ? available_aes_bios : available_uni_bios;
			if (available_mvs_bios)
			{
				NeoSystem |= available_mvs_bios->NeoSystem;
				log_cb(RETRO_LOG_WARN, "MVS Neo Geo Mode selected but MVS bios not available => fall back to another: 0x%02x (%s [0x%08x] (%s)).\n", NeoSystem, available_mvs_bios->filename, available_mvs_bios->crc, available_mvs_bios->friendly_name);
			}
		}
	}
	else if (g_opt_neo_geo_mode == NEO_GEO_MODE_AES)
	{
		NeoSystem &= ~(UINT8)0x1f;
		if (available_aes_bios)
		{
			NeoSystem |= available_aes_bios->NeoSystem;
			log_cb(RETRO_LOG_INFO, "AES Neo Geo Mode selected => Set NeoSystem: 0x%02x (%s [0x%08x] (%s)).\n", NeoSystem, available_aes_bios->filename, available_aes_bios->crc, available_aes_bios->friendly_name);
		}
		else
		{
			// fallback to another bios type if we didn't find the bios selected by the user
			available_aes_bios = (available_mvs_bios) ? available_mvs_bios : available_uni_bios;
			if (available_aes_bios)
			{
				NeoSystem |= available_aes_bios->NeoSystem;
				log_cb(RETRO_LOG_WARN, "AES Neo Geo Mode selected but AES bios not available => fall back to another: 0x%02x (%s [0x%08x] (%s)).\n", NeoSystem, available_aes_bios->filename, available_aes_bios->crc, available_aes_bios->friendly_name);
			}
		}
	}
	else if (g_opt_neo_geo_mode == NEO_GEO_MODE_UNIBIOS)
	{
		NeoSystem &= ~(UINT8)0x1f;
		if (available_uni_bios)
		{
			NeoSystem |= available_uni_bios->NeoSystem;
			log_cb(RETRO_LOG_INFO, "UNIBIOS Neo Geo Mode selected => Set NeoSystem: 0x%02x (%s [0x%08x] (%s)).\n", NeoSystem, available_uni_bios->filename, available_uni_bios->crc, available_uni_bios->friendly_name);
		}
		else
		{
			// fallback to another bios type if we didn't find the bios selected by the user
			available_uni_bios = (available_mvs_bios) ? available_mvs_bios : available_aes_bios;
			if (available_uni_bios)
			{
				NeoSystem |= available_uni_bios->NeoSystem;
				log_cb(RETRO_LOG_WARN, "UNIBIOS Neo Geo Mode selected but UNIBIOS not available => fall back to another: 0x%02x (%s [0x%08x] (%s)).\n", NeoSystem, available_uni_bios->filename, available_uni_bios->crc, available_uni_bios->friendly_name);
			}
		}
	}
}

void evaluate_neogeo_bios_mode(const char* drvname)
{
	if (!is_neogeo_game)
		return;

	bool is_neogeo_needs_specific_bios = false;
	bool is_bios_dipswitch_found = false;

	// search the BIOS dipswitch
	for (int dip_idx = 0; dip_idx < dipswitch_core_options.size(); dip_idx++)
	{
		if (strcasecmp(dipswitch_core_options[dip_idx].friendly_name, "BIOS") == 0)
		{
			is_bios_dipswitch_found = true;
			if (dipswitch_core_options[dip_idx].values.size() > 0)
			{
				// values[0] is the default value of the dipswitch
				// if the default is different than 0, this means that a different Bios is needed
				if (dipswitch_core_options[dip_idx].values[0].bdi.nSetting != 0x00)
				{
					is_neogeo_needs_specific_bios = true;
					break;
				}
			}
		}
	}

	// Games without the BIOS dipswitch don't handle alternative bioses very well
	if (!is_bios_dipswitch_found)
	{
		is_neogeo_needs_specific_bios = true;
	}

	if (is_neogeo_needs_specific_bios)
	{
		// disable the NeoGeo mode core option
		allow_neogeo_mode = false;

		// set the NeoGeo mode to DIPSWITCH to rely on the Default Bios Dipswitch
		g_opt_neo_geo_mode = NEO_GEO_MODE_DIPSWITCH;
	}
}

void set_environment()
{
	std::vector<const retro_variable*> vars_systems;
	struct retro_variable *vars;
#ifdef _MSC_VER
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
	struct retro_vfs_interface_info vfs_iface_info;
#endif
#endif

	// Add the Global core options
	vars_systems.push_back(&var_fbneo_allow_depth_32);
	vars_systems.push_back(&var_fbneo_vertical_mode);
	vars_systems.push_back(&var_fbneo_frameskip);
	vars_systems.push_back(&var_fbneo_cpu_speed_adjust);
	vars_systems.push_back(&var_fbneo_hiscores);
	if (nGameType != RETRO_GAME_TYPE_NEOCD)
		vars_systems.push_back(&var_fbneo_samplerate);
	vars_systems.push_back(&var_fbneo_sample_interpolation);
	vars_systems.push_back(&var_fbneo_fm_interpolation);
	vars_systems.push_back(&var_fbneo_analog_speed);
#ifdef USE_CYCLONE
	vars_systems.push_back(&var_fbneo_cyclone);
#endif

	if (pgi_diag)
	{
		vars_systems.push_back(&var_fbneo_diagnostic_input);
	}

	if (is_neogeo_game)
	{
		// Add the Neo Geo core options
		if (allow_neogeo_mode)
			vars_systems.push_back(&var_fbneo_neogeo_mode);
	}

	int nbr_vars = vars_systems.size();
	int nbr_dips = dipswitch_core_options.size();

#if 0
	log_cb(RETRO_LOG_INFO, "set_environment: SYSTEM: %d, DIPSWITCH: %d\n", nbr_vars, nbr_dips);
#endif

	vars = (struct retro_variable*)calloc(nbr_vars + nbr_dips + 1, sizeof(struct retro_variable));

	int idx_var = 0;

	// Add the System core options
	for (int i = 0; i < nbr_vars; i++, idx_var++)
	{
		vars[idx_var] = *vars_systems[i];
#if 0
		log_cb(RETRO_LOG_INFO, "retro_variable (SYSTEM)    { '%s', '%s' }\n", vars[idx_var].key, vars[idx_var].value);
#endif
	}

	// Add the DIP switches core options
	for (int dip_idx = 0; dip_idx < nbr_dips; dip_idx++)
	{
		// Filter out the BIOS dipswitch if present while the game needs specific bios
		if (!is_neogeo_game || allow_neogeo_mode || strcasecmp(dipswitch_core_options[dip_idx].friendly_name, "BIOS") != 0)
		{
			vars[idx_var].key = dipswitch_core_options[dip_idx].option_name;
			vars[idx_var].value = dipswitch_core_options[dip_idx].values_str.c_str();
#if 0
			log_cb(RETRO_LOG_INFO, "retro_variable (DIPSWITCH) { '%s', '%s' }\n", vars[idx_var].key, vars[idx_var].value);
#endif
			idx_var++;
		}
	}

	vars[idx_var] = var_empty;
	environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
	free(vars);

	// Initialize VFS
	// Only on UWP for now, since EEPROM saving is not VFS aware
#ifdef _MSC_VER
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
	vfs_iface_info.required_interface_version = FILESTREAM_REQUIRED_VFS_VERSION;
	vfs_iface_info.iface                      = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &vfs_iface_info))
		filestream_vfs_init(&vfs_iface_info);
#endif
#endif
}

void check_variables(void)
{
	struct retro_variable var = {0};

	var.key = var_fbneo_cpu_speed_adjust.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "110") == 0)
			nBurnCPUSpeedAdjust = 0x0110;
		else if (strcmp(var.value, "120") == 0)
			nBurnCPUSpeedAdjust = 0x0120;
		else if (strcmp(var.value, "130") == 0)
			nBurnCPUSpeedAdjust = 0x0130;
		else if (strcmp(var.value, "140") == 0)
			nBurnCPUSpeedAdjust = 0x0140;
		else if (strcmp(var.value, "150") == 0)
			nBurnCPUSpeedAdjust = 0x0150;
		else if (strcmp(var.value, "160") == 0)
			nBurnCPUSpeedAdjust = 0x0160;
		else if (strcmp(var.value, "170") == 0)
			nBurnCPUSpeedAdjust = 0x0170;
		else if (strcmp(var.value, "180") == 0)
			nBurnCPUSpeedAdjust = 0x0180;
		else if (strcmp(var.value, "190") == 0)
			nBurnCPUSpeedAdjust = 0x0190;
		else if (strcmp(var.value, "200") == 0)
			nBurnCPUSpeedAdjust = 0x0200;
		else
			nBurnCPUSpeedAdjust = 0x0100;
	}

	var.key = var_fbneo_allow_depth_32.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "enabled") == 0)
			bAllowDepth32 = true;
		else
			bAllowDepth32 = false;
	}

	var.key = var_fbneo_vertical_mode.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "enabled") == 0)
			bVerticalMode = true;
		else
			bVerticalMode = false;
	}

	var.key = var_fbneo_frameskip.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "0") == 0)
			nFrameskip = 1;
		else if (strcmp(var.value, "1") == 0)
			nFrameskip = 2;
		else if (strcmp(var.value, "2") == 0)
			nFrameskip = 3;
		else if (strcmp(var.value, "3") == 0)
			nFrameskip = 4;
		else if (strcmp(var.value, "4") == 0)
			nFrameskip = 5;
		else if (strcmp(var.value, "5") == 0)
			nFrameskip = 6;
	}

	if (pgi_diag)
	{
		var.key = var_fbneo_diagnostic_input.key;
		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
		{
			diag_input = NULL;
			SetDiagInpHoldFrameDelay(0);
			if (strcmp(var.value, "Hold Start") == 0)
			{
				diag_input = diag_input_start;
				SetDiagInpHoldFrameDelay(60);
			}
			else if(strcmp(var.value, "Start + A + B") == 0)
			{
				diag_input = diag_input_start_a_b;
				SetDiagInpHoldFrameDelay(0);
			}
			else if(strcmp(var.value, "Hold Start + A + B") == 0)
			{
				diag_input = diag_input_start_a_b;
				SetDiagInpHoldFrameDelay(60);
			}
			else if(strcmp(var.value, "Start + L + R") == 0)
			{
				diag_input = diag_input_start_l_r;
				SetDiagInpHoldFrameDelay(0);
			}
			else if(strcmp(var.value, "Hold Start + L + R") == 0)
			{
				diag_input = diag_input_start_l_r;
				SetDiagInpHoldFrameDelay(60);
			}
			else if(strcmp(var.value, "Hold Select") == 0)
			{
				diag_input = diag_input_select;
				SetDiagInpHoldFrameDelay(60);
			}
			else if(strcmp(var.value, "Select + A + B") == 0)
			{
				diag_input = diag_input_select_a_b;
				SetDiagInpHoldFrameDelay(0);
			}
			else if(strcmp(var.value, "Hold Select + A + B") == 0)
			{
				diag_input = diag_input_select_a_b;
				SetDiagInpHoldFrameDelay(60);
			}
			else if(strcmp(var.value, "Select + L + R") == 0)
			{
				diag_input = diag_input_select_l_r;
				SetDiagInpHoldFrameDelay(0);
			}
			else if(strcmp(var.value, "Hold Select + L + R") == 0)
			{
				diag_input = diag_input_select_l_r;
				SetDiagInpHoldFrameDelay(60);
			}
		}
	}

	if (is_neogeo_game)
	{
		if (allow_neogeo_mode)
		{
			var.key = var_fbneo_neogeo_mode.key;
			if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
			{
				if (strcmp(var.value, "MVS") == 0)
					g_opt_neo_geo_mode = NEO_GEO_MODE_MVS;
				else if (strcmp(var.value, "AES") == 0)
					g_opt_neo_geo_mode = NEO_GEO_MODE_AES;
				else if (strcmp(var.value, "UNIBIOS") == 0)
					g_opt_neo_geo_mode = NEO_GEO_MODE_UNIBIOS;
				else if (strcmp(var.value, "DIPSWITCH") == 0)
					g_opt_neo_geo_mode = NEO_GEO_MODE_DIPSWITCH;
			}
		}
	}

	var.key = var_fbneo_hiscores.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "enabled") == 0)
			EnableHiscores = true;
		else
			EnableHiscores = false;
	}

	if (nGameType != RETRO_GAME_TYPE_NEOCD)
	{
		var.key = var_fbneo_samplerate.key;
		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
		{
			if (strcmp(var.value, "48000") == 0)
				g_audio_samplerate = 48000;
			else if (strcmp(var.value, "44100") == 0)
				g_audio_samplerate = 44100;
			else if (strcmp(var.value, "22050") == 0)
				g_audio_samplerate = 22050;
			else if (strcmp(var.value, "11025") == 0)
				g_audio_samplerate = 11025;
			else
#if !defined(SF2000)
				g_audio_samplerate = 48000;
#else
				g_audio_samplerate = 11025;
#endif
		}
	}
	else
	{
		// src/burn/drv/neogeo/neo_run.cpp is mentioning issues with ngcd cdda playback if samplerate isn't 44100
#if !defined(SF2000)
		g_audio_samplerate = 44100;
#else
		g_audio_samplerate = 11025;
#endif
	}

	var.key = var_fbneo_sample_interpolation.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "4-point 3rd order") == 0)
			nInterpolation = 3;
		else if (strcmp(var.value, "2-point 1st order") == 0)
			nInterpolation = 1;
		else if (strcmp(var.value, "disabled") == 0)
			nInterpolation = 0;
		else
			nInterpolation = 3;
	}

	var.key = var_fbneo_fm_interpolation.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "4-point 3rd order") == 0)
			nFMInterpolation = 3;
		else if (strcmp(var.value, "disabled") == 0)
			nFMInterpolation = 0;
		else
			nFMInterpolation = 3;
	}

	var.key = var_fbneo_analog_speed.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "10") == 0)
			nAnalogSpeed = 0x0100;
		else if (strcmp(var.value, "9") == 0)
			nAnalogSpeed = 0x00F0;
		else if (strcmp(var.value, "8") == 0)
			nAnalogSpeed = 0x00E0;
		else if (strcmp(var.value, "7") == 0)
			nAnalogSpeed = 0x00C0;
		else if (strcmp(var.value, "6") == 0)
			nAnalogSpeed = 0x00B0;
		else if (strcmp(var.value, "5") == 0)
			nAnalogSpeed = 0x00A0;
		else if (strcmp(var.value, "4") == 0)
			nAnalogSpeed = 0x0090;
		else if (strcmp(var.value, "3") == 0)
			nAnalogSpeed = 0x0080;
		else if (strcmp(var.value, "2") == 0)
			nAnalogSpeed = 0x0070;
		else if (strcmp(var.value, "1") == 0)
			nAnalogSpeed = 0x0060;
		else
			nAnalogSpeed = 0x0100;
	}

#ifdef USE_CYCLONE
	var.key = var_fbneo_cyclone.key;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
	{
		if (strcmp(var.value, "enabled") == 0)
			bCycloneEnabled = true;
		else if (strcmp(var.value, "disabled") == 0)
			bCycloneEnabled = false;
	}
#endif
}

#ifdef USE_CYCLONE
void SetSekCpuCore()
{
	nSekCpuCore = (bCycloneEnabled ? 0 : 1);
}
#endif
