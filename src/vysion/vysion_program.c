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
#include "vysion_program.h"
#include "vysion.h"
#include "optix/optix.h"

//Til I collapse I'm spillin these raps long as you feel em
//Til the day that I drop you'll never say that I'm not killin em
//cause when I am not, then I'ma stop pensnin em

//asm stuff
void RunPrgm();
void RunAsmPrgm();
void EditPrgm();

//LABEL
//PROGRAM RUNNING, COPYING, AND OTHER OPERATIONS
//to run programs (NOTE: file type is a VYSION file type, not TI-OS)
void vysion_RunProgram(const char *progname, uint8_t type) {
    ti_CloseAll();
    ti_var_t slota;
    ti_var_t slotb;
    uint8_t progtype;
    uint8_t asmtok[2] = {0xBB, 0x6A};
    uint8_t prgm = 0x5F;
    progtype = 0;
    if (type == VYSION_BASIC_TYPE) progtype = TI_PRGM_TYPE;
    else if (type == VYSION_ASM_TYPE || type == VYSION_C_TYPE || type == VYSION_ICE_TYPE || type == VYSION_PBASIC_TYPE) progtype = TI_PPRGM_TYPE;
    else return;
    //make sure this doesn't happen
    if (strcmp(progname, VYSION_NAME) == 0) {
        optix_Message("ERROR", "Running VYSION within VYSION is not allowed.", 12, 120, 8);
        return;
    }
    ti_CloseAll();
    if (slotb = ti_OpenVar(progname, "r", progtype)) {
        //clean up VYSION stuff
        vysion_SaveFilesystem();
        vysion_SaveData();
        optix_DeleteAllMenus();
        ti_CloseAll();
        gfx_End();
        //os_ClrHomeFull();
        os_ClrHome();
        //slota = ti_OpenVar(VYSION_TEMPFILE, "w+", TI_PRGM_TYPE);
        //if it's not a basic program copy the asm token
        vysion_SetOP1(progtype, progname);
        if (type != VYSION_BASIC_TYPE && type != VYSION_PBASIC_TYPE) RunAsmPrgm();
        else {
            InstallFixStopHook();
            RunPrgm();
        }
        //if (type != VYSION_BASIC_TYPE && type != VYSION_PBASIC_TYPE) ti_Write(&asmtok, sizeof(asmtok), 1, slota);
        //ti_Write(&prgm, sizeof(prgm), 1, slota);
        //ti_Write(progname, strlen(progname), 1, slota);
        /*gfx_FillScreen(optix_guicolors.bgcolor);
        optix_CusText(false);
        gfx_PrintStringXY(progname, 5, 5);
        gfx_SwapDraw();
        delay(1000);*/
        //ti_CloseAll();
        //make sure the stop token doesn't rape my reloader
        //InstallFixStopHook();
        //what the actual fuck was I thinking
        //RunAsmPrgm();
        /*if (progtype == TI_PRGM_TYPE) {
            gfx_End();
            os_ClrHomeFull();
            vysion_SetOP1(progtype, name);
            RunPrgm();
        }*/
    }
}

void vysion_EditProgram(const char *progname) {
    ti_var_t slot;
    slot = ti_OpenVar(progname, "r", TI_PRGM_TYPE);
    //if it's not archived edit it
    if (ti_IsArchived(slot) == 0) {
        //clean up VYSION stuff
        vysion_SaveFilesystem();
        vysion_SaveData();
        optix_DeleteAllMenus();
        ti_CloseAll();
        gfx_End();
        os_ClrHomeFull();
        os_ClrHome();
        EditPrgm(progname, 0);
    }
}

bool vysion_Delete(bool isfolder, int index) {
    uint8_t progtype;
    uint8_t type = !isfolder * vysion_file[index].type;
    if (isfolder) vysion_DeleteFolder(index);
    else {
        if (type == VYSION_BASIC_TYPE) progtype = TI_PRGM_TYPE;
        else if (type == VYSION_ASM_TYPE || type == VYSION_C_TYPE || type == VYSION_ICE_TYPE || type == VYSION_PBASIC_TYPE) progtype = TI_PPRGM_TYPE;
        else return false;
        if (ti_DeleteVar(vysion_file[index].name, progtype)) vysion_DeleteFile(index);
        else return false;
    }
    return true;
}

//Music is like magic, there's a certain feelin' you get
//When you real and you spit, and people are feelin' your shit
//This is your moment, and every single minute you spend
//Trying hold on to it cause you may never get it again
//So while you're in it, try to get as much shit as you can
//And when your run is over, just admit when it's at its end