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
#include "vysion_fileops.h"
#include "vysion.h"
#include "optix/optix.h"

//Til I collapse I'm spillin these raps long as you feel em
//Til the day that I drop you'll never say that I'm not killin em
//cause when I am not, then I'ma stop pennin em

//LABEL
//FILE AND FOLDER OPERATIONS
void vysion_DeleteFolder(int foldertodelete) {
    int i;
    if (vysion_filesysteminfo.numfolders > 0) {
        vysion_folder[foldertodelete] = vysion_folder[vysion_filesysteminfo.numfolders - 1];
        vysion_folder = realloc(vysion_folder, --vysion_filesysteminfo.numfolders * sizeof(struct vysion_folder_t));
    }
    //fix this, delete all files in the folder (maybe change later) 
    for (i = 0; i < vysion_filesysteminfo.numfiles; i++) {
        struct vysion_file_t *currfile = &vysion_file[i];
        if (currfile->location == foldertodelete) {
            vysion_Delete(false, i);
            continue;
        }
        //the last folder is now this folder
        if (currfile->location == vysion_filesysteminfo.numfolders) currfile->location = foldertodelete;
    }
    for (i = 0; i < vysion_filesysteminfo.numfolders; i++) {
        struct vysion_folder_t *currfolder = &vysion_folder[i];
        if (currfolder->location == foldertodelete) {
            //recursion is cool
            vysion_Delete(true, i);
            continue;
        }
        //the last folder is now this folder
        if (currfolder->location == vysion_filesysteminfo.numfolders) currfolder->location = foldertodelete;
    }
}


void vysion_DeleteFile(int filetodelete) {
    if (vysion_filesysteminfo.numfiles > 0) {
        //overwrite the element to be deleted with the last element and resize the array
        vysion_file[filetodelete] = vysion_file[vysion_filesysteminfo.numfiles - 1];
        vysion_file = realloc(vysion_file, --vysion_filesysteminfo.numfiles * sizeof(struct vysion_file_t));
    }
}

void vysion_AddFile(void) {
    struct vysion_file_t *currfile;
    vysion_file = realloc(vysion_file, ++vysion_filesysteminfo.numfiles * sizeof(struct vysion_file_t));
    //this is getting broken so fixf
    currfile = &vysion_file[vysion_filesysteminfo.numfiles - 1];
    strcpy(currfile->name, "NULL");
    strcpy(currfile->description, "NULL");
    currfile->location = 1;
}

void vysion_AddFolder(char name[9], int location) {
    vysion_folder = realloc(vysion_folder, ++vysion_filesysteminfo.numfolders * sizeof(struct vysion_folder_t));
    vysion_folder[vysion_filesysteminfo.numfolders - 1].location = location;
    //this way the folders can be sorted correctly, since the index will now be stored in the folder rather than be dependent on its index in the dynamic array
    vysion_folder[vysion_filesysteminfo.numfolders - 1].index = vysion_filesysteminfo.numfolders - 1;
    strcpy(vysion_folder[vysion_filesysteminfo.numfolders - 1].name, name);
}

//for finding a folder with a given index, returns the index in the dynamic array or -1 on failure
int vysion_GetFolderByIndex(int index) {
    int i;
    for (i = 0; i < vysion_filesysteminfo.numfolders; i++) if (vysion_folder[i].index == index) return i;
    return -1;
}

void vysion_AddFileToTempMenu(bool isfolder, int index) {
    vysion_tempfile = realloc(vysion_tempfile, ++vysion_filesysteminfo.numtempfiles * sizeof(struct vysion_tempfile_t));
    vysion_tempfile[vysion_filesysteminfo.numtempfiles - 1].isfolder = isfolder;
    vysion_tempfile[vysion_filesysteminfo.numtempfiles - 1].index = index;
}

void vysion_ClearTempMenu(void) {
    vysion_filesysteminfo.numtempfiles = 0;
    vysion_tempfile = realloc(vysion_tempfile, 0);
}

