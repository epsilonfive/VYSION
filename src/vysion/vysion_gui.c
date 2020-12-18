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
#include "vysion_gui.h"
#include "vysion.h"
#include "optix/optix.h"

//Til I collapse I'm spillin these raps long as you feel em
//Til the day that I drop you'll never say that I'm not killin em
//cause when I am not, then I'ma stop pennin em

void zx7_Decompress(void *dest, void *source);

//LABEL
//GUI OPERATIONS
void vysion_RenderToolbar(void) {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    char str1[15];
    uint8_t i;
    vysion_programdata.loops++;
    if (vysion_programdata.loops % 10000 == 0) vysion_programdata.batterystatus = boot_GetBatteryStatus();
    if (!vysion_settings.transparenttaskbar) {
        gfx_SetColor(vysion_settings.taskbarcolor);
        //gfx_SetColor(0);
        gfx_FillRectangle(0, 220, 320, 20);
        gfx_SetColor(vysion_settings.taskbarbordercolor);
        gfx_HorizLine(0, 219, 320);
        gfx_VertLine(5 + taskbaricon->width + 5, 220, 20);
    }
    gfx_SetTransparentColor(0);
    gfx_TransparentSprite(taskbaricon, 5, 222);
    /*
    gfx_SetColor(optix_guicolors.selectcolor);
    //gfx_SetColor(18);
    gfx_FillTriangle(5, 225, 35, 225, 20, 235);
    gfx_SetColor(optix_guicolors.colora);
    //gfx_SetColor(0);
    gfx_FillTriangle(10, 225, 30, 225, 20, 230);*/
    boot_GetTime(&seconds, &minutes, &hours);
    //shut up KingDubDub, it's fixed now
    //ti.clockFlags + 24 hour or whatever it's called (2nd bit)
    //I think I could do something like the below line to toggle this:
    //yeah that works
    //(*(uint8_t*)0xD000BF) ^= 4;
    if ((*(uint8_t*)0xD000BF)&4) { // accessing 8-bits at address D000BF (ti.flags+ti.clockFlags)
        if (minutes > 9) sprintf(str1, "%d:%d", hours, minutes);
        else sprintf(str1, "%d:0%d", hours, minutes);
    } else {
        if (hours == 0) hours = 12;
        if (boot_IsAfterNoon()) {
            if (minutes > 9) sprintf(str1, "%d:%d PM", hours - 12, minutes);
            else sprintf(str1, "%d:0%d PM", hours - 12, minutes);
        } else {
            if (minutes > 9) sprintf(str1, "%d:%d AM", hours, minutes);
            else sprintf(str1, "%d:0%d AM", hours, minutes);
        }
    }
    //optix_CusText(false);
    gfx_SetTextFGColor(vysion_settings.taskbartextcolor);
    gfx_SetTextBGColor(vysion_settings.taskbarcolor);
    gfx_SetTextTransparentColor(vysion_settings.taskbarcolor);
    //if the clock is on
    if ((*(uint8_t*)0xD000BF)&64) gfx_PrintStringXY(str1, 295 - gfx_GetStringWidth(str1), 226);
    //draw the battery indicator
    //background
    gfx_SetColor(vysion_settings.taskbarcolor);
    gfx_FillRectangle(301, 226, 15, 8);
    gfx_SetColor(vysion_settings.taskbarbordercolor);
    gfx_Rectangle(317, 228, 1, 4);
    gfx_Rectangle(301, 226, 15, 8);
    for (i = 0; i < vysion_programdata.batterystatus; i++) gfx_FillRectangle(303 + i * 3, 228, 2, 4);
    optix_RenderMenu(vysion_programdata.taskbar);
}

void vysion_CenterTaskbar(void) {
    optix_menu[vysion_programdata.taskbar].x = 160 - (optix_menu[vysion_programdata.taskbar].numoptions * optix_menu[vysion_programdata.taskbar].xspacing) / 2;
}

