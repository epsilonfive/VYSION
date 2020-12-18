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

//#include "gfx/gfx.h"
#include "vysion_saving.h"
#include "vysion.h"
#include "optix/optix.h"

//LABEL
//SAVING AND LOADING
void vysion_SaveFilesystem(void) {
    ti_var_t slot;
    ti_CloseAll();
    if (slot = ti_Open(VYSION_FILESYSTEM_APPVAR, "w+")) {
        //this contains information about the number of folders and files
        vysion_filesysteminfo.numtempfiles = 0;
        //vysion_filesysteminfo.numfiles--;
        ti_Write(&vysion_filesysteminfo, sizeof(struct vysion_filesysteminfo_t), 1, slot);
        ti_Write(vysion_file, vysion_filesysteminfo.numfiles * sizeof(struct vysion_file_t), 1, slot);
        ti_Write(vysion_folder, vysion_filesysteminfo.numfolders * sizeof(struct vysion_folder_t), 1, slot);
        ti_SetArchiveStatus(true, slot);
    } else {
        //do something else
        optix_Message("ERROR", "Couldn't open the filesystem appvar.", 12, 150, 8);
    }
    ti_CloseAll();
}

bool vysion_LoadFilesystem(void) {
    ti_var_t slot;
    ti_var_t slotb;
    int i;
    bool loaded;
    bool hookfound;
    ti_CloseAll();
    if (slot = ti_Open(VYSION_FILESYSTEM_APPVAR, "r")) {
        //this contains information about the number of folders and files
        ti_Read(&vysion_filesysteminfo, sizeof(struct vysion_filesysteminfo_t), 1, slot);
        if (vysion_filesysteminfo.version != VYSION_FILESYSTEM_VERSION) {
            gfx_Blit(0);
            optix_Message("WARNING", "Filesystem appvar version and program filesystem version mismatch. Please reset the filesystem"
            " if you encounter any issues.", 12, 150, 8);
        }
        //vysion_filesysteminfo.numfiles--;
        //make sure we have enough memory allocated
        vysion_file = malloc(vysion_filesysteminfo.numfiles * sizeof(struct vysion_file_t));
        vysion_folder = malloc(vysion_filesysteminfo.numfolders * sizeof(struct vysion_folder_t));
        ti_Read(vysion_file, vysion_filesysteminfo.numfiles * sizeof(struct vysion_file_t), 1, slot);
        ti_Read(vysion_folder, vysion_filesysteminfo.numfolders * sizeof(struct vysion_folder_t), 1, slot);
        loaded = true;
        vysion_filesysteminfo.numtempfiles = 0;
    } else {
        //do something else (maybe initialize)
        /*hook_IsInstalled(0x000400, &hookfound);
        if (!hookfound) {
            slotb = ti_Open("VYSHSTRT", "r");
            gfx_FillScreen(224);
            gfx_SwapDraw();
            hook_Install(0x000400, ti_GetDataPtr(slotb), ti_GetSize(slotb), HOOK_TYPE_RAW_KEY, 10, "Start hook");
            hook_Sync();
        } else {
            gfx_FillScreen(18);
            gfx_SwapDraw();
            hook_Disable(0x000400);
            hook_Uninstall(0x000400);
            hook_Sync();
        }*/
        //maybe uninstall
        vysion_filesysteminfo.numfiles = vysion_filesysteminfo.numfolders = vysion_filesysteminfo.numtempfiles = vysion_programdata.currmenu = 
        vysion_programdata.taskbarselection = vysion_programdata.desktopselection = 0;
        vysion_AddFolder("Root", -1);
        vysion_AddFolder("Desktop", 0);
        vysion_AddFolder("Programs", 0);
        vysion_AddFolder("Appvars", 0);
        loaded = false;
        gfx_Blit(0);
        optix_Message("Welcome!", "Welcome to VYSION, a new shell for the CE! The filesystem has been initialized.", 12, 150, 8);
    }
    ti_CloseAll();
    //make sure this works (the check to see if everything still exists)
    for (i = 0; i < vysion_filesysteminfo.numfiles; i++) vysion_file[i].indexed = false;
    return loaded;
}