//set either to -1 to include all folders/types
//this will set the menu to the files that are pinned to the taskbar 
void vysion_GetTaskbar(void) {
    int i;
    int filecount = 0;
    vysion_ClearTempMenu();
    for (i = 0; i < vysion_filesysteminfo.numfiles; i++) {
        struct vysion_file_t *file = &vysion_file[i];
        //why did I do this
        if (file->taskbarpinned) vysion_AddFileToTempMenu(false, i);
    }
    //this part breaks so it needs a separate array
    for (i = 0; i < vysion_filesysteminfo.numtempfiles; i++) {
        //update the icon
        //get the icon
        filecount++;
        vysion_programdata.taskbarspr = realloc(vysion_programdata.taskbarspr, 3 * filecount);
        /*gfx_FillScreen(255);
        optix_BlaText();
        gfx_SetTextXY(5, 5);
        gfx_PrintUInt(vysion_tempfile[i].index, 3);
        gfx_SetTextXY(5, 15);
        gfx_PrintUInt(vysion_filesysteminfo.numtempfiles, 3);
        gfx_SwapDraw();
        delay(1000);*/
        vysion_programdata.taskbarspr[filecount - 1] = vysion_file[vysion_tempfile[i].index].icon;
    }
    vysion_GetTempMenuText();
}

//same but for the start menu
void vysion_GetStartMenu(void) {
    int i = 0;
    vysion_ClearTempMenu();
    for (i = 0; i < vysion_filesysteminfo.numfiles; i++) {
        struct vysion_file_t *file = &vysion_file[i];
        if (file->startpinned) vysion_AddFileToTempMenu(false, i);
    }
    vysion_GetTempMenuIcons();
    vysion_GetTempMenuText();
}

//folder should be the index of the folder in the folder array that is desired, as before (just fixed now to accomodate for sorting them)
//use vysion_GetFolderByIndex() to do this if you need to do something like open a specific location
void vysion_SetTempMenu(int folder, int type, bool showfiles, bool showfolders) {
    int i = 0;
    //fix this (independent of array placement)
    if (folder != -1) folder = vysion_folder[folder].index;
    vysion_ClearTempMenu();
    //get the folders
    if (showfolders) {
        for (i = 0; i < vysion_filesysteminfo.numfolders; i++) {
            struct vysion_folder_t *fold = &vysion_folder[i];
            if (fold->location == folder || (folder == -1 && fold->location != -1)) vysion_AddFileToTempMenu(true, i);
        }
    }
    if (showfiles) {
        for (i = 0; i < vysion_filesysteminfo.numfiles; i++) {
            struct vysion_file_t *file = &vysion_file[i];
            if ((file->location == folder || folder == -1) && (file->type == type || type == -1) && (strlen(file->name) > 0)) vysion_AddFileToTempMenu(false, i);
        }
    }
}

void vysion_GetTempMenuIcons(void) {
    int i;
    int file;
    file = 0;
    for (i = 0; i < vysion_filesysteminfo.numtempfiles; i++) {
        //update the icon
        //get the icon
        file++;
        vysion_programdata.icon = realloc(vysion_programdata.icon, 3 * file);
        if (!vysion_tempfile[i].isfolder) vysion_programdata.icon[file - 1] = vysion_file[vysion_tempfile[i].index].icon;
        else vysion_programdata.icon[file - 1] = foldericon;
    }
}

void vysion_GetTempMenuText(void) {
    int i;
    strcpy(vysion_programdata.str1, "");
    for (i = 0; i < vysion_filesysteminfo.numtempfiles; i++) {
        if (vysion_tempfile[i].isfolder) strcat(vysion_programdata.str1, vysion_folder[vysion_tempfile[i].index].name);
        else strcat(vysion_programdata.str1, vysion_file[vysion_tempfile[i].index].name);
        strcat(vysion_programdata.str1, "`");
    }
}

void vysion_SetOP1(uint8_t type, char name[]) {
    //does this work?
    char temp[10] = " ";
    temp[0] = type;
    strcat(temp, name);
    strcpy(os_OP1, temp);
}

