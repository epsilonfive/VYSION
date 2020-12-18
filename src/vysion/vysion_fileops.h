#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

//functions
//file operations
void vysion_DeleteFolder(int foldertodelete);
void vysion_DeleteFile(int filetodelete);
void vysion_AddFile(void);
void vysion_AddFolder(char name[9], int location);
void vysion_DetectAllFiles(bool fromstart);
int vysion_CompareFolderNames(void *a, void *b);
int vysion_CompareFileNames(void *a, void *b);
void vysion_SortFolders(void);
void vysion_SortFiles(void);
uint8_t vysion_GetFileType(uint8_t type);
void vysion_GetAsmIcons(void);
void vysion_GetBasicIcons(void);

//tempfile stuff (for making menus, the file explorer, etc.)
void vysion_AddFileToTempMenu(bool isfolder, int index);
void vysion_ClearTempMenu(void);
void vysion_SetTempMenu(int folder, int type, bool showfiles, bool showfolders);
void vysion_GetTempMenuIcons(void);
void vysion_GetTempMenuText(void);
void vysion_SetOP1(uint8_t type, char name[]);
void vysion_GetTaskbar(void);
void vysion_GetStartMenu(void);