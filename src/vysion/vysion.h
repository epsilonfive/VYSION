#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
//of course I don't need this because I never write bugs
#include <debug.h>
#include "vysion_saving.h"
#include "vysion_fileops.h"
#include "vysion_program.h"
#include "vysion_main.h"
#include "vysion_gui.h"
#include "gfx/gfx.h"
//#include "optix.h"

//defines
//name and information
#define VYSION_NAME "VYSION"
#define VYSION_VERSION "0.5.0"
#define VYSION_SETTINGS_VERSION 0
#define VYSION_FILESYSTEM_VERSION 1
#define VYSION_STATE_VERSION 0
//appvars
#define VYSION_FILESYSTEM_APPVAR "VYSFILE"
#define VYSION_DATA_APPVAR "VYSDATA"
#define VYSION_STATE_APPVAR "VYSSTATE"
#define VYSION_TEMPFILE "VYSTEMP"
#define VYSION_WALLPAPER_STRING "VYSWALFIX"
//start menu and program types
#define VYSION_START_MENU_OPTIONS 8
#define VYSION_PBASIC_TYPE 0
#define VYSION_BASIC_TYPE 1
#define VYSION_ICES_TYPE 2
#define VYSION_ICE_TYPE 3
#define VYSION_C_TYPE 4
#define VYSION_ASM_TYPE 5
#define VYSION_APPVAR_TYPE 6
#define VYSION_UNKNOWN_TYPE 7
//distributions
#define VYSION_DISTRIBUTION_CORE 0
#define VYSION_DISTRIBUTION_BASIC 1
#define VYSION_DISTRIBUTION_ULTIMATE 2
//folders
#define VYSION_FOLDER_NUM_CONSTANTS 4
#define VYSION_FOLDER_ROOT 0
#define VYSION_FOLDER_DESKTOP 1
#define VYSION_FOLDER_PROGRAMS 2
#define VYSION_FOLDER_APPVARS 3
/*versions:
-VYSION_VERSION_CORE
-VYSION_VERSION_BASIC
-VYSION_VERSION_ULTIMATE
*/
//distribution 
#define VYSION_DISTRIBUTION 1

#if (VYSION_DISTRIBUTION == VYSION_DISTRIBUTION_CORE)
#define VYSION_DISTRIBUTION_NAME "Core"
#elif (VYSION_DISTRIBUTION == VYSION_DISTRIBUTION_BASIC) 
#define VYSION_DISTRIBUTION_NAME "Basic"
#else
#define VYSION_DISTRIBUTION_NAME "Ultimate"
#endif


//icons
gfx_sprite_t *vysion_alticon[] = {basic, basic, ices, ice, cprog, ez80, appvar, unknown};


//structs
/*types:
0: unknown
1: TI-BASIC
2: ICE source
3: ICE
4: C
5: ASM
6: appvar
7: VYSION module (ASM program)
8: VYSION photo appvar
9: VYSION text appvar*/
//this system means that you can have copies of a file without really having copies of it too (two identical programs that exist in different
//locations)
struct vysion_file_t {
    char name[9];
    char *description;
    uint8_t type;
    //basically this is just the index of the folder in the folder array that contains it
    int location;
    gfx_sprite_t *icon;
    int size;
    //maybe later?
    uint8_t user;
    bool archived;
    bool locked;
    //for when we start the program and see if these files exist anymore
    bool indexed;
    bool taskbarpinned;
    bool startpinned;
};
extern struct vysion_file_t *vysion_file;

/*we always have to initialize:
0: root (/)
1: desktop
...maybe more later?*/
//-1 is where root is located
struct vysion_folder_t {
    char name[9];
    gfx_sprite_t *icon;
    //so we can have nested folders
    int location;
    uint8_t user;
    bool locked;
    //so that these can actually be sorted
    int index;
};
extern struct vysion_folder_t *vysion_folder;

struct vysion_filesysteminfo_t {
    int version;
    int numfiles;
    int numfolders;
    int numtempfiles;
};
extern struct vysion_filesysteminfo_t vysion_filesysteminfo;

//to be used for menus and that sort of thing
struct vysion_tempfile_t {
    bool isfolder;
    int index;
};
extern struct vysion_tempfile_t *vysion_tempfile;

struct vysion_programdata_t { 
    char str1[2000];
    bool clipboardisfolder;
    int clipboard;
    int currfolder;
    bool wallpapervalid;
    gfx_sprite_t *wallpaper;
    gfx_sprite_t **icon;
    //loop (for updating the battery)
    long loops;
    uint8_t batterystatus;
    //menu stuff
    uint8_t taskbar;
    uint8_t desktop;
    int taskbarselection;
    int desktopselection;
    int fileexplorerselection;
    int fileexplorermenumin;
    int currmenu;
    bool startreturn;
    uint8_t startreturnvalue;
    bool returnedfromprogram;
    //for the icons
    gfx_sprite_t **taskbarspr;
};
extern struct vysion_programdata_t vysion_programdata;

struct vysion_settings_t {
    int version;
    bool cursoron;
    bool displaylockscreen;
    bool requirepassword;
    bool showdesktopfiles;
    bool showdesktopfolders;
    bool showdesktopwallpaper;
    //0- none, 1- new files, 2- old files, 3- new files
    uint8_t filedetection;
    char wallpaperappvar[9];
    //sorry Mateo
    char password[16];
    bool centeredtaskbar;
    bool transparenttaskbar;
    //colors
    uint8_t taskbarcolor;
    uint8_t taskbarbordercolor;
    uint8_t taskbartextcolor;
    //struct optix_guicolors_t guicolors;
};
extern struct vysion_settings_t vysion_settings;

struct vysion_appvarreader_t {
    char name[strlen(VYSION_WALLPAPER_STRING)];
    uint8_t spr[160 * 120 + 2];
};