//to be called on first running, basically refreshes the filesystem or creates a new one if it doesn't exist
void vysion_DetectAllFiles(bool fromstart) {
    char *var_name;
    uint8_t *search_pos = NULL;
    uint8_t type;
    int filesfound;
    ti_var_t slot;
    uint8_t datatest;
    struct vysion_file_t *currfile;
    uint8_t loops;
    char *defdescription = "No description...";
    bool loaded;
    bool foundfile;
    uint8_t vysiontype;
    int i;
    gfx_sprite_t *alticon[] = {basic, basic, ices, ice, cprog, ez80, appvar, unknown};
    ti_CloseAll();
    filesfound = 0;
    datatest = 0;
    loaded = vysion_LoadFilesystem();
    //this could really screw up the whole system
    for (int i = 0; i < vysion_filesysteminfo.numfiles; i++) vysion_file[i].icon = NULL;
    if (loaded && vysion_settings.filedetection == 0 && !fromstart) return;
    while ((var_name = ti_DetectAny(&search_pos, NULL, &type)) != NULL) {
        //gfx_ZeroScreen();
        if (type == TI_PRGM_TYPE || type == TI_PPRGM_TYPE || type == TI_APPVAR_TYPE) {
            if (*var_name != '!' && *var_name != '#') {
                ti_Close(slot);
                if (slot = ti_OpenVar(var_name, "r", type)) {
                    //actually do stuff
                    //currfile->type = 5;
                    if (loaded) {
                        foundfile = false;
                        //this is going to be expensive but iterate through and see if it already exists (so we can preserve its location)
                        for (i = 0; i < vysion_filesysteminfo.numfiles; i++) {
                            //it's old, cool
                            if (!vysion_file[i].indexed) {
                                //test to see if it's the same basic type (asm program, basic program, or appvar)
                                //if it's ICE source or TI-BASIC
                                vysiontype = 0;
                                vysiontype = vysion_GetFileType(vysion_file[i].type);
                                //well I'm a moron
                                //anyway should work now
                                if ((strncmp(vysion_file[i].name, var_name, 8) == 0) && (type == vysiontype)) {
                                    currfile = &vysion_file[i];
                                    currfile->indexed = true;
                                    foundfile = true;
                                    break;
                                }
                            }  
                        }
                        //it's a new file
                        //I think this is what's giving us our issue?
                        if (!foundfile && (vysion_settings.filedetection != 2 || fromstart)) {
                            vysion_AddFile();
                            currfile = &vysion_file[vysion_filesysteminfo.numfiles - 1];
                            strcpy(currfile->name, var_name);
                            currfile->indexed = true;
                            switch (type) {
                                case (TI_PRGM_TYPE):
                                    currfile->type = VYSION_BASIC_TYPE;
                                    currfile->location = 2;
                                    break;
                                case (TI_PPRGM_TYPE):
                                    currfile->type = VYSION_ASM_TYPE;
                                    currfile->location = 2;
                                    break;
                                case (TI_APPVAR_TYPE):
                                    currfile->type = VYSION_APPVAR_TYPE;
                                    currfile->location = 3;
                                    break;
                                default:
                                    currfile->type = VYSION_UNKNOWN_TYPE;
                                    currfile->location = 1;
                                    break;
                            }
                            currfile->taskbarpinned = false;
                            currfile->locked = false;
                        }
                        //if we get to the end it's a new file and should be given a new entry
                    } else {
                        vysion_AddFile();
                        currfile = &vysion_file[vysion_filesysteminfo.numfiles - 1];
                        strcpy(currfile->name, var_name);
                        switch (type) {
                            case (TI_PRGM_TYPE):
                                currfile->type = VYSION_BASIC_TYPE;
                                currfile->location = 2;
                                break;
                            case (TI_PPRGM_TYPE):
                                //refine this later
                                currfile->type = VYSION_ASM_TYPE;
                                currfile->location = 2;
                                break;
                            case (TI_APPVAR_TYPE):
                                currfile->type = VYSION_APPVAR_TYPE;
                                currfile->location = 3;
                                break;
                            default:
                                currfile->type = VYSION_UNKNOWN_TYPE;
                                currfile->location = 1;
                                break;
                        }
                        currfile->taskbarpinned = false;
                        currfile->locked = false;
                    }
                    //not sure why this is here, we don't need it necessarily
                    currfile->description = defdescription;
                    //get the sprite pointer and display it
                    //get the type (maybe add more later)
                    //type = 5;
                    //will be changed if it's ICE or C
                    currfile->size = ti_GetSize(slot);
                    currfile->archived = ti_IsArchived(slot);
                    currfile->icon = NULL;
                    //for the love of God remove this later
                    //currfile->location = 1;
                    filesfound++;
                }
            }
            //check to see if it exists in the vysion_filesystem
            //try to open it, get its icon, type, etc.
        }
    }
    if (loaded && (vysion_settings.filedetection != 1 || fromstart)) {
        //go through and clear anything that doesn't exist anymore
        for (i = 0; i < vysion_filesysteminfo.numfiles; i++) if (!vysion_file[i].indexed) vysion_DeleteFile(i);
    }
    //this won't be done otherwise
    if (fromstart) {
        vysion_GetAsmIcons();
        vysion_GetBasicIcons();
    }
}

