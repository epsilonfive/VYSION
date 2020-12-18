//#include "main.h"
#include <capnhook.h>

//hooks
//extern hook_t StartHook;
//extern size_t StartHook_size;
extern hook_t FixStopHook;
extern size_t FixStopHook_size;

#include "vysion.h"
#include "vysion_saving.h"
#include "vysion_fileops.h"
#include "vysion_program.h"
#include "vysion_gui.h"
#include "optix/optix.h"
#include "gfx/gfx.h"

/* Keep these headers */
#include <tice.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <graphx.h>
#include <fileioc.h>
#include <keypadc.h>
#include <intce.h>
#include <math.h>

//definitions
struct vysion_file_t *vysion_file;
struct vysion_folder_t *vysion_folder;
struct vysion_filesysteminfo_t vysion_filesysteminfo;
struct vysion_programdata_t vysion_programdata;
struct vysion_tempfile_t *vysion_tempfile;
struct vysion_settings_t vysion_settings;

//Til I collapse I'm spillin these raps long as you feel em
//Til the day that I drop you'll never say that I'm not killin em
//cause when I am not, then I'ma stop pennin em
