//OPTIX version 2
//an open source GUI/text library in C for the TI-84 Plus CE and TI-83 Premium CE calculators
//please credit when used

#include <tice.h>
#include "optix.h"
#include "gfx/gfx.h"
/* Standard headers - it's recommended to leave them included */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
/* Other available headers */
// including stdarg.h, setjmp.h, assert.h, ctype.h, float.h, iso646.h, limits.h, errno.h
/* Available libraries */
// including lib/ce/graphc.h, lib/ce/fileioc.h, and lib/ce/keypadc.h
// Sample: #include <lib/ce/graphc.h>
#include <graphx.h>
#include <fileioc.h>
#include <keypadc.h>
#include <intce.h>
#include <tice.h>
#include <math.h>
struct optix_guicolors_t optix_guicolors;
struct optix_guidata_t optix_guidata;
struct optix_guisettings_t optix_guisettings;
struct optix_buttoninfo_t optix_buttoninfo;
struct optix_cursor_t optix_cursor;
struct optix_button_t *optix_button;
struct optix_box_t optix_box;
struct optix_wordwraptext_t *optix_wordwraptext;
struct optix_menu_t *optix_menu;
struct optix_stringinput_t optix_stringinput;
//char *optix_stringinput;

//these will be irrelevant when we can set custom colors
void optix_SetActiveMenus(int activemenus[], int nummenus) {
    optix_guidata.numactivemenus = nummenus;
    optix_guidata.activemenus = realloc(optix_guidata.activemenus, nummenus * sizeof(int *));
    memcpy(optix_guidata.activemenus, activemenus, (sizeof(optix_guidata.activemenus)) * nummenus);
}

void optix_ClearActiveMenus(void) {
    optix_guidata.activemenus = realloc(optix_guidata.activemenus, 0);
    optix_guidata.numactivemenus = 0;
}

void optix_SaveActiveMenus(void) {
    optix_guidata.backnumactivemenus = optix_guidata.numactivemenus;
    optix_guidata.backactivemenus = realloc(optix_guidata.backactivemenus, optix_guidata.backnumactivemenus * sizeof(int *));
    memcpy(optix_guidata.backactivemenus, optix_guidata.activemenus, (sizeof(optix_guidata.activemenus)) * optix_guidata.numactivemenus);
}

void optix_RestoreActiveMenus(void) {
    optix_guidata.numactivemenus = optix_guidata.backnumactivemenus;
    optix_guidata.activemenus = realloc(optix_guidata.activemenus, optix_guidata.backnumactivemenus * sizeof(int *));
    memcpy(optix_guidata.activemenus, optix_guidata.backactivemenus, (sizeof(optix_guidata.backactivemenus)) * optix_guidata.numactivemenus);
}

void optix_HandleGUI(void) {
    if (optix_guisettings.cursoron) {
        kb_Scan();
        //so apparently keypresses don't work 111anymore
        if (!kb_AnyKey()) optix_guidata.keypress = true;
        //gfx_FillScreen(optix_guicolors.bgcolor);
        //optix_RenderButtons();
        optix_HandleCursor();
        //optix_CheckForAltKey();
    }
}

void optix_HandleCursor(void) {
    //just going to take this out for now, it doesn't really work
    if (optix_cursor.trackpadactive) optix_HandleTrackPad();
    if (optix_guisettings.cursoron) {
        optix_UpdateCursor();
        optix_RenderCursor();
        //optix_ClickCursor();
    } else {
        //optix_UpdateSelectedButton();
        //optix_ClickButton();
    }
}

void optix_WhiText(void) {
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(255);
    gfx_SetTextTransparentColor(0);
}

void optix_BlaText(void) {
    gfx_SetTextBGColor(255);
    gfx_SetTextFGColor(0);
    gfx_SetTextTransparentColor(255);
}

void optix_CusText(bool title) {
    if (!title) {
        gfx_SetTextBGColor(optix_guicolors.textbgcolor);
        gfx_SetTextFGColor(optix_guicolors.textfgcolor);
        gfx_SetTextTransparentColor(optix_guicolors.textbgcolor);
    } else {
        gfx_SetTextBGColor(optix_guicolors.ttextbgcolor);
        gfx_SetTextFGColor(optix_guicolors.ttextfgcolor);
        gfx_SetTextTransparentColor(optix_guicolors.ttextbgcolor);
    }
}
/*
uint8_t bgcolor;
uint8_t colora;
uint8_t colorb;
uint8_t textfgcolor;
uint8_t textbgcolor;
uint8_t ttextfgcolor;
uint8_t ttextbgcolor;
*/
void optix_SetDefaultColors(void) {
    //whatever colors you want
    optix_guicolors.bgcolor = 0;
    optix_guicolors.colora = 0;
    optix_guicolors.colorb = 255;
    optix_guicolors.textfgcolor = 255;
    optix_guicolors.textbgcolor = 0;
    optix_guicolors.ttextfgcolor = 0;
    optix_guicolors.ttextbgcolor = 255;
    optix_guicolors.selectcolor = 18;
    optix_guicolors.outlinecolor = 74;
    //this shouldn't be here
    optix_cursor.back = gfx_MallocSprite(12, 12);
}


void optix_SetDefaultSettings(void) {
    optix_cursor.x = 0;
    optix_cursor.y = 0;
    optix_cursor.cursoractive = false;
    optix_guidata.trackpadlastx = 0;
    optix_guidata.trackpadlasty = 0;
    optix_guidata.trackpadoriginx = 0;
    optix_guidata.trackpadoriginy = 0;
    optix_guisettings.buttonsensitivity = 10;
    optix_buttoninfo.currbutton = 1;
    optix_guidata.keypress = true;
}

//could use a horizontal scroll bar for this too
void optix_BoolSwitch(bool on, uint16_t x, uint8_t y, uint8_t width, uint8_t height) {
    if (on) {
        gfx_SetColor(optix_guicolors.selectcolor);
        gfx_FillRectangle(x, y, width, height);
    }
    gfx_SetColor(optix_guicolors.colorb);
    gfx_Rectangle(x, y, width, height);
    //the actual slider thing
    gfx_FillRectangle(x + 1 + (on * (width / 2)), y + 1, width / 2 - 2, height - 2);
    gfx_SetColor(optix_guicolors.outlinecolor);
    gfx_Rectangle(x + 1 + (on * (width / 2)), y + 1, width / 2 - 2, height - 2);
}

void optix_VertScrollbar(uint24_t x, uint8_t y, uint8_t currpos, uint8_t maxpos, uint8_t width, uint8_t height, bool keypressed) {
    optix_Scrollbar(x, y, currpos, maxpos, width, height, keypressed, true);
}

void optix_HorizScrollbar(uint24_t x, uint8_t y, uint8_t currpos, uint8_t maxpos, uint24_t width, uint8_t height, bool keypressed) {
    optix_Scrollbar(x, y, currpos, maxpos, width, height, keypressed, false);
}

//I'll teach you kids a thing or two about bad code
void optix_Scrollbar(uint24_t x, uint8_t y, uint8_t currpos, uint8_t maxpos, uint24_t width, uint8_t height, bool keypressed, bool vert) {
    //because 0-3 is 4 entries not 3, for example
    maxpos++;
    gfx_SetColor(optix_guicolors.colora);
    gfx_FillRectangle(x, y, width, height);
    gfx_SetColor(optix_guicolors.colorb);
    gfx_Rectangle(x, y, width, height);
    if (keypressed) gfx_SetColor(optix_guicolors.selectcolor);
    if (currpos >= maxpos - 1) {
        if (vert) gfx_FillRectangle(x + 2, y + (height - 2) - (height - 4) / maxpos, width - 4, (height - 4) / maxpos);
        else gfx_FillRectangle(x + (width - 2) - (width - 4) / maxpos, y + 2, (width - 4) / maxpos, height - 4);
        gfx_SetColor(optix_guicolors.outlinecolor);
        if (vert) gfx_Rectangle(x + 2, y + (height - 2) - (height - 4) / maxpos, width - 4, (height - 4) / maxpos);
        else gfx_Rectangle(x + (width - 2) - (width - 4) / maxpos, y + 2, (width - 4) / maxpos, height - 4);
    } else {
        if (vert) gfx_FillRectangle(x + 2, (y + 2) + currpos * ((height - 4) / maxpos), width - 4, (height - 4) / maxpos);
        else gfx_FillRectangle((x + 2) + currpos * ((width - 4) / maxpos), y + 2, (width - 4) / maxpos, height - 4);
        gfx_SetColor(optix_guicolors.outlinecolor);
        if (vert) gfx_Rectangle(x + 2, (y + 2) + currpos * ((height - 4) / maxpos), width - 4, (height - 4) / maxpos);
        else gfx_Rectangle((x + 2) + currpos * ((width - 4) / maxpos), y + 2, (width - 4) / maxpos, height - 4);
    }
}