void vysion_GetAsmIcons(void) {
    int loops;
    int i;
    uint8_t datatest;
    ti_var_t slot;
    for (i = 0; i < vysion_filesysteminfo.numfiles; i++) {
        struct vysion_file_t *currfile = &vysion_file[i];
        ti_CloseAll();
        slot = ti_OpenVar(currfile->name, "r", vysion_GetFileType(currfile->type));
        if (vysion_GetFileType(currfile->type) == TI_PPRGM_TYPE) {
            for (loops = 0; loops < 20; loops++) {
                ti_Read(&datatest, 1, 1, slot);
                //datatest++;
                //gfx_PrintStringXY(var_name, 5, 230);
                //gfx_SetTextXY(5 + (loops / 20) * 30, 5 + 10 * (loops - (loops / 20 * 20)));
                //gfx_PrintUInt(datatest, 3);
                //gfx_SetTextXY(45, 5 + 10 * loops);
                //gfx_PrintUInt((int) datatest - (int) ti_GetDataPtr(slot), 3);
                //delay(10);
                //there's some way to get the type, will add that here later
                //fix this (check if it's really a protected basic program)
                if (loops == 0) {
                    if ((datatest != 0xEF) && *((uint8_t *) (ti_GetDataPtr(slot) + 1)) != 0x7B) {
                        currfile->type = VYSION_PBASIC_TYPE;
                        currfile->locked = true;
                        break;
                    }
                }
                if (loops == 2) {
                    switch (datatest) {
                        case 0x00:
                            //it's a C program
                            currfile->type = VYSION_C_TYPE;
                            break;
                        case 0x7F:
                            //it's an ICE program
                            currfile->type = VYSION_ICE_TYPE;
                            break;
                        default:
                            //it's an ez80 program
                            currfile->type = VYSION_ASM_TYPE;
                            break;
                    }
                }
                if (datatest == 16) {
                    ti_Read(&datatest, 1, 1, slot);
                    if (datatest == 16) {
                        currfile->icon = ti_GetDataPtr(slot) - 2;
                        currfile->description = (char *) currfile->icon + 258;
                        //if (strlen(currfile->description) > 40) currfile->description = NULL;
                    }
                }
            }
        }
        //if we couldn't get the icon, assign one
        if (currfile->icon == NULL) currfile->icon = vysion_alticon[currfile->type];
    }
}

void vysion_GetBasicIcons(void) {
    ti_var_t slot;
    int loops;
    int i;
    uint8_t palette[] = {223, 24, 224, 0, 248, 6, 228, 96, 16, 29, 231, 255, 222, 189, 148, 74};
    uint8_t datatest;
    char temp[2] = " ";
    //:DCS
    //"
    //or something
    uint8_t search[] = {0x3e, 0x44, 0x43, 0x53, 0x3f, 0x2a};
    for (i = 0; i < vysion_filesysteminfo.numfiles; i++) {
        ti_CloseAll();
        struct vysion_file_t *currfile = &vysion_file[i];
        if (currfile->type == VYSION_PBASIC_TYPE || currfile->type == VYSION_BASIC_TYPE) {
            if (slot = ti_OpenVar(currfile->name, "r", vysion_GetFileType(currfile->type))) {
                if (!memcmp(search, ti_GetDataPtr(slot), 6)) {
                    if (currfile->icon != NULL) currfile->icon = gfx_MallocSprite(16, 16);
                    ti_Seek(6, 0, slot);
                    for (loops = 0; loops < 256; loops++) {
                        ti_Read(temp, 1, 1, slot);
                        if (datatest != 0x2a) currfile->icon->data[loops] = palette[strtol(temp, NULL, 16)]; //convert the char into a number (0 -> 0x0, B -> 0xB, etc.)
                        else {
                            free(currfile->icon);
                            currfile->icon = NULL;
                        }
                    }
                }
            }
        }
        //just put this here
        if (currfile->type == VYSION_BASIC_TYPE) currfile->locked = false;
        if (currfile->icon == NULL) currfile->icon = vysion_alticon[currfile->type];
    }
}




int vysion_CompareFolderNames(void *a, void *b) {
    return strcmp(((struct vysion_folder_t *)a)->name, ((struct vysion_folder_t *)b)->name);
}

int vysion_CompareFileNames(void *a, void *b) {
    return strcmp(((struct vysion_file_t *)a)->name, ((struct vysion_file_t *)b)->name);
}

void vysion_SortFolders(void) {
    //make sure the first 4 don't get reordered, because that would be bad (Root, Desktop, Programs, Appvars)
    qsort(&(vysion_folder[VYSION_FOLDER_NUM_CONSTANTS]), vysion_filesysteminfo.numfolders - VYSION_FOLDER_NUM_CONSTANTS, sizeof(struct vysion_folder_t), vysion_CompareFolderNames);
}

void vysion_SortFiles(void) {
    qsort(vysion_file, vysion_filesysteminfo.numfiles, sizeof(struct vysion_file_t), vysion_CompareFileNames);
}

