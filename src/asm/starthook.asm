include 'include/ez80.inc'
include 'include/tiformat.inc'
include 'include/ti84pceg.inc'

include 'include/hook_equates.inc'
include 'include/capnhook.inc'

flags		equ	$D00080

;basically taken from this thread: https://www.cemetech.net/forum/viewtopic.php?t=12920&highlight=

public _hook_start
_hook_start:
    db $83
    ld (ti.plotSScreen),a
    cp ti.kPrgm       ; was Prgms pressed?
    ret nz
    ld hl,$D007E0
    ld b,(hl)
    ld a,ti.kQuit
    ;cp b
    ;jr nz,return
    call RelocBase
RelocBase:
    pop ix
    lea hl,ix-RelocBase+ShellPrgm ;actual program running stuff
    res   ti.progExecuting,(iy + ti.newDispF)
    res   ti.cmdExec,(iy + ti.cmdFlags)
    call ti.Mov9ToOP1
    call ti.ExecutePrgm
    ld a,0
    ld (ti.plotSScreen),a
    ld a,ti.cxCmd
    call ti.NewContext0
    call ti.SetupHome
return:
    ld a,(ti.plotSScreen)
    ; clear the hook
    ; VYSION will just make a new one
    ;jq	ti.ClrRawKeyHook
    ret
ShellPrgm:
    db ti.ProtProgObj,"VYSION", 0
hook_start_size := $-_hook_start

public _InstallStartHook
_InstallStartHook:
    ld	hl,_hook_start
	ld	de,ti.plotSScreen
	ld	bc,hook_start_size
	ldir
	ld	hl,ti.plotSScreen
    ;jp ti.plotSScreen
	jq	ti.SetGetKeyHook
