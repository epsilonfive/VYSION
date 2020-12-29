#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

#define OPTIX_MENU_INVALID -1
#define OPTIX_TICKS_SCROLL 7500

// function declarations
void optix_SetActiveMenus(int activemenus[], int nummenus);
void optix_ClearActiveMenus(void);
void optix_SaveActiveMenus(void);
void optix_RestoreActiveMenus(void);

void optix_WhiText(void);
void optix_BlaText(void);
void optix_CusText(bool title);
void optix_RenderWindow(const char title[], uint16_t width, uint8_t height);
uint8_t optix_WordWrap(const char message[], uint16_t width);
int optix_GetStringLength(const char string[]);
void optix_Dialogue(const char title[], const char message[], uint8_t textspacing, uint16_t width, uint8_t maxlines);
void optix_Message(const char title[], const char message[], uint8_t textspacing, uint16_t width, uint8_t maxlines);
void optix_AddWordWrapLine(const char string[], uint8_t *numlines);
void optix_PrintWordWrap(uint16_t width, uint8_t initialx, uint8_t initialy, uint8_t textspacing, uint8_t currline, uint8_t maxlines);
uint8_t optix_Menu(const char title[], const char entries[], uint8_t textspacing, uint16_t width, uint8_t maxlines);
void optix_SetAdvancedMenuCentering(uint8_t menu, uint8_t textxcentering, uint8_t textycentering, uint8_t sprxcentering, uint8_t sprycentering, uint8_t textxoffset,
                                    uint8_t textyoffset, uint8_t sprxoffset, uint8_t spryoffset);
void optix_SetMenuSprScale(uint8_t menu, uint8_t sprxscale, uint8_t spryscale);
uint16_t optix_GetWindowXCentering(uint8_t centering, uint16_t windowwidth, uint16_t width, uint16_t xspacing);
uint16_t optix_GetWindowYCentering(uint8_t centering, uint16_t windowheight, uint16_t height, uint16_t yspacing);
int optix_Slider(uint16_t x, uint8_t y, uint16_t width, uint8_t height, int min, int max, int initial);
//uint8_t optix_DispMenu(const char entries[], uint16_t width, bool print, uint8_t initialx, uint8_t initialy, uint8_t textspacing, uint8_t currline, uint8_t linestoprint, uint8_t currentselection);
int optix_SimpleMenu(uint16_t x, uint8_t y, uint16_t xspacing, uint8_t yspacing, const char *entries);
int optix_AddMenu(uint16_t x, uint8_t y, uint8_t xpadding, uint8_t ypadding, uint8_t width, uint8_t height, uint8_t xspacing, uint8_t yspacing,
                      const char text[], gfx_sprite_t **sprites);
void optix_UpdateCurrMenu(void);
void optix_RenderMenu(uint8_t menu);
void optix_DeleteLastMenu(void);
void optix_DeleteAllMenus(void);
uint8_t optix_ColorPicker(int initial);
uint8_t optix_InsertSpecialCharacter(void);
bool optix_UpdateStringInput(bool allowadd);
void optix_ResetStringInput(void);
char *optix_GetStringInput(const char title[], uint8_t textspacing, uint16_t width, int maxchars);
void optix_UpdateCursor(void);
void optix_RenderCursor(void);
void optix_ClickCursor(void);
void optix_HandleTrackPad(void);
void optix_InitializeButtons(void);
void optix_AddButton(uint16_t x, uint8_t y, uint16_t width, uint8_t height, const char text[], gfx_sprite_t *spr, void (*funct)(void), sk_key_t altkey);
void optix_DeleteButton(uint8_t buttontodelete);
void optix_RenderButtons(void);
void optix_UpdateSelectedButton(void);
void optix_ClickButton(void);
void optix_CheckForAltKey(void);
void optix_SetDefaultColors(void);
void optix_SetDefaultSettings(void);
void optix_HandleCursor(void);
void optix_HandleGUI(void);
void optix_BoolSwitch(bool on, uint16_t x, uint8_t y, uint8_t width, uint8_t height);
void optix_VertScrollbar(uint24_t x, uint8_t y, uint8_t currpos, uint8_t maxpos, uint8_t width, uint8_t height, bool keypressed);
void optix_HorizScrollbar(uint24_t x, uint8_t y, uint8_t currpos, uint8_t maxpos, uint24_t width, uint8_t height, bool keypressed);
void optix_Scrollbar(uint24_t x, uint8_t y, uint8_t currpos, uint8_t maxpos, uint24_t width, uint8_t height, bool keypressed, bool vert);