void vysion_SaveData(void) {
    ti_var_t slot;
    //clean stuff up
    free(vysion_programdata.taskbarspr);
    free(vysion_programdata.icon);
    ti_CloseAll();
    if (slot = ti_Open(VYSION_DATA_APPVAR, "w+")) {
        ti_Write(&vysion_settings, sizeof(vysion_settings), 1, slot);
        ti_Write(&optix_guicolors, sizeof(optix_guicolors), 1, slot);
        ti_SetArchiveStatus(true, slot);
    } else optix_Message("ERROR", "Couldn't open the data appvar.", 12, 150, 8);
    ti_CloseAll();
    if (slot = ti_Open(VYSION_STATE_APPVAR, "w+")) {
        struct vysion_programdata_t *p = &vysion_programdata;
        ti_Write(&p->startreturn, sizeof(p->startreturn), 1, slot);
        ti_Write(&p->startreturnvalue, sizeof(p->startreturnvalue), 1, slot);
        ti_Write(&p->desktopselection, sizeof(p->desktopselection), 1, slot);
        ti_Write(&p->taskbarselection, sizeof(p->taskbarselection), 1, slot);
        ti_Write(&p->currfolder, sizeof(p->currfolder), 1, slot);
        ti_Write(&p->fileexplorerselection, sizeof(p->fileexplorerselection), 1, slot);
        ti_Write(&p->fileexplorermenumin, sizeof(p->fileexplorermenumin), 1, slot);
        ti_Write(&optix_cursor.x, sizeof(optix_cursor.x), 1, slot);
        ti_Write(&optix_cursor.y, sizeof(optix_cursor.y), 1, slot);
    } else optix_Message("ERROR", "Couldn't open the state appvar.", 12, 150, 8);

}

void vysion_LoadData(void) {
    ti_var_t slot;
    ti_CloseAll();
    if (slot = ti_Open(VYSION_DATA_APPVAR, "r")) {
        ti_Read(&vysion_settings, sizeof(vysion_settings), 1, slot);
        ti_Read(&optix_guicolors, sizeof(optix_guicolors), 1, slot);
        if (vysion_settings.version != VYSION_SETTINGS_VERSION) {
            gfx_Blit(0);
            optix_Message("WARNING", "Settings appvar version and program settings version mismatch. Please reset the settings"
            " if you encounter any issues.", 12, 150, 8);
        }
    }
    ti_CloseAll();
    if (slot = ti_Open(VYSION_STATE_APPVAR, "r")) {
        struct vysion_programdata_t *p = &vysion_programdata;
        ti_Read(&p->startreturn, sizeof(p->startreturn), 1, slot);
        ti_Read(&p->startreturnvalue, sizeof(p->startreturnvalue), 1, slot);
        ti_Read(&p->desktopselection, sizeof(p->desktopselection), 1, slot);
        ti_Read(&p->taskbarselection, sizeof(p->taskbarselection), 1, slot);
        ti_Read(&p->currfolder, sizeof(p->currfolder), 1, slot);
        ti_Read(&p->fileexplorerselection, sizeof(p->fileexplorerselection), 1, slot);
        ti_Read(&p->fileexplorermenumin, sizeof(p->fileexplorermenumin), 1, slot);
        ti_Read(&optix_cursor.x, sizeof(optix_cursor.x), 1, slot);
        ti_Read(&optix_cursor.y, sizeof(optix_cursor.y), 1, slot);
        //this stuff shouldn't be saved
        //maybe the program is smart enough?
        ti_CloseAll();
        ti_Delete(VYSION_STATE_APPVAR);
    }
}

void vysion_InitializeSettings(void) {
    struct vysion_settings_t *v = &vysion_settings;
    strcpy(v->wallpaperappvar, "DEFAULT");
    strcpy(v->password, "");
    v->transparenttaskbar = v->showdesktopwallpaper = v->showdesktopfiles = v->showdesktopfolders = true;
    v->centeredtaskbar = v->transparenttaskbar = v->cursoron = v->displaylockscreen = v->requirepassword = false;
    v->taskbarcolor = 0;
    v->taskbarbordercolor = 255;
    v->taskbartextcolor = 255;
    //detect all files
    vysion_settings.filedetection = 3;
    optix_SetDefaultColors();
}