void vysion_FileExplorer(int folder) {
    struct vysion_file_t *currfile;
    uint8_t sidebar;
    uint8_t main;
    int currfolder;
    char *sidebartext = "< Back`Root`Desktop`Exit`";
    uint8_t rows;
    uint8_t columns;
    bool runrefresh;
    //just will store a number and whether or not it's a program or folder
    bool clipboardisfolder;
    //default state is -1, or with nothing on it
    int clipboard;
    int temp;
    uint8_t filetype;
    int currmenus[2];
    //because apparently OPTIX can't do this properly
    gfx_sprite_t *arrowback = gfx_MallocSprite(84, 12);
    //optix_SaveActiveMenus();
    //gfx_Blit(0);
    rows = 9;
    columns = 2;
    runrefresh = false;
    //clipboard = -1;
    //maybe add icons later
    //set the default folder state (which in this case is the root)
    vysion_SetTempMenu(folder, -1, true, true);
    vysion_GetTempMenuIcons();
    vysion_GetTempMenuText();
    //this is the menu on the right
    optix_AddMenu(optix_GetWindowXCentering(0, 100 + 100 * columns, columns, 93), optix_GetWindowYCentering(1, rows * 20, rows, 20), 0, 0, 1, rows, 92, 20, sidebartext, NULL);
    sidebar = optix_guidata.nummenus - 1;
    if (vysion_filesysteminfo.numtempfiles > 0) optix_AddMenu(optix_GetWindowXCentering(2, 100 + 100 * columns, columns, 100) - 7, optix_GetWindowYCentering(1, rows * 20, rows, 20), 0, 0, columns, rows, 100, 20, vysion_programdata.str1, vysion_programdata.icon);
    else optix_AddMenu(optix_GetWindowXCentering(2, 100 + 100 * columns, columns, 100) - 8, optix_GetWindowYCentering(1, rows * 20, rows, 20), 0, 0, columns, rows, 100, 20, "No files...`", NULL);
    main = optix_guidata.nummenus - 1;
    //optix_AddMenu(0, 0, 0, 0, 1, 8, 100, 20, str1, icon);
    //optix_AddMenu(0, 0, 0, 0, 16, 11, 20, 20, str1, icon);
    //left aligned, centered vertically, icon offset of 2, text shifted right by 20 pixels
    optix_SetAdvancedMenuCentering(main, 0, 1, 0, 1, 22, 0, 2, 0);
    optix_SetAdvancedMenuCentering(sidebar, 0, 1, 0, 1, 5, 0, 2, 0);
    optix_guidata.currmenu = main;
    optix_menu[main].currselection = vysion_programdata.fileexplorerselection;
    optix_menu[main].menumin = vysion_programdata.fileexplorermenumin;
    vysion_programdata.currfolder = folder;
    currmenus[0] = main;
    currmenus[1] = sidebar;
    optix_SetActiveMenus(currmenus, 2);
    //gfx_Blit(1);
    //optix_cursor.x = 160;
    //optix_cursor.y = 120;
    optix_menu[main].enterpressed = false;
    gfx_GetSprite(arrowback, 214, 228);
    while (!(kb_Data[6] & kb_Clear)) {
        struct optix_menu_t *m = &optix_menu[optix_guidata.currmenu];
        struct vysion_tempfile_t *v = &vysion_tempfile[optix_menu[main].currselection];
        currfile = &vysion_file[v->index];
        kb_Scan();
        //make sure we're at the left edge of the menu
        if (kb_Data[7] & kb_Left && (optix_menu[main].currselection + 1) % columns == 1 && optix_guidata.keypress && optix_guidata.currmenu == main) {
            optix_guidata.currmenu = sidebar;
            optix_guidata.keypress = false;
        }
        if (kb_Data[7] & kb_Right && optix_guidata.keypress && optix_guidata.currmenu == sidebar) {
            optix_guidata.currmenu = main;
            optix_guidata.keypress = false;
        }
        if (optix_guidata.currmenu == main && vysion_programdata.currfolder != 0) vysion_CheckAlphaJump();
        //maybe the file operations menu?
        //you're now not allowed to do anything to root
        if (kb_Data[1] & kb_Mode && optix_guidata.keypress && optix_guidata.currmenu == main && vysion_programdata.currfolder != 0) {
            optix_guidata.keypress = false;
            temp = m->currselection;
            //do something
            if (vysion_filesysteminfo.numtempfiles == 0) filetype = FILE_OPERATIONS_NOFILE;
            else if (v->isfolder) filetype = FILE_OPERATIONS_FOLDER;
            else if (currfile->type == VYSION_BASIC_TYPE || currfile->type == VYSION_PBASIC_TYPE) filetype = FILE_OPERATIONS_TI_BASIC_PRGM;
            else if (currfile->type == VYSION_APPVAR_TYPE) filetype = FILE_OPERATIONS_APPVAR;
            else filetype = FILE_OPERATIONS_ASM_PRGM;
            //optix_HandleGUI();
            //gfx_SwapDraw();
            //gfx_Blit(0);
            //gfx_BlitRectangle(0, optix_cursor.x - 6, optix_cursor.y - 6, 12, 12);
            switch (vysion_FileOperationsMenu(filetype)) {
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
                    vysion_AddFolder(optix_GetStringInput("Name?", 10, 100, 8), vysion_programdata.currfolder);
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
            optix_SetActiveMenus(currmenus, 2);
        }
        //actual code
        //refresh it to open a new folder if needed
        if ((m->enterpressed && optix_guidata.currmenu != OPTIX_MENU_INVALID && !((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd))) || runrefresh) {
            //do something if it's a folder
            if ((vysion_tempfile[optix_menu[main].currselection].isfolder && optix_guidata.currmenu == main) || (optix_guidata.currmenu == sidebar && m->currselection < 3) || (runrefresh)) {
                //delete it
                if (!runrefresh) {
                    if (optix_guidata.currmenu == main) {
                        vysion_programdata.currfolder = vysion_tempfile[m->currselection].index;
                        vysion_SetTempMenu(vysion_programdata.currfolder, -1, true, true);
                    //if it's the root we can't go back
                    } else if (m->currselection == 0 && vysion_folder[vysion_programdata.currfolder].location != -1) {
                        vysion_programdata.currfolder = vysion_folder[vysion_programdata.currfolder].location;
                        vysion_SetTempMenu(vysion_programdata.currfolder, -1, true, true);
                    } else if (m->currselection == 1 || m->currselection == 2) {
                        //since root is 0 and desktop is 1
                        vysion_programdata.currfolder = m->currselection - 1;
                        vysion_SetTempMenu(vysion_programdata.currfolder, -1, true, true);
                    }
                //just redo the current one
                } else {
                    vysion_SetTempMenu(vysion_programdata.currfolder, -1, true, true);
                    runrefresh = false;
                }
                //we don't need to do anything otherwise
                vysion_GetTempMenuIcons();
                vysion_GetTempMenuText();
                optix_DeleteLastMenu();
                if (vysion_filesysteminfo.numtempfiles > 0) optix_AddMenu(optix_GetWindowXCentering(2, 100 + 100 * columns, columns, 100) - 7, optix_GetWindowYCentering(1, rows * 20, rows, 20), 0, 0, columns, rows, 100, 20, vysion_programdata.str1, vysion_programdata.icon);
                else optix_AddMenu(optix_GetWindowXCentering(2, 100 + 100 * columns, columns, 100) - 7, optix_GetWindowYCentering(1, rows * 20, rows, 20), 0, 0, columns, rows, 100, 20, "No files...", NULL);
                optix_SetAdvancedMenuCentering(main, 0, 1, 0, 1, 22, 0, 2, 0);
                //this could probably get messed up
                m = &optix_menu[optix_guidata.currmenu];
                //fix this
                optix_menu[main].currselection = temp;
            } else if (optix_guidata.currmenu == sidebar) break;
            else if (optix_menu[main].enterpressed && optix_guidata.currmenu == main) {
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
            }
            m->enterpressed = false;
        }
        //make the start menu still work here
        vysion_CheckStartMenu();
        if (vysion_programdata.startreturn) break;
        //gui stuff
        //gfx_FillScreen(optix_guicolors.bgcolor);
        /*gfx_ZeroScreen();
        gfx_PrintStringXY(vysion_folder[0].name, 5, 5);
        gfx_PrintUInt(currfolder, 3);
        gfx_PrintUInt(vysion_tempfile[m->currselection].index, 3);*/
        optix_UpdateCurrMenu();
        /*gfx_FillScreen(optix_guicolors.bgcolor);
        optix_CusText(false);
        gfx_PrintStringXY("Clipboard: ", 5, 5);
        gfx_PrintUInt(clipboard, 3);
        gfx_PrintString(", ");
        gfx_PrintUInt(clipboardisfolder, 2);*/
        optix_RenderWindow("File explorer", 100 + columns * 100 + 2, rows * 20 + 2);
        //separator
        gfx_VertLine(optix_menu[main].x - 1, optix_menu[main].y, rows * 20);
        //the scroll bar
        //I don't want to write all this out
        m = &optix_menu[main];
        optix_VertScrollbar(m->x + m->xspacing * m->width, m->y - 1, m->menumin / m->width, (m->numoptions > m->width * m->height) * ((m->numoptions + 1) / m->width - m->height), 8, rows * 20 + 2, false);
        optix_RenderMenu(main);
        optix_RenderMenu(sidebar);
        gfx_Sprite(arrowback, 214, 228);
        //vysion_RenderToolbar();
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    while (kb_AnyKey()) kb_Scan();
    //clean things up
    optix_DeleteLastMenu();
    optix_DeleteLastMenu();
    vysion_programdata.fileexplorerselection = 0;
    vysion_programdata.fileexplorermenumin = 0;
    //save for later
    vysion_SaveFilesystem();
    vysion_RenderDesktop();
    gfx_Blit(1);
    free(arrowback);
    //optix_RestoreActiveMenus();
}

uint8_t vysion_FileOperationsMenu(uint8_t filetype) {
    gfx_sprite_t *background;
    //this definitely isn't the most efficient way to do it
    //folder, TI-BASIC prgm, asm program, appvar
    const char *menutext[] = {"Cut`Paste`Delete`New folder`",
        "Run`Cut`Paste`Edit`Delete`New folder`Properties`",
        "Run`Cut`Paste`Delete`New folder`Properties`",
        "Cut`Paste`Delete`New folder`Properties`",
        "Paste`New folder`"};
    //rows, columns
    int returnvalue[5][7] = {{FILE_OPERATIONS_CUT, FILE_OPERATIONS_PASTE, FILE_OPERATIONS_DELETE, FILE_OPERATIONS_NEW_FOLDER, 0, 0, 0},
        {FILE_OPERATIONS_RUN, FILE_OPERATIONS_CUT, FILE_OPERATIONS_PASTE, FILE_OPERATIONS_EDIT, FILE_OPERATIONS_DELETE, FILE_OPERATIONS_NEW_FOLDER, FILE_OPERATIONS_PROPERTIES},
        {FILE_OPERATIONS_RUN, FILE_OPERATIONS_CUT, FILE_OPERATIONS_PASTE, FILE_OPERATIONS_DELETE, FILE_OPERATIONS_NEW_FOLDER, FILE_OPERATIONS_PROPERTIES, 0},
        {FILE_OPERATIONS_CUT, FILE_OPERATIONS_PASTE, FILE_OPERATIONS_DELETE, FILE_OPERATIONS_NEW_FOLDER, FILE_OPERATIONS_PROPERTIES, 0, 0},
        {FILE_OPERATIONS_PASTE, FILE_OPERATIONS_NEW_FOLDER, 0, 0, 0, 0, 0}};
    struct optix_menu_t *m;
    uint8_t realreturnvalue;
    int currmenu[] = {optix_guidata.nummenus};
    optix_SetActiveMenus(currmenu, 1);
    optix_guisettings.cursoron = false;
    //it's going to be on the top of the screen, at 0, 0
    //because it's easy to do that
    optix_AddMenu(1, 1, 0, 0, 1, 7, 100, 14, menutext[filetype], NULL);
    optix_guidata.currmenu = optix_guidata.nummenus - 1;
    m = &optix_menu[optix_guidata.currmenu];
    background = gfx_MallocSprite(m->width * m->xspacing + 2, m->numoptions * m->yspacing + 2);
    //get the background
    gfx_Blit(0);
    gfx_GetSprite(background, m->x - 1, m->y - 1);
    while (!m->enterpressed || (kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) {
        if (((kb_Data[6] & kb_Clear) || (kb_Data[1] & kb_Mode)) && optix_guidata.keypress) {
            optix_guidata.keypress = false;
            realreturnvalue = 255;
            break;
        }
        optix_UpdateCurrMenu();
        gfx_SetColor(optix_guicolors.colora);
        gfx_FillRectangle(m->x - 1, m->y - 1, m->width * m->xspacing + 2, m->numoptions * m->yspacing + 2);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(m->x - 1, m->y - 1, m->width * m->xspacing + 2, m->numoptions * m->yspacing + 2);
        optix_RenderMenu(optix_guidata.nummenus - 1);
        realreturnvalue = returnvalue[filetype][m->currselection];
        /*gfx_SetTextXY(200, 5);
        gfx_SetColor(0);
        gfx_FillRectangle(199, 4, 40, 10);
        gfx_PrintUInt(returnvalue[0][0], 3);*/

        //optix_RenderCursor();
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    gfx_Sprite(background, m->x - 1, m->y - 1);
    gfx_Blit(1);
    optix_DeleteLastMenu();
    optix_guidata.currmenu--;
    free(background);
    while (kb_AnyKey()) kb_Scan();
    /*gfx_FillScreen(0);
    optix_WhiText();
    gfx_SetTextXY(5, 5);
    gfx_PrintUInt(realreturnvalue, 3);
    gfx_SwapDraw();
    while (!os_GetCSC());*/
    optix_guisettings.cursoron = vysion_settings.cursoron;
    return (uint8_t) realreturnvalue;
}

uint8_t vysion_StartMenu(void) {
    const char *menutext = "Programs`Appvars`Files`Search`Settings`Refresh`Power`Exit";
    struct optix_menu_t *m;
    uint8_t returnvalue;
    gfx_sprite_t *background;
    //sprites, cool
    gfx_sprite_t *spr[] = {startprogram, startappvar, startfile, startsearch, startsettings, startrefresh, startpower, startexit};
    //it's going to be on the top of the screen, at 0, 0
    //because it's easy to do that
    //may as well put this here
    int currmenu[] = {optix_guidata.nummenus};
    optix_SetActiveMenus(currmenu, 1);
    optix_AddMenu(1 + startmenuicon->width + 2, 220 - 8 * 16 - 1, 0, 0, 1, 9, 100, 16, menutext, spr);
    optix_guidata.currmenu = optix_guidata.nummenus - 1;
    m = &optix_menu[optix_guidata.currmenu];
    //left aligned, centered vertically, icon offset of 2, text shifted right by 20 pixels
    optix_SetAdvancedMenuCentering(optix_guidata.currmenu, 0, 1, 0, 1, 18, 0, 2, 0);
    optix_guidata.keypress = false;
    //get the background
    background = gfx_MallocSprite(2 + startmenuicon->width + 2 + optix_menu[optix_guidata.currmenu].xspacing, optix_menu[optix_guidata.currmenu].numoptions * optix_menu[optix_guidata.currmenu].yspacing + 2);
    gfx_GetSprite(background, 0, optix_menu[optix_guidata.currmenu].y - 1);
    while (!m->enterpressed || (kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) {
        if (((kb_Data[6] & kb_Clear) || (kb_Data[1] & kb_Yequ)) && optix_guidata.keypress) {
            optix_guidata.keypress = false;
            returnvalue = 255;
            break;
        }
        optix_UpdateCurrMenu();
        gfx_SetColor(optix_guicolors.colora);
        gfx_FillRectangle(m->x - 1, m->y - 1, m->width * m->xspacing + 2, m->numoptions * m->yspacing + 2);
        //let's fix this
        gfx_SetColor(0);
        gfx_FillRectangle(0, 220 - m->numoptions * m->yspacing - 2, startmenuicon->width + 2, m->numoptions * m->yspacing + 2);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(m->x - 1, m->y - 1, m->width * m->xspacing + 2, m->numoptions * m->yspacing + 2);
        //to border the logo
        gfx_Rectangle(0, 220 - m->numoptions * m->yspacing - 2, startmenuicon->width + 2, m->numoptions * m->yspacing + 2);
        //the logo
        gfx_Sprite(startmenuicon, 1, 220 - m->numoptions * m->yspacing - 1);
        optix_RenderMenu(optix_guidata.nummenus - 1);
        returnvalue = m->currselection;
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    //restore the background
    gfx_Sprite(background, 0, optix_menu[optix_guidata.nummenus - 1].y - 1);
    gfx_Blit(1);
    free(background);
    optix_DeleteLastMenu();
    optix_guidata.currmenu--;
    while (kb_AnyKey()) kb_Scan();
    return returnvalue;
}

//to display the start menu when running other functions
void vysion_CheckStartMenu(void) {
    kb_Scan();
    if (kb_Data[1] & kb_Yequ) {
        optix_SaveActiveMenus();
        vysion_programdata.startreturnvalue = vysion_StartMenu();
        if (vysion_programdata.startreturnvalue < VYSION_START_MENU_OPTIONS) {
            vysion_programdata.startreturn = true;
        }
        optix_RestoreActiveMenus();
    }
}

void vysion_PropertiesMenu(int file) {
    struct vysion_file_t *currfile;
    char str1[100];
    const char *yesno[] = {"N", "Y"};
    const char *text[] = {"Type: ", "Size: ", "Archived: ", "Taskbar pinned: ", "Location: "};
    const char *type[] = {"TI-BASIC (P)", "TI-BASIC", "ICE source", "ICE", "C", "ez80", "appvar", "unknown"};
    const char *menutext = "Rename`Archived`Locked`Start pinned`Taskbar pinned`";
    bool *properties[] = {&vysion_file[file].archived, &vysion_file[file].locked, &vysion_file[file].startpinned, &vysion_file[file].taskbarpinned};
    struct optix_menu_t *m;
    int i;
    uint8_t width = 160;
    uint8_t height = 130;
    uint16_t windowx;
    uint8_t windowy;
    uint8_t vysiontype;
    int currmenu[] = {optix_guidata.nummenus};
    ti_var_t slot;
    currfile = &vysion_file[file];
    vysiontype = 0;
    vysiontype = vysion_GetFileType(currfile->type);
    windowx = optix_GetWindowXCentering(0, width, 1, width);
    windowy = optix_GetWindowYCentering(0, height, 1, height);
    currfile = &vysion_file[file];
    //make sure this is correct, because apparently it could get messed up somewhere
    if (currfile->type == VYSION_PBASIC_TYPE || currfile->type == VYSION_BASIC_TYPE) currfile->locked = (vysiontype == TI_PPRGM_TYPE);
    //a menu
    optix_AddMenu(windowx, windowy + height - 5 * 16, 0, 0, 1, 5, 120, 16, menutext, NULL);
    optix_SetAdvancedMenuCentering(optix_guidata.nummenus - 1, 0, 1, 0, 0, 4, 0, 0, 0);
    optix_guidata.currmenu = optix_guidata.nummenus - 1;
    m = &optix_menu[optix_guidata.currmenu];
    optix_SetActiveMenus(currmenu, 1);
    while (!(kb_Data[6] & kb_Clear)) {
        kb_Scan();
        //property changing stuff
        optix_UpdateCurrMenu();
        if (m->enterpressed && !((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd))) {
            if (m->currselection > 0) {
                if (m->currselection == 1) {
                    ti_CloseAll();
                    if ((bool) (slot = ti_OpenVar(currfile->name, "r", vysiontype))) {
                        if (ti_SetArchiveStatus(!currfile->archived, slot)); 
                        else optix_Message("ERROR", "Something went wrong...", 12, 120, 6);
                        currfile->archived = ti_IsArchived(slot);
                    } else optix_Message("ERROR", "Something went wrong...", 12, 120, 6);
                } else if (m->currselection == 2) {
                    //lock/unlock
                    if (currfile->type == VYSION_PBASIC_TYPE || currfile->type == VYSION_BASIC_TYPE) {
                        ti_CloseAll();
                        if ((bool) (slot = ti_OpenVar(currfile->name, "r", vysion_GetFileType(currfile->type)))) {
                            //basically just change the T1 type byte to TI_PRGM_TYPE if it's protected, and TI_PPRGM_TYPE if it's not
                            //make sure the type is set correctly
                            if (vysiontype == VYSION_BASIC_TYPE) {
                                currfile->type = vysiontype = VYSION_PBASIC_TYPE;
                                *((uint8_t *) ti_GetVATPtr(slot)) = TI_PPRGM_TYPE;
                            } else {
                                currfile->type = vysiontype = VYSION_BASIC_TYPE;
                                *((uint8_t *) ti_GetVATPtr(slot)) = TI_PRGM_TYPE;
                            }
                            *properties[m->currselection - 1] = currfile->type == VYSION_PBASIC_TYPE;
                        } else optix_Message("ERROR", "Something went wrong...", 12, 120, 6);
                    } else *properties[m->currselection - 1] = !*properties[m->currselection - 1];
                } else *properties[m->currselection - 1] = !*properties[m->currselection - 1];
            } else {
                //it is 0
                ti_CloseAll();
                //optix_stringinput = realloc(optix_stringinput, strlen(currfile->name) + 1);
                //strcpy(optix_stringinput, currfile->name);
                strcpy(str1, optix_GetStringInput("Rename", 12, 120, 8));
                //convert the string to all uppercase
                for (i = 0; i < strlen(str1); i++) if (str1[i] >= 'a' && str1[i] <= 'z') str1[i] -= 32;
                if (strlen(str1)) {
                    if (ti_RenameVar(currfile->name, str1, vysiontype) == 0) strcpy(currfile->name, str1);
                    else optix_Message("ERROR", "Something went wrong...", 12, 120, 6);
                } else optix_Message("ERROR", "Name cannot be a blank string.", 12, 120, 6);
            }
            m->enterpressed = false;
        }
        sprintf(str1, "%s properties", currfile->name);
        optix_RenderWindow(str1, width + 2, height + 2);
        gfx_Sprite(currfile->icon, windowx + 2, windowy + 2);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_HorizLine(windowx, windowy + 20, width);
        optix_RenderMenu(optix_guidata.nummenus - 1);
        optix_CusText(false);
        if (currfile->type == VYSION_ASM_TYPE || currfile->type == VYSION_C_TYPE || currfile->type == VYSION_ICE_TYPE) gfx_PrintStringXY(currfile->description, windowx + 20, windowy + 6);
        else gfx_PrintStringXY(currfile->name, windowx + 20, windowy + 6);
        //gfx_PrintUInt(vysiontype, 3);
        //print the rest
        for (i = 0; i < 2; i++) {
            strcpy(str1, text[i]);
            switch (i) {
                case 0:
                    strcat(str1, type[currfile->type]);
                    break;
                case 1:
                    sprintf(str1, "%s%d", text[i], currfile->size);
                    break;
                case 2:
                    strcat(str1, yesno[currfile->archived]);
                    break;
                case 3:
                    strcat(str1, yesno[currfile->taskbarpinned]);
                    break;
                case 4:
                    sprintf(str1, "%s%d", text[i], currfile->location);
                    break;
            }
            gfx_PrintStringXY(str1, windowx + 4, windowy + 25 + i * 12);
        }
        gfx_SetColor(optix_guicolors.colorb);
        gfx_HorizLine(windowx, windowy + 20 + 2 * 12 + 5, width);
        for (i = 0; i < 4; i++) optix_BoolSwitch(*properties[i], windowx + width - 20 - 4, (windowy + height - 4 * 16 + 2) + i * 16, 20, 10);
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    while (kb_AnyKey()) kb_Scan();
    optix_DeleteLastMenu();
    optix_guidata.currmenu--;
}

void vysion_RenderDesktop(void) {
    gfx_FillScreen(optix_guicolors.bgcolor);
    //background cool
    if (vysion_programdata.wallpaper != NULL && vysion_programdata.wallpapervalid && vysion_settings.showdesktopwallpaper) gfx_ScaledSprite_NoClip(vysion_programdata.wallpaper, 0, 0, 2, 2);
    vysion_RenderToolbar();
    //optix_RenderMenu(vysion_programdata.desktop);
}

void vysion_SetWallpaper(char name[9]) {
    struct vysion_appvarreader_t *appvarreader;
    uint8_t search[] = {0x56, 0x00, 0x59, 0x53, 0x57, 0x41, 0x4c, 0x46, 0x49, 0x58};
    //we're assuming that this is an appvar prepended with VYSWALLP
    //taken from the convimg file
    //unsigned int data, i;
    ti_var_t slot;
    char appvarstring[10] = VYSION_WALLPAPER_STRING;
    //gfx_sprite_t *temp = gfx_MallocSprite(160, 120);
    //void *location;
    appvarreader = malloc(sizeof(struct vysion_appvarreader_t));
    ti_CloseAll();
    //let's just hope no one makes a bad wallpaper because that could screw everything up
    vysion_programdata.wallpapervalid = true;
    if (slot = ti_OpenVar(name, "r", TI_APPVAR_TYPE)) {
        if (memcmp(search, ti_GetDataPtr(slot), 10) == 0) {
            //gfx_FillScreen(7);
            //gfx_SwapDraw();
            zx7_Decompress(appvarreader, ti_GetDataPtr(slot));
            //gfx_FillScreen(244);
            //gfx_SwapDraw();
            memcpy(appvarstring, appvarreader->name, strlen(VYSION_WALLPAPER_STRING));
            //check if it starts with the string
            if (strcmp(appvarstring, VYSION_WALLPAPER_STRING) == 0) {
                if (appvarreader->spr[0] == 160 && appvarreader->spr[1] == 120) {
                    memcpy(vysion_programdata.wallpaper, appvarreader->spr, (size_t) (160 * 120 + 2));
                    strcpy(vysion_settings.wallpaperappvar, name);
                } else vysion_programdata.wallpapervalid = false;
            } else vysion_programdata.wallpapervalid = false;
        } else vysion_programdata.wallpapervalid = false;
    } else vysion_programdata.wallpapervalid = false;
    ti_CloseAll();
    free(appvarreader);
    //free(temp);
}

void vysion_SettingsMenu(void) {
    //this is getting boring
    //make some kind of thing at the top
    struct optix_menu_t *m;
    const char *maintext = "System`Shell`Colors`Desktop`Taskbar`Reset`";
    const char *subtext[] = {"Hour`Minute`24-hour`Clock on`Brightness`",
                            "Refresh files`Show lock screen`Require password`Use cursor`Set password`About`",
                            "Background`Title text`Text`Selection`Window`Window border`Outline`",
                            "Wallpaper`Show wallpaper`Show folders`Show files`",
                            "Transparent`Centered`Color`Border color`Text color`",
                            "Reset filesystem`Reset settings`Reset state`Uninstall`"};
    const char *filedetection[] = {"None", "New files", "Deleted files", "All"};
    char appvarnames[3][9];
    char temp[10];
    uint8_t topmenu;
    uint8_t submenu = 0;
    int lastselection;
    uint8_t windowheight = 127 + 7;
    uint16_t windowx;
    uint8_t windowy;
    int i;
    char buffer[100];
    //hours, minutes, seconds
    uint8_t time[3];
    int currmenu[2];
    uint8_t *colors[] = {&optix_guicolors.bgcolor, &optix_guicolors.ttextfgcolor, &optix_guicolors.textfgcolor, &optix_guicolors.selectcolor, &optix_guicolors.colora, 
    &optix_guicolors.colorb, &optix_guicolors.outlinecolor};
    bool *taskbarbool[] = {&vysion_settings.transparenttaskbar, &vysion_settings.centeredtaskbar};
    bool *desktopbool[] = {&vysion_settings.showdesktopwallpaper, &vysion_settings.showdesktopfolders, &vysion_settings.showdesktopfiles};
    bool *startupbool[] = {&vysion_settings.displaylockscreen, &vysion_settings.requirepassword, &vysion_settings.cursoron};
    uint8_t *taskbarcolors[] = {&vysion_settings.taskbarcolor, &vysion_settings.taskbarbordercolor, &vysion_settings.taskbartextcolor};
    //because 24 hour time is the 2nd bit, and clockon is the 7th
    uint8_t systembool[] = {4, 64};
    boot_GetTime(&time[2], &time[1], &time[0]);
    windowx = optix_GetWindowXCentering(0, 240, 1, 1);
    windowy = optix_GetWindowYCentering(0, windowheight, 1, 1);
    optix_AddMenu(optix_GetWindowXCentering(0, 240, 4, 60), optix_GetWindowYCentering(0, windowheight, 1, 14), 0, 0, 4, 1, 60, 14, maintext, NULL);
    optix_guidata.currmenu = topmenu = optix_guidata.nummenus - 1;
    optix_AddMenu(optix_GetWindowXCentering(0, 240, 4, 60), optix_GetWindowYCentering(0, windowheight, 1, 14) + 23, 0, 0, 1, 8, windowheight, 16, subtext[optix_menu[topmenu].currselection], NULL);
    submenu = optix_guidata.nummenus - 1;
    currmenu[0] = topmenu;
    currmenu[1] = submenu;
    strcpy(appvarnames[0], VYSION_FILESYSTEM_APPVAR);
    strcpy(appvarnames[1], VYSION_DATA_APPVAR);
    strcpy(appvarnames[2], VYSION_STATE_APPVAR);



    optix_SetActiveMenus(currmenu, 2);
    optix_SetAdvancedMenuCentering(submenu, 0, 1, 0, 0, 4, 0, 0, 0);
    while (!(kb_Data[6] & kb_Clear)) {
        kb_Scan();
        //logic
        //if you pressed up and the current selection is the top option on the submenu go up to the other one
        if ((kb_Data[7] & kb_Up) && (optix_guidata.currmenu == submenu) && (optix_menu[submenu].currselection == 0) && optix_guidata.keypress) {
            optix_guidata.currmenu = topmenu;
            optix_guidata.keypress = false;
        }
        if ((kb_Data[7] & kb_Down) && (optix_guidata.currmenu == topmenu) && optix_guidata.keypress) {
            optix_guidata.currmenu = submenu;
            optix_guidata.keypress = false;
        }
        //handle changing options and stuff
        //this is not going to be clean but it will work
        if (optix_menu[submenu].enterpressed && !(kb_Data[6] & kb_Enter || kb_Data[1] & kb_2nd)) {
            switch (optix_menu[topmenu].currselection) {
                //startup
                case 0:
                    //make sure the current time is correct
                    //optix_Slider(0, 0, 100, 16, 0, 60);
                    boot_GetTime(&time[2], &time[1], &time[0]);
                    if (optix_menu[submenu].currselection < 2) 
                        time[optix_menu[submenu].currselection] = (uint8_t) optix_Slider(windowx + 240 - 101, windowy + 21 + (optix_menu[submenu].currselection == 1) * 16, 102, 18, 0, 23 + (optix_menu[submenu].currselection == 1) * 36, time[optix_menu[submenu].currselection]);
                    else if (optix_menu[submenu].currselection < 4) (*(uint8_t*)0xD000BF) ^= (systembool[optix_menu[submenu].currselection - 2]);
                    else lcd_BacklightLevel = (uint8_t) optix_Slider(windowx + 240 - 101, windowy + 21 + 4 * 16, 102, 18, 0, 255, lcd_BacklightLevel);
                    boot_SetTime(0, time[1], time[0]);
                    //vysion_RenderDesktop();
                    //gfx_Blit(1);
                    break;
                case 1:
                    if (optix_menu[submenu].currselection == 0) vysion_settings.filedetection = optix_SimpleMenu(windowx + 240 - 100, windowy + 22, 100, 16, "None`New files`Deleted files`All`");
                    else if (optix_menu[submenu].currselection == 4) strcpy(vysion_settings.password, optix_GetStringInput("Password?", 10, 160, 16));
                    else if (optix_menu[submenu].currselection == 5) {
                        sprintf(buffer, "VERSION: %s %s %s `BUILT: %s", VYSION_NAME, VYSION_DISTRIBUTION_NAME, VYSION_VERSION, __TIMESTAMP__);
                        optix_Message("About", buffer, 12, 210, 4);
                    }
                    else {
                        *startupbool[optix_menu[submenu].currselection - 1] = !*startupbool[optix_menu[submenu].currselection - 1];
                        optix_guisettings.cursoron = vysion_settings.cursoron;
                    }
                    break;
                //color stuff
                    case 2:
                    *colors[optix_menu[submenu].currselection] = optix_ColorPicker(*colors[optix_menu[submenu].currselection]);
                    vysion_RenderDesktop();
                    gfx_Blit(1);
                    optix_guicolors.textbgcolor = optix_guicolors.colora;
                    optix_guicolors.ttextbgcolor = optix_guicolors.colorb;
                    break;
                case 3:
                    if (optix_menu[submenu].currselection == 0) vysion_SelectWallpaper();
                    else *desktopbool[optix_menu[submenu].currselection - 1] = !*desktopbool[optix_menu[submenu].currselection - 1];
                    optix_SetActiveMenus(currmenu, 2);
                    break;
                //taskbar
                case 4:
                    if (optix_menu[submenu].currselection < 2) *taskbarbool[optix_menu[submenu].currselection] = !*taskbarbool[optix_menu[submenu].currselection];
                    else *taskbarcolors[optix_menu[submenu].currselection - 2] = optix_ColorPicker(*taskbarcolors[optix_menu[submenu].currselection - 2]);
                    //vysion_RenderDesktop();
                    //gfx_Blit(1);
                    break;
                //resetting stuff
                case 5:
                    if (optix_menu[submenu].currselection < 3) {
                        if (!optix_Menu("Reset?", "Yes`No`", 12, 120, 3)) {
                            vysion_SaveFilesystem();
                            vysion_SaveData();
                            if (ti_Delete(appvarnames[optix_menu[submenu].currselection])) {
                                optix_Message("SUCCESS", "Successfully reset. VYSION will now exit.", 12, 120, 8);
                                gfx_End();
                                exit(0);
                            } else optix_Message("ERROR", "Failed to reset...", 12, 120, 8);
                        } 
                    } else {
                        //the user wants to uninstall VYSION... :(
                        if (!optix_Menu("Uninstall?", "Yes`No`", 12, 120, 3)) {
                            if (ti_DeleteVar(VYSION_NAME, TI_PPRGM_TYPE)) {
                                for (int i = 0; i < 3; i++) ti_Delete(appvarnames[i]);
                                optix_Message("SUCCESS", "Successfully uninstalled. Please post on the Cemetech forum for this project"
                                " why you made this decision, so the program can be improved. Exiting now...", 12, 200, 8);
                                gfx_End();
                                exit(0);
                            } optix_Message("ERROR", "Failed to uninstall...", 12, 120, 8);
                        }
                    }
                break;
                default:
                    break;

            }
            optix_menu[submenu].enterpressed = false;
        }
        if (optix_menu[topmenu].enterpressed && !(kb_Data[6] & kb_Enter || kb_Data[1] & kb_2nd)) optix_menu[topmenu].enterpressed = false;
        optix_UpdateCurrMenu();
        if (optix_menu[topmenu].currselection != lastselection) {
            optix_DeleteLastMenu();
            optix_AddMenu(optix_GetWindowXCentering(0, 240, 4, 60), optix_GetWindowYCentering(0, windowheight, 1, 14) + 22, 0, 0, 1, 8, windowheight, 16, subtext[optix_menu[topmenu].currselection], NULL);
            submenu = optix_guidata.nummenus - 1;
            optix_SetAdvancedMenuCentering(submenu, 0, 1, 0, 0, 4, 0, 0, 0);
        }
        //make sure the start menu still works
        vysion_CheckStartMenu();
        if (vysion_programdata.startreturn) break;
        lastselection = optix_menu[topmenu].currselection;
        //display it
        optix_RenderWindow("Settings", 240 + 2, windowheight + 2);
        optix_RenderMenu(topmenu);
        optix_RenderMenu(submenu);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_HorizLine(windowx, windowy + optix_menu[topmenu].yspacing, 240);
        optix_HorizScrollbar(windowx - 1, windowy + optix_menu[topmenu].yspacing, optix_menu[topmenu].menumin, 2, 242, 8, false);
        //draw the menu specific stuff
        //main this is going to be unoptimized
        switch (optix_menu[topmenu].currselection) {
            case 0:
                optix_CusText(false);
                //try not to slow down the program
                if (vysion_programdata.loops % 1000 == 0) boot_GetTime(&time[2], &time[1], &time[0]);
                for (i = 0; i < 2; i++) {
                    sprintf(temp, "%d", time[i]);
                    gfx_PrintStringXY(temp, windowx + 240 - gfx_GetStringWidth(temp) - 4, windowy + 14 + 12 + i * 16);
                }
                for (i = 2; i < 4; i++) {
                    optix_BoolSwitch(((*(uint8_t*)0xD000BF) & (systembool[i - 2])), windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                }
                sprintf(temp, "%d", lcd_BacklightLevel);
                gfx_PrintStringXY(temp, windowx + 240 - gfx_GetStringWidth(temp) - 4, windowy + 14 + 12 + 4 * 16);
                break;
            case 1:
                optix_CusText(false);
                gfx_PrintStringXY(filedetection[vysion_settings.filedetection], windowx + 240 - gfx_GetStringWidth(filedetection[vysion_settings.filedetection]) - 4, windowy + 14 + 12);
                for (i = 1; i < 4; i++) {
                    optix_BoolSwitch(*startupbool[i - 1], windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                }
                break;
            case 2:
                for (i = 0; i < 7; i++) {
                    gfx_SetColor(*colors[i]);
                    gfx_FillRectangle(windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                    gfx_SetColor(optix_guicolors.colorb);
                    gfx_Rectangle(windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                    //optix_BoolSwitch(*colors[i], windowx + 240 - 20 - 4, (windowy + windowheight - 4 * 16 + 2) + i * 16, 20, 10);
                }
                break;
            //desktop
            case 3:
                optix_CusText(false);
                if (vysion_programdata.wallpapervalid) gfx_PrintStringXY(vysion_settings.wallpaperappvar, windowx + 240 - gfx_GetStringWidth(vysion_settings.wallpaperappvar) - 4, windowy + 14 + 12);
                else gfx_PrintStringXY("Invalid", windowx + 240 - gfx_GetStringWidth("Invalid") - 4, windowy + 14 + 12);
                for (i = 1; i < 4; i++) {
                    optix_BoolSwitch(*desktopbool[i - 1], windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                }
                break;
            //taskbar
            case 4:
                for (i = 0; i < 2; i++) {
                    optix_BoolSwitch(*taskbarbool[i], windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                }
                for (i = 2; i < 5; i++) {
                    gfx_SetColor(*taskbarcolors[i - 2]);
                    gfx_FillRectangle(windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                    gfx_SetColor(optix_guicolors.colorb);
                    gfx_Rectangle(windowx + 240 - 20 - 4, windowy + i * 16 + 14 + 11, 20, 10);
                    //optix_BoolSwitch(*colors[i], windowx + 240 - 20 - 4, (windowy + windowheight - 4 * 16 + 2) + i * 16, 20, 10);
                }
                break;
            case 5:
                //for (i = 0; i < 3; i++) gfx_PrintStringXY(appvarnames[i], windowx + 240 - gfx_GetStringWidth(appvarnames[i]) - 4, windowy + 14 + 12 + i * 16);
                break;
            default:
                break;
        }
        //load a new menu
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    optix_DeleteLastMenu();
}

void vysion_SelectWallpaper(void) {
    uint8_t search[] = {0x56, 0x00, 0x59, 0x53, 0x57, 0x41, 0x4c, 0x46, 0x49, 0x58};
    ti_var_t slot;
    //will be a dynamic array
    void **spr = NULL;
    struct vysion_appvarreader_t *appvarreader;
    //gfx_sprite_t *back = NULL;
    char *var_name = NULL;
    uint8_t *search_pos = NULL;
    char files[1000] = "";
    int filesfound = 0;
    struct optix_menu_t *m;
    uint16_t windowwidth = 260;
    char appvarstring[10] = VYSION_WALLPAPER_STRING;
    int currmenu[1];
    appvarreader = malloc(sizeof(struct vysion_appvarreader_t));
    //vysion_programdata.wallpaper = NULL;
    //get all those files
    //this won't work anymore so fixed
    ti_CloseAll();
    optix_Dialogue("Loading...", "Finding wallpapers...", 10, 100, 4);
    gfx_SwapDraw();
    dbg_sprintf(dbgout, "Finding appvars...\n");
    while ((var_name = ti_DetectVar(&search_pos, search, TI_APPVAR_TYPE))) {
        dbg_sprintf(dbgout, "Found appvar %s", var_name);
        if (var_name == NULL) break;
        ti_CloseAll();
        if (!(slot = ti_Open(var_name, "r"))) break;
        //ti_Rewind(slot);
        if (memcmp(search, ti_GetDataPtr(slot), 10) != 0) continue;
        zx7_Decompress(appvarreader, ti_GetDataPtr(slot));
        memcpy(appvarstring, appvarreader->name, strlen(VYSION_WALLPAPER_STRING));
        if (strncmp(appvarstring, VYSION_WALLPAPER_STRING, strlen(VYSION_WALLPAPER_STRING)) == 0) {
            if (appvarreader->spr[0] == 160 && appvarreader->spr[1] == 120) {
                dbg_sprintf(dbgout, " VALID\n", var_name);
                //gfx_FillScreen(randInt(0, 255));
                //gfx_PrintStringXY(var_name, 5, 5);
                //gfx_Sprite(appvarreader->spr, 5, 15);
                //gfx_SwapDraw();
                //delay(100);
                filesfound++;
                spr = realloc(spr, 3 * filesfound);
                //this might work?
                spr[filesfound - 1] = ti_GetDataPtr(slot);
                strcat(files, var_name);
                strcat(files, "`");
            } else dbg_sprintf(dbgout, " INVALID\n", var_name);
        } else dbg_sprintf(dbgout, " INVALID\n", var_name);
    }
    dbg_sprintf(dbgout, "Finished finding appvars.\n");
    vysion_RenderDesktop();
    gfx_Blit(1);
    //gfx_FillScreen(7);
    //gfx_SwapDraw();
    //delay(100);
    if (filesfound == 0) strcpy(files, "NONE`");
    //let's make this in a window with 100 px on the side for the appvar names and 160 for the actual sprite so 160 x 120, showing 12 appvars at a time
    optix_AddMenu(optix_GetWindowXCentering(0, windowwidth, 1, 100), optix_GetWindowYCentering(0, 120, 10, 12), 0, 0, 1, 12, 99, 12, files, NULL);
    m = &optix_menu[optix_guidata.nummenus - 1];
    optix_guidata.currmenu = optix_guidata.nummenus - 1;
    currmenu[0] = optix_guidata.nummenus - 1;
    optix_SetActiveMenus(currmenu, 1);
    while (!(m->enterpressed || ((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)))) {
        optix_UpdateCurrMenu();
        if (kb_Data[6] & kb_Clear) {
            filesfound = 0;
            break;
        }
        //let's hope this works
        optix_RenderWindow("Wallpaper", windowwidth + 2, 120 + 2);
        optix_RenderMenu(optix_guidata.nummenus - 1);
        if (filesfound > 0 && optix_menu[optix_guidata.nummenus - 1].currselection != OPTIX_MENU_INVALID) {
            zx7_Decompress(appvarreader, spr[m->currselection]);
            gfx_Sprite((gfx_sprite_t *) appvarreader->spr, optix_GetWindowXCentering(2, windowwidth, 1, 160), optix_GetWindowYCentering(0, 120, 1, 120));
        }
        gfx_SetColor(optix_guicolors.colorb);
        gfx_VertLine(optix_GetWindowXCentering(0, windowwidth, 0, 0) + 99, optix_GetWindowYCentering(0, 120, 0, 0), 120);
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    if (spr != NULL) free(spr);
    free(appvarreader);
    optix_DeleteLastMenu();
    optix_guidata.currmenu--;
    while (kb_AnyKey()) kb_Scan();
    //maybe move this so we don't allocate too much memory at once
    if (filesfound > 0) vysion_SetWallpaper(m->text[m->currselection]);
    vysion_RenderDesktop();
    gfx_Blit(1);
}

//I think we can assume this will be done for the current menu?
void vysion_CheckAlphaJump(void) {
    char text[] = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0?\0VQLG\0\0.ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0\0";
    uint8_t skKey = 0;
    struct optix_menu_t *m = &optix_menu[optix_guidata.currmenu];
    gfx_Blit(0);
    if (!(kb_Data[2] & kb_Alpha && optix_guidata.keypress)) return;
    optix_guidata.keypress = false;
    optix_Dialogue("Waiting...", "Press a letter key to jump to that letter.", 12, 100, 6);
    gfx_SwapDraw();
    while (!text[skKey]) {
        skKey = os_GetCSC();
        if (skKey == sk_Clear) {
            while (kb_AnyKey()) kb_Scan();
            return;
        }
    }
    m->currselection = 0;
    //I think this will work?
    //no it won't because LLVM is a broken piece of shit
    //works without the +1 but not with it
    //please explain
    while (m->text[m->currselection][0] != text[skKey] && m->text[m->currselection + 1][0] <= text[skKey]) m->currselection++;
    while (kb_AnyKey()) kb_Scan();
}

void vysion_LockScreen(void) {
    const char *text[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    uint8_t day, month, hours, minutes, seconds;
    uint16_t year;
    //day will be used for seconds, and months for hours
    char str1[50];
    uint8_t i;
    uint8_t cursorblink = 0;
    optix_CusText(false);
    if (vysion_settings.displaylockscreen) {
        while (!os_GetCSC()) {
            kb_Scan();
            if (kb_Data[6] & kb_Clear) {
                gfx_End();
                exit(0);
            }
            gfx_FillScreen(optix_guicolors.bgcolor);
            //background cool
            if (vysion_programdata.wallpaper != NULL && vysion_programdata.wallpapervalid && vysion_settings.showdesktopwallpaper) gfx_ScaledSprite_NoClip(vysion_programdata.wallpaper, 0, 0, 2, 2);
            boot_GetTime(&seconds, &minutes, &hours);
            if ((*(uint8_t*)0xD000BF)&4) { //accessing 8-bits at address D000BF (ti.flags+ti.clockFlags)
                if (minutes > 9) sprintf(str1, "%d:%d", hours, minutes);
                else sprintf(str1, "%d:0%d", hours, minutes);
            } else {
                if (hours == 0) hours = 12;
                if (boot_IsAfterNoon()) {
                    if (minutes > 9) sprintf(str1, "%d:%d PM", hours - 12, minutes);
                    else sprintf(str1, "%d:0%d PM", hours - 12, minutes);
                } else {
                    if (minutes > 9) sprintf(str1, "%d:%d AM", hours, minutes);
                    else sprintf(str1, "%d:0%d AM", hours, minutes);
                }
            }
            gfx_SetTextScale(2, 2);
            gfx_PrintStringXY(str1, 160 - gfx_GetStringWidth(str1) / 2, 105);
            gfx_SetTextScale(1, 1);
            boot_GetDate(&day, &month, &year);
            sprintf(str1, "%s %d, %d", text[month - 1], day, year);
            gfx_PrintStringXY(str1, 160 - gfx_GetStringWidth(str1) / 2, 127);
            gfx_SwapDraw();
        }
    }
    if (vysion_settings.requirepassword) {
        optix_ResetStringInput();
        //while password is wrong
        while (kb_AnyKey()) kb_Scan();
        while (strcmp(optix_stringinput.text, vysion_settings.password) != 0) {
            optix_ResetStringInput();
            while (!(kb_Data[6] & kb_Enter)) {
                kb_Scan();
                optix_UpdateStringInput(optix_stringinput.numchars < 20);
                strcpy(str1, "");
                for (i = 0; i < strlen(optix_stringinput.text); i++) strcat(str1, "*");
                cursorblink++;
                if (cursorblink > 20) cursorblink = 0;
                if (cursorblink < 10) strcat(str1, "_");
                optix_CusText(false);
                gfx_FillScreen(optix_guicolors.bgcolor);
                //background cool
                if (vysion_programdata.wallpaper != NULL && vysion_programdata.wallpapervalid && vysion_settings.showdesktopwallpaper) gfx_ScaledSprite_NoClip(vysion_programdata.wallpaper, 0, 0, 2, 2);
                gfx_SetTextScale(2, 2);
                gfx_PrintStringXY("LOCKED", 160 - gfx_GetStringWidth("LOCKED") / 2, 105);
                gfx_SetTextScale(1, 1);
                gfx_PrintStringXY(str1, 160 - (gfx_GetStringWidth(str1) + (cursorblink > 9) * 9) / 2, 127);
                gfx_SwapDraw();
            }
            if (strcmp(optix_stringinput.text, vysion_settings.password) != 0) optix_Message("INCORRECT", "Incorrect password.", 12, 160, 4);
        }
    }
}