//note: height does not include the height of the title, which is by default 12
void optix_RenderWindow(const char title[], uint16_t width, uint8_t height) {
    uint8_t xprint;
    uint8_t yprint;
    uint8_t titlewidth;
    titlewidth = 12;
    xprint = 160 - width / 2;
    yprint = 120 - (height + titlewidth) / 2;
    gfx_SetColor(optix_guicolors.colora);
    gfx_FillRectangle(xprint, yprint + titlewidth, width, height);
    gfx_SetColor(optix_guicolors.colorb);
    gfx_Rectangle(xprint, yprint + titlewidth, width, height);
    //the title bar
    gfx_FillRectangle(xprint, yprint, width, titlewidth);
    optix_CusText(true);
    gfx_PrintStringXY(title, 160 - gfx_GetStringWidth(title) / 2, yprint + titlewidth / 2 - 3);
}




//this doesn't need to print anymore because another function will do it
uint8_t optix_WordWrap(const char message[], uint16_t width) {
    uint8_t numlines;
    uint16_t charnumber;
    char tempstr[100] = "";
    char linestr[100] = "";
    uint8_t tempchar;
    int stringlength;
    //uint8_t i = 0;
    //I don't want to talk about it okay
    //free(optix_wordwraptext);
    charnumber = 0;
    numlines = 0;
    stringlength = optix_GetStringLength(message);
    //retrying this
    //so we want to get words, and stop when we get to the end
    for ( ; ; ) {
        strcpy(tempstr, "");
        tempchar = 0;
        //get each new word
        while (message[charnumber] != " "[0]) {
            //if we reach the newline character immediately stop and start a new line
            //but put priority on keeping the words where they should be
            if ((message[charnumber] == "`"[0] && gfx_GetStringWidth(tempstr) + gfx_GetStringWidth(linestr) <= width)) {
                //shift everything back
                //put the stuff on a new line
                tempstr[tempchar] = '\0';
                strcat(linestr, tempstr);
                optix_AddWordWrapLine(linestr, &numlines);
                strcpy(linestr, "");
                strcpy(tempstr, "");
                break;
            }
            tempstr[tempchar] = message[charnumber];
            tempstr[tempchar + 1] = '\0';
            if (gfx_GetStringWidth(tempstr) >= width) {
                tempstr[tempchar] = '\0';
                charnumber--;
                optix_AddWordWrapLine(tempstr, &numlines);
                strcpy(linestr, "");
                strcpy(tempstr, "");
                break;
            }
            tempchar++;
            charnumber++;
            if (charnumber >= stringlength) break;
        }
        //so it was never moving on
        if (message[charnumber] != "`"[0]) tempstr[tempchar++] = " "[0];
        tempstr[tempchar++] = '\0';
        charnumber++;
        //check if the line width added to the word width is greater than the width of the window
        if (gfx_GetStringWidth(tempstr) + gfx_GetStringWidth(linestr) <= width) strcat(linestr, tempstr);
        //if it's greater that word becomes the new next line
        else {
            optix_AddWordWrapLine(linestr, &numlines);
            strcpy(linestr, tempstr);
        }
        if (charnumber >= stringlength) break;
    }
    optix_AddWordWrapLine(linestr, &numlines);
    //because
    return numlines;
}

void optix_AddWordWrapLine(const char string[], uint8_t *numlines) {
    if (optix_wordwraptext = realloc(optix_wordwraptext, (*numlines + 1) * sizeof(struct optix_wordwraptext_t))) {
        //thanks LLVM
        strcpy(optix_wordwraptext[*numlines].text, string);
        ++*numlines;
    }
}

void optix_PrintWordWrap(uint16_t width, uint8_t initialx, uint8_t initialy, uint8_t textspacing, uint8_t currline, uint8_t maxlines) {
    uint8_t i;
    uint8_t line = 0;
    char linestr[200];
    optix_CusText(false);
    for (i = 0; i < maxlines; i++) {
        if (line >= maxlines) break;
        strcpy(linestr, optix_wordwraptext[i + currline].text);
        if (linestr[0] != "~"[0]) gfx_PrintStringXY(linestr, initialx, initialy + line * textspacing);
        else if (maxlines - line >= 2) {
            linestr[0] = " "[0];
            //maybe later
            //gfx_SetTextFGColor(224);
            gfx_SetTextScale(2, 2);
            //add 4 here because that is the width of the space, meaning that the title would be shifted over 2 pixels to the right
            //doesn't really matter but could be irritating if anyone noticed
            //half of 16 is 8, add that to y to center vertically between lines
            gfx_PrintStringXY(linestr, (initialx + width / 2) - (gfx_GetStringWidth(linestr) + 4) / 2, (textspacing - 9) + initialy + line * textspacing);
            //CusText(false);
            gfx_SetTextScale(1, 1);
            //bigger text so taller
            line++;
        }
        line++;
    }
}



int optix_GetStringLength(const char string[]) {
    int stringlength;
    stringlength = 0;
    while (string[stringlength] != '\0') stringlength++;
    return stringlength;
}

//HOW TO USE: just call the function, giving it the title, your desired text spacing (in pixels), the width of the box (in pixels),
//and it will handle the rest, centering the box and wrapping the text for you, and now scrolling as well
//ANOTHER NOTE: Making new lines and titles within the box: You can use the characters "`" and "~" to start a new line and title a line,
//respectively. In order to start a new line, you must put the "`" directly in front of the word that you want the new line to start with:
//"Some text... `newline starts here". If you'd like to make a line centered and in a different color, as for a subheader, you can use "~".
//This is best coupled with "`", where it must come second (newline, then indicate you want that to be a title). Example: "Some text... `~This
//is a title! `more text"
void optix_Message(const char title[], const char message[], uint8_t textspacing, uint16_t width, uint8_t maxlines) {
    //change as necessary
    uint8_t xprint;
    uint8_t yprint;
    uint8_t numlines = 0;
    uint8_t currline;
    char temp[2] = " ";
    bool keypress;
    uint8_t titlewidth;
    uint8_t padding = 3;
    //let's fix this
    //gfx_Blit(1);
    currline = 0;
    keypress = true;
    titlewidth = 12;
    numlines = optix_WordWrap(message, width);
    if (numlines < maxlines) maxlines = numlines;
    xprint = 160 - (width + 2 * padding) / 2;
    //the top bar will always be 12 pixels tall
    yprint = 120 - (maxlines * textspacing + titlewidth + 2 * padding) / 2;
    while (kb_AnyKey()) kb_Scan();
    while (!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
        //handle scrolling
        if (numlines > maxlines) {
            if ((kb_Data[7] & kb_Up) && (currline > 0) && keypress) {
                currline--;
                keypress = false;
            }
            if ((kb_Data[7] & kb_Down) && (currline < numlines - maxlines) && keypress) {
                currline++;
                keypress = false;
            }
        }
        if (!kb_AnyKey()) keypress = true;
        //numlines++ because we need to display the title too
        //15 is the textspacing
        //yprint = 120 - maxlines * textspacing / 2;
        //make a box
        optix_RenderWindow(title, width + 2 * padding, maxlines * textspacing + (2 * padding));
        /*gfx_SetColor(optix_guicolors.colora);
        gfx_FillRectangle(xprint, yprint + titlewidth, width + 2 * padding, maxlines * textspacing + (2 * padding));
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(xprint, yprint + titlewidth, width + 2 * padding, maxlines * textspacing + (2 * padding));
        //gfx_Rectangle(xprint + 1, yprint + titlewidth + 1, (width + 2 * padding) - 2, (maxlines * textspacing + (2 * padding)) - 2);
        //make some triangles (not now though, they're a little off and I like the box better)
        /*gfx_FillTriangle(xprint - 5, yprint - 5 + textspacing, xprint - 5 + textspacing, yprint - 5 + textspacing, xprint - 5 + textspacing, yprint - 5);
        gfx_FillTriangle(xprint - 5 + width, yprint - 5 + textspacing, xprint - 5 + width - textspacing, yprint - 5 + textspacing, xprint - 5 + width - textspacing, yprint - 5);*/
        /*gfx_FillRectangle(xprint, yprint, width + 2 * padding, titlewidth);
        optix_CusText(true);
        gfx_PrintStringXY(title, 160 - gfx_GetStringWidth(title) / 2, yprint + (titlewidth / 2 - 3));*/
        //indicate that scrolling can be done if applicable
        if (numlines > maxlines) {
            if (currline > 0 && currline < numlines - maxlines) temp[0] = 18;
            else if (currline > 0) temp[0] = 24;
            else if (currline < numlines - maxlines) temp[0] = 25;
            gfx_PrintStringXY(temp, xprint + width + padding - gfx_GetStringWidth(temp), yprint + (titlewidth / 2 - 3));
        }
        //we still want the text to be in the right spot
        //yprint += textspacing + 2;
        optix_PrintWordWrap(width, xprint + 2 * padding, yprint + titlewidth + padding + 1, textspacing, currline, maxlines);
        gfx_SwapDraw();
    }
    while (kb_AnyKey()) kb_Scan();
}

