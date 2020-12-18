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

#include "gfx/gfx.h"
#include "vysion_main.h"
#include "vysion_fileops.h"
#include "vysion.h"
#include "optix/optix.h"

void vysion_InitializeGraphics(void) {
    gfx_Begin();
    gfx_SetDraw(1);
    optix_SetDefaultColors();
    gfx_FillScreen(optix_guicolors.bgcolor);
}

void vysion_InitializeShell(void) {
    vysion_programdata.batterystatus = boot_GetBatteryStatus();
    vysion_programdata.loops = 0;
    vysion_programdata.wallpaper = gfx_MallocSprite(160, 120);
    ClearFixStopHook();
    optix_cursor.x = 160;
    optix_cursor.y = 120;
    //hook_Install(0x000400, &StartHook, StartHook_size, HOOK_TYPE_RAW_KEY, 10, "Start hook");
    //hook_Sync();
    //vysion_filesysteminfo.numfiles = 0;
    //let's see if we can edit
    //vysion_SetOP1(0x06, "VYSION");
    //delay(2000);
    //EditPrgm("VYSTEMP", 0);
    vysion_InitializeSettings();
    vysion_LoadData();
    optix_guisettings.cursoron = vysion_settings.cursoron;
    gfx_FillScreen(optix_guicolors.bgcolor);
    vysion_SetWallpaper(vysion_settings.wallpaperappvar);
    if (vysion_programdata.wallpaper != NULL && vysion_programdata.wallpapervalid && vysion_settings.showdesktopwallpaper) gfx_ScaledSprite_NoClip(vysion_programdata.wallpaper, 0, 0, 2, 2);
    optix_Dialogue("Loading...", "Detecting files and restoring filesystem...", 10, 100, 4);
    gfx_SwapDraw();
    vysion_DetectAllFiles(false);
    //make sure this still works
    vysion_GetAsmIcons();
    vysion_GetBasicIcons();
    vysion_SortFiles();
    vysion_LockScreen();
    //vysion_SortFolders();
    vysion_programdata.clipboard = -1;
    vysion_settings.version = VYSION_SETTINGS_VERSION;
    vysion_filesysteminfo.version = VYSION_FILESYSTEM_VERSION;
}

void vysion_End(void) {
    optix_DeleteAllMenus();
    vysion_SaveFilesystem();
    vysion_SaveData();
    gfx_End();
    free(vysion_programdata.wallpaper);
    //InstallStartHook();
}