//variables
//rip
//extern char *optix_stringinput;

struct optix_button_t {
    uint16_t x;
    uint8_t y;
    uint16_t width;
    uint8_t height;
    char text[15];
    gfx_sprite_t *spr;
    void (*funct)(void);
    sk_key_t altkey;
};
extern struct optix_button_t *optix_button;

//could support different colors or scales on a line by line basis later?
//no I'm just lazy
struct optix_wordwraptext_t {
    char text[200];
    uint8_t useless;
};
extern struct optix_wordwraptext_t *optix_wordwraptext;


struct optix_buttoninfo_t {
    uint8_t numbuttons;
    uint8_t currbutton;
};
extern struct optix_buttoninfo_t optix_buttoninfo;

//variables
struct optix_cursor_t {
    uint16_t x;
    uint8_t y;
    bool cursoractive;
    bool trackpadactive;
    gfx_sprite_t *back;
};
extern struct optix_cursor_t optix_cursor;

struct optix_menu_t {
    uint16_t x;
    uint8_t y;
    uint8_t xpadding;
    uint8_t ypadding;
    uint8_t width;
    uint8_t height;
    uint8_t xspacing;
    uint8_t yspacing;
    char **text;
    int currselection;
    uint8_t maxtodisplay;
    int numoptions;
    bool enterpressed;
    //for the first option to be displayed (handles scrolling)
    uint8_t menumin;
    //for centering purposes, 0 = left (or top) aligned, 1 = centered, 2 = right (or bottom) aligned
    uint8_t textxcentering;
    uint8_t textycentering;
    uint8_t sprxcentering;
    uint8_t sprycentering;
    //the offset for all of these
    uint8_t textxoffset;
    uint8_t textyoffset;
    uint8_t sprxoffset;
    uint8_t spryoffset;
    uint8_t sprxscale;
    uint8_t spryscale;
    //an array eventually
    gfx_sprite_t **icon;
    bool displaytext;
};
extern struct optix_menu_t *optix_menu;

//will be able to parent buttons, sliders, text, etc. to boxes eventually
struct optix_box_t {
    uint16_t x;
    uint8_t y;
    uint16_t width;
    uint8_t height;
};
extern struct optix_box_t optix_box;

struct optix_guicolors_t {
    uint8_t bgcolor;
    uint8_t colora;
    uint8_t colorb;
    uint8_t selectcolor;
    uint8_t textfgcolor;
    uint8_t textbgcolor;
    uint8_t ttextfgcolor;
    uint8_t ttextbgcolor;
    uint8_t outlinecolor;
};
extern struct optix_guicolors_t optix_guicolors;

struct optix_guidata_t {
    //wow a lot of variables
    //maybe make these all go in the trackpad struct later
    int *activemenus;
    int numactivemenus;
    int *backactivemenus;
    int backnumactivemenus;
    //for holding an arrow key down and having it scroll
    int loopsarrowheld;
    int currmenu;
    uint8_t nummenus;
    bool keypress;
    sk_key_t skkey;
    bool trackpadpressedlast;
    uint16_t trackpadlastx;
    uint8_t trackpadlasty;
    uint16_t trackpadoriginx;
    uint8_t trackpadoriginy;
    uint8_t loopssincepress;
};
extern struct optix_guidata_t optix_guidata;

struct optix_stringinput_t {
    int numchars;
    char *text;
    //the current key map
    uint8_t currmap;
};
extern struct optix_stringinput_t optix_stringinput;


struct optix_guisettings_t {
    bool cursoron;
    bool dispbuttonback;
    uint8_t buttonsensitivity;
};
extern struct optix_guisettings_t optix_guisettings;