void optix_Dialogue(const char title[], const char message[], uint8_t textspacing, uint16_t width, uint8_t maxlines) {
    //change as necessary
    uint8_t xprint;
    uint8_t yprint;
    uint8_t numlines = 0;
    uint8_t currline;
    char temp[2] = " ";
    bool keypress;
    uint8_t titlewidth;
    uint8_t padding = 3;
    //let's fix this
    //gfx_Blit(1);
    currline = 0;
    keypress = true;
    titlewidth = 12;
    numlines = optix_WordWrap(message, width);
    if (numlines < maxlines) maxlines = numlines;
    xprint = 160 - (width + 2 * padding) / 2;
    //the top bar will always be 12 pixels tall
    yprint = 120 - (maxlines * textspacing + titlewidth + 2 * padding) / 2;
    //numlines++ because we need to display the title too
    //15 is the textspacing
    //yprint = 120 - maxlines * textspacing / 2;
    //make a box
    gfx_SetColor(optix_guicolors.colora);
    optix_RenderWindow(title, width + 2 * padding, maxlines * textspacing + (2 * padding));
    /*gfx_FillRectangle(xprint, yprint + titlewidth, width + 2 * padding, maxlines * textspacing + (2 * padding));
    gfx_SetColor(optix_guicolors.colorb);
    gfx_Rectangle(xprint, yprint + titlewidth, width + 2 * padding, maxlines * textspacing + (2 * padding));
    //gfx_Rectangle(xprint + 1, yprint + titlewidth + 1, (width + 2 * padding) - 2, (maxlines * textspacing + (2 * padding)) - 2);
    //make some triangles (not now though, they're a little off and I like the box better)
    /*gfx_FillTriangle(xprint - 5, yprint - 5 + textspacing, xprint - 5 + textspacing, yprint - 5 + textspacing, xprint - 5 + textspacing, yprint - 5);
    gfx_FillTriangle(xprint - 5 + width, yprint - 5 + textspacing, xprint - 5 + width - textspacing, yprint - 5 + textspacing, xprint - 5 + width - textspacing, yprint - 5);*/
    /*gfx_FillRectangle(xprint, yprint, width + 2 * padding, titlewidth);
    optix_CusText(true);
    gfx_PrintStringXY(title, 160 - gfx_GetStringWidth(title) / 2, yprint + (titlewidth / 2 - 3));*/
    //indicate that scrolling can be done if applicable
    if (numlines > maxlines) {
        if (currline > 0 && currline < numlines - maxlines) temp[0] = 18;
        else if (currline > 0) temp[0] = 24;
        else if (currline < numlines - maxlines) temp[0] = 25;
        gfx_PrintStringXY(temp, xprint + width + padding - gfx_GetStringWidth(temp), yprint + (titlewidth / 2 - 3));
    }
    //we still want the text to be in the right spot
    //yprint += textspacing + 2;
    optix_PrintWordWrap(width, xprint + 2 * padding, yprint + titlewidth + padding + 1, textspacing, currline, maxlines);
}


