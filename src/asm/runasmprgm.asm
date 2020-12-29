public _RunAsmPrgm

include 'include/ez80.inc'
include 'include/tiformat.inc'
include 'include/ti84pceg.inc'

;rework this
_RunAsmPrgm:
copy_to_saferam:
   ;fix the return stuff
   ld   sp,(__exitsp)
   pop   iy,af,hl
   ld   (hl),a
   call   $04F0
   ld hl, run_asm_prgm_addr
   ld de, ti.vRam
   ld bc, lengthof code
   ldir
   call ti.PushOP1
   ld hl, program_name
   call ti.Mov9ToOP1
   ;OP1 to be reloaded later
   ld hl,(ti.OP1)
   ld (tmp0),hl
   ld hl, (ti.OP1 + 3)
   ld (tmp1),hl
   ld hl, (ti.OP1 + 6)
   ld (tmp2),hl
   ;this part will also copy the program to be run and its type to OP1
   call ti.PopOP1
   jp run_asm_prgm
program_name:
   db ti.ProtProgObj, "VYSION", 0
 
run_asm_prgm_addr:
   ;org   ti.vRam
   virtual at ti.vRam
run_asm_prgm:
   extern __exitsp
   ld   de,(ti.asm_prgm_size)
   ld hl, 0
   ld   (ti.asm_prgm_size), hl
   ld   hl,ti.userMem         ; delete the current program from usermem
   call   ti.DelMem
   ;attempt to copy the reloader to the stack and set up a return address
   ;not sure how to do negative numbers but maybe this would work?
   ;should end up with -(reloader_end - reloader_start)
   ld hl,reloader_start - reloader_end
   add hl,sp
   ld sp,hl
   ld (.returnaddr),hl
   ex de,hl
   ld hl,reloader_start
   ld bc,reloader_end - reloader_start
   ldir
   ld hl,reloader_error - reloader_start
   add hl,sp
   call ti.PushErrorHandler
   ;run the asm program
   ;we stored the name and type into OP1 earlier
   res 0,(iy + ti.asm_Flag2)
   set   ti.graphDraw,(iy + ti.graphFlags)
   set   ti.appTextSave,(iy + ti.appFlags)
   set   ti.progExecuting,(iy + ti.newDispF)
   set   ti.appAutoScroll,(iy + ti.appFlags)
   set   ti.cmdExec,(iy + ti.cmdFlags)
   res   ti.onInterrupt,(iy + ti.onFlags)
   xor   a,a
   ld   (ti.kbdGetKy),a
   call   ti.EnableAPD
   ei
.getprgm:
   call   ti.ChkFindSym
   jr   c,.error_not_found
   call   ti.ChkInRam
   ex   de,hl
   jr   z,.in_ram
   ld   bc,9
   add   hl,bc
   ld   c,(hl)
   add   hl,bc
   inc   hl
.in_ram:
   call   ti.LoadDEInd_s
   bit 0,(iy + ti.asm_Flag2)
   jr  z,.getsize
   push    de
   pop bc
   inc hl
   inc hl
   ld  de,ti.userMem
   ldir
   call   ti.DisableAPD
   di
   ;ld hl,reloader_end - reloader_start
   ;add hl,sp
   ;ld sp,hl
   ld hl,0
.returnaddr := $-3
   push hl
   ;jq   ti.ParseInp
   jq ti.userMem
.getsize:
   push    de
   ex  de,hl
   call   ti.EnoughMem
   pop hl
   jr   c,.error_mem
   push    hl
   ld   de,ti.userMem
   call   ti.InsertMem
   pop   bc
   ld   (ti.asm_prgm_size),bc
   set 0,(iy + ti.asm_Flag2)
   jr  .getprgm
.error_mem:
.error_not_found:
.error_on:
   ;di
   ;ld hl,reloader_end - reloader_start
   ;add hl,sp
   ;ld sp,hl
   ret
 
reloader_start:
   call    ti.PopErrorHandler
reloader_error:
   ld iy,ti.flags
	ld hl,ti.userMem
	ld de,(ti.asm_prgm_size)
	call ti.DelMem
   di
   res   ti.progExecuting,(iy + ti.newDispF)
   res   ti.cmdExec,(iy + ti.cmdFlags)
   bit   ti.onInterrupt,(iy + ti.onFlags)
   jr  nz,.error_on
   ld hl, 0
tmp0 := $-3
   ld (ti.OP1),hl
   ld hl,0
tmp1 := $-3
   ld (ti.OP1 + 3),hl
   ld hl,0
tmp2 := $-3
   ld (ti.OP1 + 6), hl
   res 0,(iy + ti.asm_Flag2)
.getprgm:
   call   ti.ChkFindSym
   jr   c,.error_not_found
   call   ti.ChkInRam
   ex   de,hl
   jr   z,.in_ram
   ld   bc,9
   add   hl,bc
   ld   c,(hl)
   add   hl,bc
   inc   hl
.in_ram:
   call   ti.LoadDEInd_s
   bit 0,(iy + ti.asm_Flag2)
   jr  z,.getsize
   push    de
   pop bc
   inc hl
   inc hl
   ld  de,ti.userMem
   ldir
   call   ti.DisableAPD
   di
   ld hl,reloader_end - reloader_start
   add hl,sp
   ld sp,hl
   jp  ti.userMem
.getsize:
   push    de
   ex  de,hl
   call   ti.EnoughMem
   pop hl
   jr   c,.error_mem
   push    hl
   ld   de,ti.userMem
   call   ti.InsertMem
   pop   bc
   ld   (ti.asm_prgm_size),bc
   set 0,(iy + ti.asm_Flag2)
   jr  .getprgm
.error_mem:
.error_not_found:
.error_on:
   di
   ld hl,reloader_end - reloader_start
   add hl,sp
   ld sp,hl
   ret
reloader_end:
   load code: $-run_asm_prgm from run_asm_prgm
   end virtual
   db code
run_asm_prgm_end: