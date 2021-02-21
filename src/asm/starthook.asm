public _InstallStartHook
public _ClearStartHook

include 'include/ez80.inc'
include 'include/tiformat.inc'
include 'include/ti84pceg.inc'

_hook_start_addr:
    virtual at ti.appData
_hook_start:
    db $83
    ;ld (ti.plotSScreen),a
    cp ti.kPrgm       ; was Prgms pressed?
    ret nz
    ;call ti.NewContext0
    ;check if there's enough memory
    ld hl,9
    call ti.EnoughMem
    jq c,NotEnoughMemory
    ;delete the entry program
    ld hl,Prog_Name
    ld de,ti.OP1
    ld bc,3     ; type (1) + name (1) + zero (1)
    ldir
    call ti.ChkFindSym
    ret c
    call ti.DelVar
    ;create the new program
    ld hl,Prog_Name
    call ti.Mov9ToOP1
    ld hl,9 ;the data is 9 bytes long
    ld a,05h
    call ti.CreateVar
    ;write the stuff to it
    inc de
    inc de
    ld hl, Prog_Data
    ld bc, Prog_Data_End - Prog_Data
    ldir
    ld a, ti.kEnter
    call ti.JForceCmd
    ret
NotEnoughMemory:
    ld hl, String_Mem
    call ti.PutS
    ret
NotInEditor:
    ld hl, String_Editor
    call ti.PutS
    ret
Prog_Name:
    db ti.ProgObj, "#", 0
Prog_Data:
    db $bb, $6a, ti.tProg, "VYSION"
Prog_Data_End:
String_Mem:
    db "Not enough memory...", 0
String_Editor:
    db "Not in editor...", 0
load code: $-_hook_start from _hook_start
end virtual
db code


_InstallStartHook:
    ld iy, ti.flags
    ld	hl,_hook_start_addr
	ld	de,ti.appData
	ld	bc,lengthof code
	ldir
	ld	hl,ti.appData
    ;jp ti.plotSScreen
	call ti.SetGetKeyHook
    set 0, (iy + ti.hookflags1)
    set 5, (iy + ti.hookflags1)
    call	ti.ClrScrn
	call	ti.HomeUp
    ld hl, String_Success
	call	ti.PutS
.getkey:
    ;wait for a keypress
	call	ti.GetCSC
	or	a,a
	jr	z,.getkey
    ret
String_Success:
    db "Successfully installed.", 0

_ClearStartHook:
    ;all of this is probably unnecessary
    ld iy, ti.flags
    call ti.ClrGetKeyHook
    call ti.ClrRawKeyHook
    res 0, (iy + ti.hookflags1)
    res 5, (iy + ti.hookflags1)
    ret