//SYNTAX NOTES: specify your title as normal, as well as your text spacing and box width (in pixels), as for the message routine
//maxlines is how many lines will be displayed of the menu at one time, and if there are more entries in the menu than this number,
//it will allow you to scroll. Entries should be in the format of something like this: "Play`Statistics`Options`Reset`Game mode`Credits`About`Quit`",
//where the character "`" comes AFTER each entry
uint8_t optix_Menu(const char title[], const char entries[], uint8_t textspacing, uint16_t width, uint8_t maxlines) {
    //the new menu stuff will do it all for us
    struct optix_menu_t *m = &optix_menu[optix_guidata.currmenu];
    uint8_t xprint;
    uint8_t yprint;
    uint8_t numlines;
    uint8_t currline;
    uint8_t currentselection;
    uint8_t titlewidth = 12;
    char temp[2] = " ";
    bool enterpressed;
    int currmenu[] = {optix_guidata.nummenus};
    optix_SaveActiveMenus();
    optix_SetActiveMenus(currmenu, 1);
    //same here
    //gfx_Blit(1);
    numlines = optix_AddMenu(0, 0, 0, 0, 0, 0, 0, 0, entries, NULL);
    if (numlines < maxlines) maxlines = numlines;
    optix_DeleteLastMenu();
    currline = 0;
    currentselection = 0;
    enterpressed = false;
    xprint = 160 - width / 2;
    yprint = 120 - (titlewidth + maxlines * textspacing) / 2;
    optix_AddMenu(xprint, yprint + titlewidth, 0, 0, 1, maxlines, width, textspacing, entries, NULL);
    optix_guidata.currmenu = optix_guidata.nummenus - 1;
    m = &optix_menu[optix_guidata.currmenu];
    while (kb_AnyKey()) kb_Scan();
    while ((!m->enterpressed) || (kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) {
        optix_UpdateCurrMenu();
        gfx_SetColor(optix_guicolors.colora);
        gfx_FillRectangle(xprint - 1, yprint + titlewidth - 1, width + 2, maxlines * textspacing + 2);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(xprint - 1, yprint + titlewidth - 1, width + 2, maxlines * textspacing + 2);
        //gfx_Rectangle(xprint + 1, yprint + titlewidth + 1, (width + 2 * padding) - 2, (maxlines * textspacing + (2 * padding)) - 2);
        //make some triangles (not now though, they're a little off and I like the box better)
        /*gfx_FillTriangle(xprint - 5, yprint - 5 + textspacing, xprint - 5 + textspacing, yprint - 5 + textspacing, xprint - 5 + textspacing, yprint - 5);
        gfx_FillTriangle(xprint - 5 + width, yprint - 5 + textspacing, xprint - 5 + width - textspacing, yprint - 5 + textspacing, xprint - 5 + width - textspacing, yprint - 5);*/
        gfx_FillRectangle(xprint - 1, yprint - 1, width + 2, titlewidth);
        optix_CusText(true);
        gfx_PrintStringXY(title, 160 - gfx_GetStringWidth(title) / 2, yprint + (titlewidth / 2 - 4));
        //indicate that scrolling can be done if applicable
        if (numlines > maxlines) {
            if (m->menumin > 0 && m->menumin < m->numoptions - maxlines) temp[0] = 18;
            else if (m->menumin > 0) temp[0] = 24;
            else if (m->menumin < m->numoptions - maxlines) temp[0] = 25;
            gfx_PrintStringXY(temp, xprint + width - gfx_GetStringWidth(temp), yprint + (titlewidth / 2 - 4));
        }
        optix_RenderMenu(optix_guidata.nummenus - 1);
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    currentselection = m->currselection;
    optix_DeleteLastMenu();
    optix_RestoreActiveMenus();
    optix_guidata.currmenu--;
    return currentselection;
}

int optix_Slider(uint16_t x, uint8_t y, uint16_t width, uint8_t height, int min, int max, int initial) {
    char str1[20];
    //make it start in the middle
    int returnvalue = (min + max) / 2;
    bool enterpressed = false;
    int loopsdown;
    if (initial != -1) returnvalue = initial;
    while (!enterpressed || (kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) {
        kb_Scan();
        //make sure you can't scroll while holding enter
        if (!enterpressed) {
            if (kb_Data[7] & kb_Left && optix_guidata.keypress && returnvalue > min) {
                returnvalue--;
                optix_guidata.keypress = false;
                loopsdown = 0;
            } else if (kb_Data[7] & kb_Left && returnvalue > min) {
                loopsdown++;
                if (loopsdown > 25) returnvalue -= (loopsdown % 2 == 0);
            }
            if (kb_Data[7] & kb_Right && optix_guidata.keypress && returnvalue < max) {
                returnvalue++;
                optix_guidata.keypress = false;
                loopsdown = 0;
            } else if (kb_Data[7] & kb_Right && returnvalue < max) {
                loopsdown++;
                if (loopsdown > 25) returnvalue += (loopsdown % 2 == 0);
            }
        }
        if ((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) enterpressed = true;
        if (!kb_AnyKey()) optix_guidata.keypress = true;
        //make the string
        strcpy(str1, "");
        if (returnvalue > min) sprintf(str1, "< %d", returnvalue);
        else sprintf(str1, "%d", returnvalue);
        if (returnvalue < max) strcat(str1, " >");
        if (!enterpressed) gfx_SetColor(optix_guicolors.colorb);
        else gfx_SetColor(optix_guicolors.selectcolor);
        gfx_FillRectangle(x, y, width, height);
        gfx_SetColor(optix_guicolors.outlinecolor);
        gfx_Rectangle(x + 1, y + 1, width - 2, height - 2);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(x, y, width, height);
        optix_CusText(!enterpressed);
        gfx_PrintStringXY(str1, (x + width / 2) - gfx_GetStringWidth(str1) / 2, (y + height / 2) - 4);
        gfx_SwapDraw();
    }
    return returnvalue;
}


int optix_SimpleMenu(uint16_t x, uint8_t y, uint16_t xspacing, uint8_t yspacing, const char *entries) {
    int numlines;
    struct optix_menu_t *m;
    int currselection;
    int currmenu[] = {optix_guidata.nummenus};
    optix_SaveActiveMenus();
    optix_SetActiveMenus(currmenu, 1);
    numlines = optix_AddMenu(x, y, 0, 0, 1, 100000, xspacing, yspacing, entries, NULL);
    optix_DeleteLastMenu();
    optix_AddMenu(x, y, 0, 0, 1, numlines, xspacing, yspacing, entries, NULL);
    optix_guidata.currmenu = optix_guidata.nummenus - 1;
    m = &optix_menu[optix_guidata.currmenu];
    while ((!m->enterpressed) || (kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd)) {
        optix_UpdateCurrMenu();
        //GUI stuff
        gfx_SetColor(optix_guicolors.colora);
        gfx_FillRectangle(m->x - 1, m->y - 1, m->xspacing + 2, m->yspacing * m->numoptions + 2);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(m->x - 1, m->y - 1, m->xspacing + 2, m->yspacing * m->numoptions + 2);
        optix_RenderMenu(optix_guidata.nummenus - 1);
        gfx_SwapDraw();
        optix_HandleGUI();
    }
    currselection = m->currselection;
    optix_DeleteLastMenu();
    optix_guidata.currmenu--;
    optix_RestoreActiveMenus();
    return currselection;
}


//old
/*uint8_t optix_DispMenu(const char entries[], uint16_t width, bool print, uint8_t initialx, uint8_t initialy, uint8_t textspacing, uint8_t currline, uint8_t linestoprint, uint8_t currentselection) {
    uint8_t numlines;
    uint16_t charnumber;
    char tempstr[100] = " ";
    uint8_t tempchar;
    int stringlength;
    charnumber = 0;
    numlines = 0;
    stringlength = optix_GetStringLength(entries);
    linestoprint -= 3;
    while ((charnumber < stringlength) && ((numlines <= currline + linestoprint + 1) || (!print))) {
        tempchar = 0;
        strcpy(tempstr, " ");
        while (entries[charnumber] != "`"[0]) {
            tempstr[tempchar] = entries[charnumber];
            tempchar++;
            charnumber++;
            if (charnumber > stringlength) break;
        }
        charnumber++;
        tempstr[tempchar] = '\0';
        numlines++;
        if ((print) && (numlines > currline)) {
            if (numlines - 1 == currentselection) optix_CusText(true);
            else optix_CusText(false);
            gfx_PrintStringXY(tempstr, (initialx + width / 2) - gfx_GetStringWidth(tempstr) / 2, initialy + (numlines - currline - 1) * textspacing);
        }
    }
    //if (print) gfx_PrintStringXY(tempstr, (initialx + width / 2) - gfx_GetStringWidth(tempstr) / 2, initialy + (numlines - currline) * textspacing);
    //numlines++;
    return numlines;
}*/

void optix_SetAdvancedMenuCentering(uint8_t menu, uint8_t textxcentering, uint8_t textycentering, uint8_t sprxcentering, uint8_t sprycentering, uint8_t textxoffset,
                                    uint8_t textyoffset, uint8_t sprxoffset, uint8_t spryoffset) {
    struct optix_menu_t *m = &optix_menu[menu];
    m->textxcentering = textxcentering;
    m->textycentering = textycentering;
    m->sprxcentering = sprxcentering;
    m->sprycentering = sprycentering;
    m->textxoffset = textxoffset;
    m->textyoffset = textyoffset;
    m->sprxoffset = sprxoffset;
    m->spryoffset = spryoffset;
}

void optix_SetMenuSprScale(uint8_t menu, uint8_t sprxscale, uint8_t spryscale) {
    struct optix_menu_t *m = &optix_menu[menu];
    m->sprxscale = sprxscale;
    m->spryscale = spryscale;
}

//function for aligning menus to windows
//same centerings as before: 0 = left, 1 = centered, 2 = right aligned
uint16_t optix_GetWindowXCentering(uint8_t centering, uint16_t windowwidth, uint16_t width, uint16_t xspacing) {
    return (((windowwidth - width * xspacing) / 2) * centering) + (160 - windowwidth / 2);
}

uint16_t optix_GetWindowYCentering(uint8_t centering, uint16_t windowheight, uint16_t height, uint16_t yspacing) {
    return (((windowheight - height * yspacing) / 2) * centering) + (120 - windowheight / 2) + 6;
    //+ 6 because we're assuming the title bar will be 12 px tall, and 12/2 == 6
}


int optix_AddMenu(uint16_t x, uint8_t y, uint8_t xpadding, uint8_t ypadding, uint8_t width, uint8_t height, uint8_t xspacing, uint8_t yspacing,
                  const char text[], gfx_sprite_t **sprites) {
    int numlines;
    uint16_t charnumber;
    char tempstr[100] = " ";
    uint8_t tempchar;
    int stringlength;
    struct optix_menu_t *m;
    if ((bool) (optix_menu = realloc(optix_menu, ++optix_guidata.nummenus * sizeof(struct optix_menu_t)))) {
        m = &optix_menu[optix_guidata.nummenus - 1];
        m->width = width;
        m->height = height;
        m->xspacing = xspacing;
        m->yspacing = yspacing;
        m->menumin = 0;
        m->x = x;
        m->y = y;
        m->xpadding = xpadding;
        m->ypadding = ypadding;
        m->enterpressed = false;
        m->currselection = 0;
        m->text = NULL;
        m->textxcentering = m->textycentering = m->sprxcentering = m->sprycentering = m->sprxscale = m->spryscale =  1;
        m->textxoffset = m->textyoffset = m->sprxoffset = m->spryoffset = 0;
        m->icon = sprites;
        m->displaytext = true;
        //free(m->text);
        charnumber = 0;
        numlines = 0;
        stringlength = optix_GetStringLength(text);
        while (charnumber < stringlength) {
            tempchar = 0;
            strcpy(tempstr, " ");
            while (text[charnumber] != "`"[0]) {
                tempstr[tempchar] = text[charnumber];
                tempchar++;
                charnumber++;
                if (charnumber > stringlength) break;
            }
            charnumber++;
            tempstr[tempchar] = '\0';
            //add it to the text
            numlines++;
            //realloc the array
            m->text = realloc(m->text, numlines * 3);
            //realloc the const char pointer
            m->text[numlines - 1] = malloc(tempchar + 1);
            strcpy(m->text[numlines - 1], tempstr);
        }
        m->numoptions = numlines;
        return numlines;
    } else {
        free(optix_menu);
        return -1;
    }
}

void optix_UpdateCurrMenu(void) {
    struct optix_menu_t *m = &optix_menu[optix_guidata.currmenu];
    int attoption = -1;
    int i;
    int attx;
    int atty;
    if (optix_guisettings.cursoron) {
        kb_Scan();
        optix_guidata.currmenu = OPTIX_MENU_INVALID;
        for (i = 0; i < optix_guidata.numactivemenus; i++) {
            m = &optix_menu[optix_guidata.activemenus[i]];
            if (optix_cursor.x > m->x && optix_cursor.y > m->y && optix_cursor.x - m->x < m->width * m->xspacing && optix_cursor.y - m->y < m->height * m->yspacing) {
                attx = (optix_cursor.x - m->x) / m->xspacing;
                atty = (optix_cursor.y - m->y) / m->yspacing;
                attoption = atty * m->width + attx + m->menumin;
            } else continue;
            if (attoption >= 0 && attoption < m->numoptions) {
                optix_guidata.currmenu = optix_guidata.activemenus[i];
                m->currselection = attoption;
                break;
            } else m->currselection = OPTIX_MENU_INVALID;
        }
        if (optix_guidata.currmenu != OPTIX_MENU_INVALID) {
            //m = &optix_menu[optix_guidata.currmenu];
            if (kb_Data[1] & kb_Graph && optix_guidata.keypress) {
                if (m->menumin / m->width < ((m->numoptions > m->width * m->height) * ((m->numoptions + 1) / m->height - m->width)) ||
                m->menumin / m->width < ((m->numoptions > m->width * m->height) * ((m->numoptions + 1) / m->width - m->height))) {
                    //gfx_FillScreen(224);
                    //gfx_SwapDraw();
                    if (m->width == 1 || m->height == 1) {
                        m->menumin++;
                        //safety net
                        if (m->menumin > m->numoptions - (m->width * m->height)) m->menumin--;
                    } else m->menumin += m->width;
                    optix_guidata.keypress = false;
                }
            }
            if (kb_Data[1] & kb_Trace && optix_guidata.keypress) {
                if (m->menumin - m->height >= 0 || m->menumin - m->width >= 0) {
                    if (m->width == 1 || m->height == 1) m->menumin--;
                    else m->menumin -= m->width;
                    optix_guidata.keypress = false;
                }
            }
        }
    } else {
        kb_Scan();
        if (!kb_AnyKey()) timer_1_Counter = 0;
        //fix this in case we have to later
        if (m->currselection >= m->numoptions) m->currselection = m->numoptions - 1;
        if (kb_Data[7] & kb_Up && ((optix_guidata.keypress && m->height > 1) || timer_1_Counter > OPTIX_TICKS_SCROLL)) {
            attoption = m->currselection - m->width;
            optix_guidata.keypress = false;
        }
        if (kb_Data[7] & kb_Down && ((optix_guidata.keypress && m->height > 1) || timer_1_Counter > OPTIX_TICKS_SCROLL)) {
            attoption = m->currselection + m->width;
            optix_guidata.keypress = false;
        }
        if (kb_Data[7] & kb_Left && (optix_guidata.keypress || timer_1_Counter > OPTIX_TICKS_SCROLL)) {
            if ((m->currselection) % m->width > 0 || m->height == 1) attoption = m->currselection - 1;
            optix_guidata.keypress = false;
        }
        if (kb_Data[7] & kb_Right && (optix_guidata.keypress || timer_1_Counter > OPTIX_TICKS_SCROLL)) {
            //check if it's not one of the two ones on the edge
            if (m->currselection % m->width < m->width - 1 || m->height == 1) attoption = m->currselection + 1;
            optix_guidata.keypress = false;
        }
        if (timer_1_Counter > OPTIX_TICKS_SCROLL) timer_1_Counter = 0;
        //else m->enterpressed = false;
        if (attoption >= 0) m->currselection = attoption;
        //if you're trying to get on an option that's not on the menu
        if (attoption >= m->numoptions) m->currselection = m->numoptions - 1;
        //handle scrolling
        if (m->currselection > m->menumin + (m->width * m->height - 1)) {
            if (m->width == 1 || m->height == 1) m->menumin++;
            else m->menumin += m->width;
            //m->menumin += m->width;
        }
        else if (m->currselection < m->menumin) {
            if (m->width == 1 || m->height == 1) m->menumin--;
            else m->menumin -= m->width;
            //m->menumin -= m->width;
        }
    }
    m = &optix_menu[optix_guidata.currmenu];
    if ((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd) && optix_guidata.currmenu != OPTIX_MENU_INVALID && m->currselection != OPTIX_MENU_INVALID) {
        m->enterpressed = true;
        //make sure you can't move after you press it
        optix_guidata.keypress = false;
    }
    if (optix_guidata.currmenu == OPTIX_MENU_INVALID) m->enterpressed = false;
    //start the timer again
    //we're assuming that updatecurrmenu() will be called before rendermenu()
    if (!kb_AnyKey()) optix_guidata.keypress = true;
}

void optix_RenderMenu(uint8_t menu) {
    struct optix_menu_t *m = &optix_menu[menu];
    int width;
    int height;
    int iteration;
    //let's figure out the centering stuff
    uint16_t textx;
    uint8_t texty;
    uint16_t sprx;
    uint8_t spry;
    iteration = m->menumin;
    for (height = 0; height < m->height; height++) {
        for (width = 0; width < m->width; width++) {
            if (iteration >= m->numoptions) break;
            if (m->displaytext) {
                textx = (m->xspacing - gfx_GetStringWidth(m->text[iteration])) / 2;
                texty = (m->yspacing - 8) / 2;
            }
            if (m->icon[iteration] != NULL && m->icon != NULL) {
                sprx = (m->xspacing - m->icon[iteration]->width * m->sprxscale) / 2;
                spry = (m->yspacing - m->icon[iteration]->height * m->spryscale) / 2;
            }
            if (iteration == m->currselection && menu == optix_guidata.currmenu) {
                if (m->enterpressed) {
                    gfx_SetColor(optix_guicolors.selectcolor);
                    optix_CusText(false);
                } else {
                    gfx_SetColor(optix_guicolors.colorb);
                    optix_CusText(true);
                }
                gfx_FillRectangle(width * m->xspacing + m->x + m->xpadding, height * m->yspacing + m->y + m->ypadding, m->xspacing - 2 * m->xpadding, m->yspacing - 2 * m->ypadding);
                gfx_SetColor(optix_guicolors.outlinecolor);
                gfx_Rectangle(width * m->xspacing + m->x + m->xpadding, height * m->yspacing + m->y + m->ypadding, m->xspacing - 2 * m->xpadding, m->yspacing - 2 * m->ypadding);
            } else optix_CusText(false);
            //gfx_PrintStringXY(m->text[iteration], m->x + (width * m->xspacing + m->xspacing / 2) - gfx_GetStringWidth(m->text[iteration]) / 2, m->y + height * m->yspacing + (m->yspacing / 2) - 4);
            if (m->displaytext) gfx_PrintStringXY(m->text[iteration], m->x + width * m->xspacing + textx * m->textxcentering + m->textxoffset, m->y + height * m->yspacing + texty * m->textycentering + m->textyoffset);
            if (m->icon[iteration] != NULL && m->icon != NULL) {
                //display the sprite
                gfx_ScaledSprite_NoClip(m->icon[iteration], m->x + width * m->xspacing + sprx * m->sprxcentering + m->sprxoffset, m->y + height * m->yspacing + spry * m->sprycentering + m->spryoffset, m->sprxscale, m->spryscale);
            }

            iteration++;
            //gfx_PrintStringXY(m->text[height * m->width + width], (width * m->xspacing - m->xspacing) - gfx_GetStringWidth(m->text[m->width * height + width]) / 2, height * m->yspacing + 4);
        }
    }
}

void optix_DeleteLastMenu(void) {
    struct optix_menu_t *m;
    uint8_t i = 0;
    if (optix_guidata.nummenus > 0) {
        m = &optix_menu[--optix_guidata.nummenus];
        for (i = 0; i < m->numoptions; i++) free(m->text[i]);
        free(m->text);
        //if (optix_guidata.nummenus > 1)
        optix_menu = realloc(optix_menu, optix_guidata.nummenus * sizeof(struct optix_menu_t));
        //else free(optix_menu);
        //just to make sure
        //optix_guidata.currmenu--;
    }
}

void optix_DeleteAllMenus(void) {
    while (optix_guidata.nummenus != 0) optix_DeleteLastMenu();
}

//cool
uint8_t optix_ColorPicker(int initial) {
    int i;
    uint8_t color;
    bool keypress;
    color = 0;
    if (initial != -1) color = initial;
    while (!((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd))) {
        if ((kb_Data[7] & kb_Up) && (keypress) && (color > 15)) {
            color -= 16;
            keypress = false;
        }
        if ((kb_Data[7] & kb_Down) && (keypress) && (color < 240)) {
            color += 16;
            keypress = false;
        }
        if ((kb_Data[7] & kb_Left) && (keypress) && (color > 0)) {
            color--;
            keypress = false;
        }
        if ((kb_Data[7] & kb_Right) && (keypress) && (color < 255)) {
            color++;
            keypress = false;
        }
        if (!kb_AnyKey()) keypress = true;
        //render the window
        //let's make it slightly taller to show the color at the bottom
        //there are 256 possible colors, so 16 x 16
        optix_RenderWindow("Color picker", 16 * 8 + 2, 16 * 8 + 12 + 2);
        for (i = 0; i < 256; i++) {
            gfx_SetColor(i);
            gfx_FillRectangle(optix_GetWindowXCentering(0, 16 * 8, 16, 8) + (i % 16) * 8, optix_GetWindowYCentering(0, 16 * 8 + 12, 16, 8) + (i / 16) * 8, 8, 8);
            if (i == color) {
                if (optix_guicolors.colorb == color) gfx_SetColor(optix_guicolors.colora);
                else gfx_SetColor(optix_guicolors.colorb);
                gfx_Rectangle(optix_GetWindowXCentering(0, 16 * 8, 16, 8) + (i % 16) * 8, optix_GetWindowYCentering(0, 16 * 8 + 12, 16, 8) + (i / 16) * 8, 8, 8);
                gfx_Rectangle(optix_GetWindowXCentering(0, 16 * 8, 16, 8) + (i % 16) * 8 + 1, optix_GetWindowYCentering(0, 16 * 8 + 12, 16, 8) + (i / 16) * 8 + 1, 6, 6);
            }
        }
        gfx_SetColor(color);
        gfx_FillRectangle(optix_GetWindowXCentering(0, 16 * 8, 16, 8), optix_GetWindowYCentering(0, 16 * 8 + 12, 16, 8) + 16 * 8, 16 * 8, 12);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_HorizLine(optix_GetWindowXCentering(0, 16 * 8, 16, 8), optix_GetWindowYCentering(0, 16 * 8 + 12, 16, 8) + 16 * 8, 16 * 8);
        //some sort of indication of what we're doing
        gfx_SetTextXY(optix_GetWindowXCentering(0, 16 * 8, 16, 8) + 3, optix_GetWindowYCentering(0, 16 * 8 + 12, 16, 8) + 16 * 8 + 3);
        if (optix_guicolors.textfgcolor == color) optix_CusText(true);
        else optix_CusText(false);
        gfx_PrintUInt(color, 1);
        gfx_SwapDraw();
    }
    while (kb_AnyKey()) kb_Scan();
    return color;
}


//string input stuff
uint8_t optix_InsertSpecialCharacter(void) {
    uint8_t x;
    uint8_t y;
    uint8_t character;
    uint8_t currentx;
    uint8_t currenty;
    bool keypress;
    //gfx_sprite_t *oldback = gfx_MallocSprite(255, 240);
    optix_CusText(false);
    character = 0;
    x = 0;
    y = 0;
    currentx = 0;
    currenty = 0;
    gfx_Blit(0);
    //gfx_GetSprite(oldback, 65, 0);
    while (!((kb_Data[6] & kb_Enter) || (kb_Data[1] & kb_2nd))) {
        gfx_FillScreen(optix_guicolors.bgcolor);
        kb_Scan();
        if ((kb_Data[7] & kb_Up) && (keypress) && (currenty > 0)) {
            currenty--;
            keypress = false;
        }
        if ((kb_Data[7] & kb_Down) && (keypress) && (currenty < 7)) {
            currenty++;
            keypress = false;
        }

        if ((kb_Data[7] & kb_Left) && (keypress) && (currentx > 0)) {
            currentx--;
            keypress = false;
        }
        if ((kb_Data[7] & kb_Right) && (keypress) && (currentx < 15)) {
            currentx++;
            keypress = false;
        }
        if ((kb_Data[6] & kb_Clear) && (keypress)) {
            character = '\0';
            break;
        }
        if (!kb_AnyKey()) keypress = true;
        gfx_SetColor(optix_guicolors.colora);
        gfx_FillRectangle(77, 83, 166, 86);
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(77, 83, 166, 86);
        gfx_Rectangle(currentx * 10 + 79, currenty * 10 + 85, 12, 12);
        gfx_FillRectangle(77, 71, 166, 12);
        optix_CusText(false);
        for (y = 0; y < 8; y++) {
            for (x = 0; x < 16; x++) {
                character = y * 16 + x;
                gfx_SetTextXY(x * 10 + 81, y * 10 + 87);
                gfx_PrintChar(character);
            }
        }
        gfx_SetColor(optix_guicolors.colorb);
        gfx_FillRectangle(currentx * 10 + 80, currenty * 10 + 86, 10, 10);
        optix_CusText(true);
        //the title
        gfx_PrintStringXY("Special characters", 160 - gfx_GetStringWidth("Special characters") / 2, 74);


        gfx_SetTextXY(currentx * 10 + 81, currenty * 10 + 87);
        character = currenty * 16 + currentx;
        gfx_PrintChar(character);
        gfx_SwapDraw();
    }
    while (kb_Data[6] & kb_Enter) {
        kb_Scan();
        gfx_SetColor(optix_guicolors.selectcolor);
        gfx_FillRectangle(currentx * 10 + 80, currenty * 10 + 86, 10, 10);
        optix_CusText(true);
        gfx_SetTextXY(currentx * 10 + 81, currenty * 10 + 87);
        character = currenty * 16 + currentx;
        gfx_PrintChar(character);
        gfx_SwapDraw();
    }
    //check if this is one of the characters that OPTIX uses for word wrapping (just in case)
    if (character == "`"[0] || character == "~"[0]) {
        optix_Message("ERROR", "You attempted to use a character reserved by OPTIX. Nice try, bucko.", 10, 150, 10);
        character = '\0';
    }
    //gfx_Sprite(oldback, 65, 0);
    //free(oldback);
    gfx_Blit(1);
    return character;
}

//this function is stupid so just be careful (just will add characters to the end, or clear/delete them as it sees fit)
bool optix_UpdateStringInput(bool allowadd) {
    const char *keys[3] = {"\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0?\0VQLG\0\0.ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0\0",
                           "\0\0\0\0\0\0\0\0\0\0\0wrmh\0\0?\0vqlg\0\0.zupkfc\0 ytojeb\0\0xsnida\0\0\0\0\0\0\0\0\0",
                           "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0-\x33\x36\x39)\0\0\0.\x32\x35\x38(\0\0\0\x30\x31\x34\x37,\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                          };
    uint8_t skKey;
    //was a character pressed?
    bool returnvalue = false;
    skKey = os_GetCSC();
    if (keys[optix_stringinput.currmap][skKey] && allowadd) {
        //make the pointer's memory one bigger
        optix_stringinput.text = realloc(optix_stringinput.text, ++optix_stringinput.numchars);
        //because null terminator
        optix_stringinput.text[optix_stringinput.numchars - 2] = keys[optix_stringinput.currmap][skKey];
        returnvalue = true;
    }
    if ((skKey == sk_Del) && (optix_stringinput.numchars > 1)) {
        optix_stringinput.text = realloc(optix_stringinput.text, --optix_stringinput.numchars);
        optix_stringinput.text[optix_stringinput.numchars - 1] = '\0';
        returnvalue = true;
    }
    if (skKey == sk_Clear) {
        optix_ResetStringInput();
        returnvalue = true;
    }
    if (skKey == sk_Add && allowadd) {
        //make the pointer's memory one bigger
        optix_stringinput.text = realloc(optix_stringinput.text, ++optix_stringinput.numchars);
        //because null terminator
        optix_stringinput.text[optix_stringinput.numchars - 2] = optix_InsertSpecialCharacter();
        returnvalue = true;
    }
    if (skKey == sk_Alpha) optix_stringinput.currmap++;
    if (optix_stringinput.currmap > 2) optix_stringinput.currmap = 0;
    optix_stringinput.text[optix_stringinput.numchars - 1] = '\0';
    return returnvalue;
}

void optix_ResetStringInput(void) {
    //needs to start at 1 because null terminator
    optix_stringinput.text = realloc(optix_stringinput.text, 1);
    optix_stringinput.text[0] = '\0';
    optix_stringinput.numchars = 1;
    optix_stringinput.currmap = 0;
}

char *optix_GetStringInput(const char title[], uint8_t textspacing, uint16_t width, int maxchars) {
    //change as necessary
    const char *keys[3] = {"\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0?\0VQLG\0\0.ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0\0",
                           "\0\0\0\0\0\0\0\0\0\0\0wrmh\0\0?\0vqlg\0\0.zupkfc\0 ytojeb\0\0xsnida\0\0\0\0\0\0\0\0\0",
                           "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0-\x33\x36\x39)\0\0\0.\x32\x35\x38(\0\0\0\x30\x31\x34\x37,\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                          };
    uint8_t xprint;
    uint8_t yprint;
    uint8_t numlines;
    uint8_t currline;
    char temp[2] = "";
    char limit[20] = "";
    bool keypress;
    uint8_t titlewidth;
    uint8_t padding = 3;
    //basically the char number
    int i;
    uint8_t skKey;
    //uppercase keys
    uint8_t currmap = 0;
    uint8_t cursorblink;
    //free(optix_stringinput);
    cursorblink = 0;
    skKey = 0;
    i = 0;
    //free(optix_wordwraptext);
    //null terminating byte is a thing
    //strcpy(optix_stringinput, "");
    //strcpy(optix_wordwraptext[0].text, "");
    optix_WordWrap("", width);
    currline = 0;
    keypress = true;
    titlewidth = 12;
    numlines = 1;
    xprint = 160 - (width + 2 * padding) / 2;
    //the top bar will always be 12 pixels tall
    yprint = 120 - (numlines * textspacing + titlewidth + 2 * padding) / 2;
    optix_ResetStringInput();
    while (kb_AnyKey()) kb_Scan();
    while (!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
        //if (kb_AnyKey() && optix_stringinput.numchars < maxchars + 1) {
        if (optix_UpdateStringInput(optix_stringinput.numchars < maxchars + 1)) {
            cursorblink = 0;
            numlines = optix_WordWrap(optix_stringinput.text, width);
        }
        //}
        //gfx_FillScreen(optix_guicolors.bgcolor);
        optix_RenderWindow(title, width + 2 * padding, numlines * textspacing + (2 * padding));
        /*gfx_SetColor(optix_guicolors.colora);
        gfx_FillRectangle(xprint, yprint + titlewidth, width + 2 * padding, numlines * textspacing + (2 * padding));
        gfx_SetColor(optix_guicolors.colorb);
        gfx_Rectangle(xprint, yprint + titlewidth, width + 2 * padding, numlines * textspacing + (2 * padding));
        gfx_FillRectangle(xprint, yprint, width + 2 * padding, titlewidth);*/
        //a cursor
        if (cursorblink < 10) {
            gfx_VertLine(xprint + 2 * padding + gfx_GetStringWidth(optix_wordwraptext[numlines - 1].text) - 2, yprint + titlewidth + padding + (numlines - 1) * textspacing - 1, 11);
            gfx_VertLine(xprint + 2 * padding + gfx_GetStringWidth(optix_wordwraptext[numlines - 1].text) - 1, yprint + titlewidth + padding + (numlines - 1) * textspacing - 1, 11);
        }
        cursorblink++;
        if (cursorblink > 20) cursorblink = 0;
        optix_CusText(true);
        //gfx_PrintStringXY(title, 160 - gfx_GetStringWidth(title) / 2, yprint + (titlewidth / 2 - 3));
        sprintf(limit, "%d/%d", optix_stringinput.numchars - 1, maxchars);
        switch (optix_stringinput.currmap) {
        case 0:
            temp[0] = 'A';
            break;
        case 1:
            temp[0] = 'a';
            break;
        case 2:
            temp[0] = '1';
            break;
        default:
            temp[0] = '?';
            break;
        }
        gfx_PrintStringXY(limit, xprint + 2 * padding, yprint + (titlewidth / 2 - 3));
        gfx_PrintStringXY(temp, xprint + width + padding - gfx_GetStringWidth(temp), yprint + (titlewidth / 2 - 3));
        optix_PrintWordWrap(width, xprint + 2 * padding, yprint + titlewidth + padding + 1, textspacing, currline, numlines);
        gfx_SwapDraw();
    }
    while (kb_AnyKey()) kb_Scan();
    return optix_stringinput.text;
}



void optix_UpdateCursor(void) {
    kb_Scan();
    //gfx_SetDraw(0);
    //gfx_Sprite(optix_cursor.back, optix_cursor.x - 6, optix_cursor.y - 6);
    gfx_BlitRectangle(0, optix_cursor.x - 8, optix_cursor.y - 8, 16, 16);
    gfx_BlitRectangle(0, 214, 228, 84, 12);
    if ((kb_Data[7] & kb_Left) && (optix_cursor.x > 12)) optix_cursor.x -= 4;
    if ((kb_Data[7] & kb_Right) && (optix_cursor.x < 308)) optix_cursor.x += 4;
    if ((kb_Data[7] & kb_Up) && (optix_cursor.y > 12)) optix_cursor.y -= 4;
    if ((kb_Data[7] & kb_Down) && (optix_cursor.y < 228)) optix_cursor.y += 4;
    if (optix_cursor.x > 320) optix_cursor.x = 320;
    if (optix_cursor.y > 240) optix_cursor.y = 240;
    gfx_SetDraw(1);
}

void optix_RenderCursor(void) {
    struct optix_menu_t *m = &optix_menu[optix_guidata.currmenu];
    //gfx_SetDraw(1);
    //gfx_GetSprite(optix_cursor.back, optix_cursor.x - 6, optix_cursor.y - 6);
    //gfx_BlitRectangle(1, optix_cursor.x - 6, optix_cursor.y - 6, 12, 12);
    gfx_SetDraw(0);
    if (optix_guidata.currmenu != OPTIX_MENU_INVALID) {
        //this is a beastly if statement
        if (m->menumin / m->width < ((m->numoptions > m->width * m->height) * ((m->numoptions + 1) / m->height - m->width)) ||
        m->menumin / m->width < ((m->numoptions > m->width * m->height) * ((m->numoptions + 1) / m->width - m->height))
        || m->menumin - m->height >= 0 || m->menumin - m->width >= 0) {
            gfx_SetColor(optix_guicolors.colora);
            //one arrow at 288 and another at 224
            gfx_FillRectangle(214, 228, 84, 12);
            gfx_SetColor(optix_guicolors.colorb);
            gfx_Rectangle(214, 228, 84, 12);
            optix_CusText(false);
            gfx_PrintStringXY("<", 220, 230);
            gfx_PrintStringXY(">", 284, 230);
        }
    }
    gfx_SetTransparentColor(255);
    //show the pointer if we're hovering over a menu
    if (optix_guidata.currmenu == OPTIX_MENU_INVALID) gfx_TransparentSprite(cursor_normal, optix_cursor.x - 8, optix_cursor.y - 8);
    else gfx_TransparentSprite(cursor_pointer, optix_cursor.x - 8, optix_cursor.y - 8);
    /*else gfx_SetColor(optix_guicolors.ttextfgcolor);
    gfx_Circle(optix_cursor.x, optix_cursor.y, 5);
    gfx_SetColor(optix_guicolors.selectcolor);
    gfx_Circle(optix_cursor.x, optix_cursor.y, 4);*/
    gfx_SetDraw(1);
}


//just don't ask please
//alright this is getting pretty annoying just work you asdkjdfs;ljkafsd;jlksjfda;f;sdjlkda;jlkas;ljksda;jlka;kjs;jasdfadjs;lkoijiyweapo
//listen here you little son of a bioth;oirty;jlnfds;lkjf;lkfsdj;lk
void optix_HandleTrackPad(void) {
    uint16_t x;
    uint8_t y;
    kb_key_t key;
    optix_guidata.skkey = os_GetCSC();
    x = 0;
    y = 0;
    //thanks Michael2_3B (maybe will optimize)
    if (kb_Data[2]) {
        key = kb_Data[2];
        x=32;
        if(key == kb_Sto) y=188;
        if(key == kb_Ln) y=154;
        if(key == kb_Log) y=120;
        if(key == kb_Square) y=85;
        if(key == kb_Recip) y=51;
        if(key == kb_Math) y=17;
    }
    if (kb_Data[3]) {
        key = kb_Data[3];
        x=96;
        if(key == kb_0) y=222;
        if(key == kb_1) y=188;
        if(key == kb_4) y=154;
        if(key == kb_7) y=120;
        if(key == kb_Comma) y=85;
        if(key == kb_Sin) y=51;
        if(key == kb_Apps) y=17;
    }
    if (kb_Data[4]) {
        key = kb_Data[4];
        x=160;
        if(key == kb_DecPnt) y=222;
        if(key == kb_2) y=188;
        if(key == kb_5) y=154;
        if(key == kb_8) y=120;
        if(key == kb_LParen) y=85;
        if(key == kb_Cos) y=51;
        if(key == kb_Prgm) y=17;
    }
    if (kb_Data[5]) {
        key = kb_Data[5];
        x=224;
        if(key == kb_Chs) y=222;
        if(key == kb_3) y=188;
        if(key == kb_6) y=154;
        if(key == kb_9) y=120;
        if(key == kb_RParen) y=85;
        if(key == kb_Tan) y=51;
        if(key == kb_Vars) y=17;
    }
    if (kb_Data[6]) {
        key = kb_Data[6];
        x=288;
        if(key == kb_Enter) y=222;
        if(key == kb_Add) y=188;
        if(key == kb_Sub) y=154;
        if(key == kb_Mul) y=120;
        if(key == kb_Div) y=85;
        if(key == kb_Power) y=51;
        if(key == kb_Clear) y=17;
    }
    if (x != 0 && y != 0) {
        //we need to know to start where the cursor was before
        //reset according to new location
        if (optix_guidata.loopssincepress > 2) {
            optix_guidata.trackpadoriginx = optix_cursor.x;
            optix_guidata.trackpadoriginy = optix_cursor.y;
            optix_guidata.trackpadlastx = x;
            optix_guidata.trackpadlasty = y;
            optix_guidata.loopssincepress = 0;
        }
        //if you're moving the cursor more slowly, it should more slower
        optix_cursor.x = optix_guidata.trackpadoriginx + (x - optix_guidata.trackpadlastx) / 3;
        optix_cursor.y = optix_guidata.trackpadoriginy + (y - optix_guidata.trackpadlasty) / 3;
        //more things
        optix_guidata.loopssincepress = 0;
        //guidata.trackpadpressedlast = true;
    } else {
        //guidata.trackpadpressedlast = false;
        optix_guidata.loopssincepress++;
    }
}

//this stuff was formerly in button.c
//now old and we don't need it anymore



//also updates which button the cursor is currently over
/*void optix_ClickCursor(void) {
    uint8_t i;
    if ((kb_Data[6] & kb_Enter) && (optix_guidata.keypress)) {
        i = 0;
        for (i = 0; i < optix_buttoninfo.numbuttons; i++) {
            struct optix_button_t *b = &optix_button[i];
            if (optix_cursor.x - b->x < b->width && optix_cursor.y - b->y < b->height && optix_cursor.x - b->x > 0 && optix_cursor.y - b->y > 0) {
                b->funct();
                break;
            }
        }
        optix_guidata.keypress = false;
    } else {
        i = 0;
        for (i = 0; i < optix_buttoninfo.numbuttons; i++) {
            struct optix_button_t *b = &optix_button[i];
            if (optix_cursor.x - b->x < b->width && optix_cursor.y - b->y < b->height && optix_cursor.x - b->x > 0 && optix_cursor.y - b->y > 0) {
                optix_buttoninfo.currbutton = i;
                break;
            } else {
                //make sure we aren't selecting buttons we shouldn't be
                optix_buttoninfo.currbutton = optix_buttoninfo.numbuttons + 1;
            }
        }
    }
}


void optix_InitializeButtons(void) {
    optix_button = (struct optix_button_t*) malloc(0 * sizeof(struct optix_button_t));
}

void optix_AddButton(uint16_t x, uint8_t y, uint16_t width, uint8_t height, const char text[15], gfx_sprite_t *spr, void (*funct)(void), sk_key_t altkey) {
    if (optix_button = realloc(optix_button, (optix_buttoninfo.numbuttons + 1) * sizeof(struct optix_button_t))) {
        optix_button[optix_buttoninfo.numbuttons].x = x;
        optix_button[optix_buttoninfo.numbuttons].y = y;
        optix_button[optix_buttoninfo.numbuttons].width = width;
        optix_button[optix_buttoninfo.numbuttons].height = height;
        strcpy(optix_button[optix_buttoninfo.numbuttons].text, text);
        optix_button[optix_buttoninfo.numbuttons].spr = spr;
        optix_button[optix_buttoninfo.numbuttons].funct = funct;
        optix_button[optix_buttoninfo.numbuttons].altkey = altkey;
        optix_buttoninfo.numbuttons++;
    } else {
        optix_Message("ERROR 01", "Failed to reallocate space in the dynamic array for a new button. Please submit a bug report!", 10, 150, 3);
    }
}
//untested so be careful
void optix_DeleteButton(uint8_t buttontodelete) {
    uint8_t i;
    i = 0;
    for (i = optix_buttoninfo.numbuttons; i > buttontodelete; i--) optix_button[i - 1] = optix_button[i];
    optix_buttoninfo.numbuttons--;
    optix_button = realloc(optix_button, optix_buttoninfo.numbuttons * sizeof(*optix_button));
}

void optix_RenderButtons(void) {
    uint8_t i;
    struct optix_button_t *b;
    i = 0;
    optix_CusText(false);
    for (i = 0; i < optix_buttoninfo.numbuttons; i++) {
        b = &optix_button[i];
        gfx_SetColor(optix_guicolors.colora);
        if (i == optix_buttoninfo.currbutton) {
            //are you serious
            //ffs just work, it's not that hard to know what a null pointer is yhou bitkhlclk
            //I mean I like looking at unmapped memory and everything but this isn't that hard
            if (b->spr == NULL) gfx_FillRectangle(b->x, b->y, b->width, b->height);
            else {
                //make the box a little wider to show an outline
                //could mess up hitboxes somewhat
                gfx_FillRectangle(b->x - 2, b->y - 2, b->width + 4, b->height + 4);
                gfx_TransparentSprite(b->spr, b->x, b->y);
            }
        }
        gfx_PrintStringXY(b->text, (b->x + b->width / 2) - gfx_GetStringWidth(b->text) / 2, (b->y + b->height / 2) - 4);
    }
    //Let's hope that works
}

//all of the values in this function are pretty random, may need changing later on but seems to work for now
void optix_UpdateSelectedButton(void) {
    int tryx;
    int tryy;
    uint8_t closestbutton;
    int closestbuttonscore;
    int buttonscore;
    uint8_t i;
    struct optix_button_t *c;
    i = 0;
    tryx = 0;
    tryy = 0;
    closestbuttonscore = 10000;
    closestbutton = 0;
    buttonscore = 0;
    if (optix_guidata.keypress) {
        kb_Scan();
        if (kb_Data[7] & kb_Left) tryx--;
        if (kb_Data[7] & kb_Right) tryx++;
        if (kb_Data[7] & kb_Up) tryy--;
        if (kb_Data[7] & kb_Down) tryy++;
        if (tryx == 0 && tryy == 0) return;
        c = &optix_button[optix_buttoninfo.currbutton];
        for (i = 0; i < optix_buttoninfo.numbuttons; i++) {
            struct optix_button_t *b = &optix_button[i];
            //the first check, assume that if the key has an alt code it can't be selected when not in cursor mode
            //button can't be selected if there is a shortcut for it pretty much (like the start key, which will be
            //F1, no point in having a jump to that, but you will be able to click on it with the cursor if you want)
            if (b->altkey != 0) continue;
            if (tryx == -1) {
                if (b->x < c->x) buttonscore = (c->x - b->x) + (abs(b->y - c->y)) * optix_guisettings.buttonsensitivity;
                else buttonscore = 10000;
            } else if (tryx == 1) {
                if (b->x > c->x) buttonscore = (b->x - c->x) + (abs(b->y - c->y)) * optix_guisettings.buttonsensitivity;
                else buttonscore = 10000;
            } else if (tryy == -1) {
                if (b->y < c->y) buttonscore = (c->y - b->y) + (abs(b->x - c->x)) * optix_guisettings.buttonsensitivity;
                else buttonscore = 10000;
            } else if (tryy == 1) {
                if (b->y > c->y) buttonscore = (b->y - c->y) + (abs(b->x - c->x)) * optix_guisettings.buttonsensitivity;
                else buttonscore = 10000;
            } else closestbuttonscore = 10000;
            if (buttonscore < closestbuttonscore) {
                closestbutton = i;
                closestbuttonscore = buttonscore;
            }
        }
        if (closestbuttonscore < 5000) optix_buttoninfo.currbutton = closestbutton;
        optix_guidata.keypress = false;
    }
}

void optix_ClickButton(void) {
    struct optix_button_t *b = &optix_button[optix_buttoninfo.currbutton];
    if ((kb_Data[6] & kb_Enter) && (optix_guidata.keypress)) {
        //not really sure why this is here but I guess there isn't a bug with the functions after all
        //gfx_FillScreen(224);
        //gfx_SwapDraw();
        b->funct();
        optix_guidata.keypress = false;
    }
}

//cool, altkeys (will work in both cursor and box modes, I think)
void optix_CheckForAltKey(void) {
    uint8_t i;
    for (i = 0; i < optix_buttoninfo.numbuttons; i++) {
        struct optix_button_t *b = &optix_button[i];
        //could be problematic if altkey is set to 0 so fixed it
        if (b->altkey == optix_guidata.skkey && b->altkey != 0) b->funct();
    }
}*/


//these will be irrelevant when we can set custom colors