void vysion_Desktop(void) {
    int i;
    struct vysion_file_t *currfile;
    gfx_sprite_t **icon;
    int file;
    bool running;
    int currmenu[] = {0, 1};
    running = true;
    icon = NULL;
    //while (kb_AnyKey()) kb_Scan();
    file = 0;
    vysion_GetTaskbar();
    optix_AddMenu(43, 220, 0, 0, 8 - vysion_settings.centeredtaskbar, 1, 24, 20, vysion_programdata.str1, vysion_programdata.taskbarspr);
    vysion_programdata.taskbar = optix_guidata.nummenus - 1;
    optix_menu[vysion_programdata.taskbar].displaytext = false;
    //center it because why not
    if (vysion_settings.centeredtaskbar) vysion_CenterTaskbar();
    vysion_SetTempMenu(1, -1, vysion_settings.showdesktopfiles, vysion_settings.showdesktopfolders);
    vysion_GetTempMenuText();
    vysion_GetTempMenuIcons();
    optix_AddMenu(0, 0, 0, 0, 4, 4, 80, 50, vysion_programdata.str1, vysion_programdata.icon);
    vysion_programdata.desktop = optix_guidata.nummenus - 1;
    optix_SetMenuSprScale(vysion_programdata.desktop, 2, 2);
    optix_guidata.currmenu = vysion_programdata.desktop;
    optix_SetAdvancedMenuCentering(vysion_programdata.desktop, 1, 1, 1, 0, 0, 18, 0, 4);
    //get the saved state stuff
    optix_menu[vysion_programdata.desktop].currselection = vysion_programdata.desktopselection;
    optix_menu[vysion_programdata.taskbar].currselection = vysion_programdata.taskbarselection;
    //if (vysion_programdata.currmenu < 2) optix_guidata.currmenu = vysion_programdata.currmenu;
    //because it won't get displayed otherwise, when returning from a saved state
    vysion_RenderDesktop();
    optix_RenderMenu(vysion_programdata.desktop);
    gfx_Blit(1);
    optix_SetActiveMenus(currmenu, 2);
    //timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_UP;
    while (!(kb_Data[6] & kb_Clear) && running) {
        struct optix_menu_t *m = &optix_menu[optix_guidata.currmenu];
        currfile = &vysion_file[vysion_tempfile[m->currselection].index];
        kb_Scan();
        //here you go KingDubDub
        if (kb_Data[4] & kb_Prgm) {
            vysion_programdata.startreturn = true;
            vysion_programdata.startreturnvalue = 0;
        }
        if (kb_Data[3] & kb_Apps) {
            vysion_programdata.startreturn = true;
            vysion_programdata.startreturnvalue = 1;
        }
        if (kb_Data[1] & kb_Yequ || vysion_programdata.startreturn) {
            //WHY DOESN'T THIS WORK!?!?!?!?!
            //this whole operation was a fucking stupid idea
            //"Wow I wonder why no one ever finishes shells?"
            //BECAUSE IT'S FUCKING AWFUL TO DO
            //so if yequ was pressed
            if (!vysion_programdata.startreturn) vysion_programdata.startreturnvalue = vysion_StartMenu();
            else vysion_programdata.startreturn = false;
            /*gfx_FillScreen(255);
            optix_BlaText();
            gfx_SetTextXY(5, 5);
            gfx_PrintUInt(vysion_programdata.startreturnvalue, 3);
            gfx_SwapDraw();
            delay(1000);*/
            switch (vysion_programdata.startreturnvalue) {
                //programs
                case 0:
                    vysion_FileExplorer(2);
                    break;
                //appvars
                case 1:
                    vysion_FileExplorer(3);
                    break;
                case 2:
                    vysion_FileExplorer(vysion_programdata.currfolder);
                    break;
                case 4:
                    vysion_SettingsMenu();
                    //vysion_SelectWallpaper();
                    break;
                case 5:
                    optix_Dialogue("Loading...", "Detecting files and restoring filesystem...", 10, 100, 4);
                    gfx_SwapDraw();
                    vysion_DetectAllFiles(true);
                    break;
                case 7:
                    running = false;
                    break;
            }
            //refresh the taskbar
            optix_DeleteLastMenu();
            optix_DeleteLastMenu();
            vysion_GetTaskbar();
            optix_AddMenu(43, 220, 0, 0, 8 - vysion_settings.centeredtaskbar, 1, 24, 20, vysion_programdata.str1, vysion_programdata.taskbarspr);
            //optix_SetMenuSprScale(vysion_programdata.taskbar, 2, 2);
            vysion_programdata.taskbar = optix_guidata.nummenus - 1;
            //optix_guidata.currmenu = vysion_programdata.taskbar;
            optix_menu[vysion_programdata.taskbar].displaytext = false;
            if (vysion_settings.centeredtaskbar) vysion_CenterTaskbar();
            vysion_SetTempMenu(1, -1, vysion_settings.showdesktopfiles, vysion_settings.showdesktopfolders);
            vysion_GetTempMenuText();
            vysion_GetTempMenuIcons();
            optix_AddMenu(0, 0, 0, 0, 4, 4, 80, 50, vysion_programdata.str1, vysion_programdata.icon);
            vysion_programdata.desktop = optix_guidata.nummenus - 1;
            optix_SetMenuSprScale(vysion_programdata.desktop, 2, 2);
            optix_guidata.currmenu = vysion_programdata.desktop;
            optix_SetAdvancedMenuCentering(vysion_programdata.desktop, 1, 1, 1, 0, 0, 18, 0, 4);
            //fix this
            m = &optix_menu[optix_guidata.currmenu];
            m->enterpressed = false;
            while (kb_AnyKey()) kb_Scan();
            optix_SetActiveMenus(currmenu, 2);
        }
        if (kb_Data[1] & kb_Window) {
            optix_guidata.currmenu = vysion_programdata.taskbar;
            vysion_GetTaskbar();
        }
        if ((kb_Data[7] & kb_Up) && vysion_programdata.taskbar == optix_guidata.currmenu) {
            optix_guidata.currmenu = vysion_programdata.desktop;
            vysion_SetTempMenu(1, -1, true, true);
        }
        //an easter egg
        if (kb_Data[3] & kb_Sin && kb_Data[6] & kb_Add && kb_Data[4] & kb_5) {
            gfx_SetDraw(0);
            while (!os_GetCSC()) {
                gfx_SetColor(randInt(0, 255));
                gfx_FillRectangle(randInt(0, 320), randInt(0, 240), 2, 2);
            }
            gfx_SetDraw(1);
            optix_Message("SUCCESS", "You found an Easter egg! See if you can find some more.", 12, 120, 4);
        }


        if ((optix_guidata.currmenu != OPTIX_MENU_INVALID && m->enterpressed && !(kb_Data[6] & kb_Enter))) {
            //if (optix_guidata.currmenu == vysion_programdata.taskbar) 
            //yeah this could cause some issues
            if (vysion_tempfile[m->currselection].isfolder) vysion_FileExplorer(vysion_tempfile[m->currselection].index);
            else if (optix_menu[optix_guidata.currmenu].numoptions > 0) {
                vysion_programdata.currmenu = optix_guidata.currmenu;
                vysion_RunProgram(currfile->name, currfile->type);
            }
            m->enterpressed = false;
        }
        optix_UpdateCurrMenu();
        vysion_programdata.desktopselection = optix_menu[vysion_programdata.desktop].currselection;
        vysion_programdata.taskbarselection = optix_menu[vysion_programdata.taskbar].currselection;
        vysion_RenderDesktop();
        optix_RenderMenu(vysion_programdata.desktop);
        //gfx_SetTextXY(5, 5);
        //gfx_PrintUInt(32768 / timer_1_Counter, 3);
        //timer_1_Counter = 0;
        gfx_SwapDraw();
        optix_HandleGUI();
    }
}