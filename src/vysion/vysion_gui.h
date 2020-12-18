#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

//defines
//for different menus there
#define FILE_OPERATIONS_FOLDER 0
#define FILE_OPERATIONS_TI_BASIC_PRGM 1
#define FILE_OPERATIONS_ASM_PRGM 2
#define FILE_OPERATIONS_APPVAR 3
#define FILE_OPERATIONS_NOFILE 4
//for operations
#define FILE_OPERATIONS_RUN 0
#define FILE_OPERATIONS_CUT 1
#define FILE_OPERATIONS_PASTE 2
#define FILE_OPERATIONS_EDIT 3
#define FILE_OPERATIONS_DELETE 4
#define FILE_OPERATIONS_NEW_FOLDER 5
#define FILE_OPERATIONS_PROPERTIES 6




//functions
//toolbar & GUI stuff
void vysion_RenderToolbar(void);
void vysion_CenterTaskbar(void);
void vysion_FileExplorer(int folder);
void vysion_SettingsMenu(void);
uint8_t vysion_FileOperationsMenu(uint8_t filetype);
uint8_t vysion_StartMenu(void);
void vysion_PropertiesMenu(int file);
void vysion_RenderDesktop(void);
void vysion_SetWallpaper(char name[9]);
void vysion_SelectWallpaper(void);
void vysion_CheckStartMenu(void);
void vysion_CheckAlphaJump(void);
void vysion_LockScreen(void);