uint8_t vysion_GetFileType(uint8_t type) {
    if (type == VYSION_BASIC_TYPE) return TI_PRGM_TYPE;
    else if (type == VYSION_ASM_TYPE || type == VYSION_C_TYPE || type == VYSION_ICE_TYPE || type == VYSION_PBASIC_TYPE) return TI_PPRGM_TYPE;
    else if (type == VYSION_APPVAR_TYPE) return TI_APPVAR_TYPE;
    else return 255;
}

//returns true if filesystem should be refreshed
bool vysion_DoFileOperation(uint8_t operation, int main) {
    bool runrefresh = false;
    struct optix_menu_t *m = &optix_menu[optix_guidata.currmenu];
    struct vysion_tempfile_t *v = &vysion_tempfile[optix_menu[main].currselection];
    switch (operation) {
        //run
        case FILE_OPERATIONS_RUN:
            //clean things up
            vysion_programdata.startreturn = true;
            vysion_programdata.startreturnvalue = 2;
            vysion_programdata.fileexplorerselection = optix_menu[main].currselection;
            vysion_programdata.fileexplorermenumin = optix_menu[main].menumin;
            if (vysion_filesysteminfo.numtempfiles > 0) vysion_RunProgram(vysion_file[vysion_tempfile[optix_menu[main].currselection].index].name, vysion_file[vysion_tempfile[optix_menu[main].currselection].index].type);
            //if it runs correctly we won't get here
            vysion_programdata.startreturn = false;
            vysion_programdata.fileexplorerselection = 0;
            vysion_programdata.fileexplorermenumin = 0;
            break;
        //new folder
        case FILE_OPERATIONS_NEW_FOLDER:
            vysion_AddFolder(optix_GetStringInput("Name?", 10, 100, 8), vysion_folder[vysion_programdata.currfolder].index);
            while (kb_AnyKey()) kb_Scan();
            runrefresh = true;
            break;
        //cut
        case FILE_OPERATIONS_CUT:
            vysion_programdata.clipboardisfolder = vysion_tempfile[optix_menu[main].currselection].isfolder;
            vysion_programdata.clipboard = vysion_tempfile[optix_menu[main].currselection].index;
            runrefresh = true;
            break;
        case FILE_OPERATIONS_PASTE:
            if (vysion_programdata.clipboard != -1) {
                if (vysion_programdata.clipboardisfolder) {
                    if (vysion_programdata.clipboard != vysion_programdata.currfolder) vysion_folder[vysion_programdata.clipboard].location = vysion_programdata.currfolder;
                    else optix_Message("ERROR", "Pasting a folder within itself is not allowed.", 12, 120, 8);
                } else vysion_file[vysion_programdata.clipboard].location = vysion_programdata.currfolder;
                vysion_programdata.clipboard = -1;
                runrefresh = true;
            }
            break;
        //pin it to the taskbar
        case FILE_OPERATIONS_EDIT:
            //if it's basic
            if (vysion_file[vysion_tempfile[optix_menu[main].currselection].index].type == VYSION_BASIC_TYPE) {
                if (!vysion_file[vysion_tempfile[optix_menu[main].currselection].index].archived)
                    vysion_EditProgram(vysion_file[vysion_tempfile[optix_menu[main].currselection].index].name);
                else optix_Message("ERROR", "Please unarchive this file to edit it.", 12, 120, 8);
            }
            break;
        case FILE_OPERATIONS_DELETE:
            //fix this later
            //if (!vysion_tempfile[m->currselection].isfolder) currfile->taskbarpinned = true;
            if (!optix_Menu("Delete?", "Yes`No`", 12, 120, 3)) if (!vysion_Delete(v->isfolder, v->index)) optix_Message("ERROR", "Deletion failed...", 12, 120, 8);
            optix_guidata.currmenu = main;
            runrefresh = true;
            vysion_GetAsmIcons();
            vysion_GetBasicIcons();
            break;
        case FILE_OPERATIONS_PROPERTIES:
            if (!vysion_tempfile[m->currselection].isfolder && vysion_filesysteminfo.numtempfiles > 0) vysion_PropertiesMenu(vysion_tempfile[optix_menu[main].currselection].index);
            runrefresh = true;
            vysion_GetAsmIcons();
            vysion_GetBasicIcons();
            break;
        //is passed if need to quit
        case 255:
            break;
        default:
            break;
    }
    return runrefresh;
}

bool vysion_HandleFileOperations(uint8_t filetype) {
    return vysion_DoFileOperation(vysion_FileOperationsMenu(filetype), optix_guidata.currmenu);
}